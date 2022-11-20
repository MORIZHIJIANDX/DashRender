#include "PCH.h"
#include "CommandContext.h"
#include "GraphicsCore.h"
#include "RootSignature.h"

namespace Dash
{
	FCommandContext* FCommandContextManager::AllocateContext(D3D12_COMMAND_LIST_TYPE type)
	{
		ASSERT(type < D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE && type >= 0);

		if (type < 0 || type > D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE)
		{
			return nullptr;
		}

		std::lock_guard<std::mutex> lock(mAllocationMutex);

		auto& availableContext = mAvailableContexts[type];

		FCommandContext* context = nullptr;
		if (availableContext.empty())
		{
			context = new FCommandContext(type);
			mContextPool[type].emplace_back(context);
		}
		else
		{	
			context = availableContext.front();
			context->Reset();
			availableContext.pop();
		}

		ASSERT(context != nullptr);
		ASSERT(context->mType == type);

		context->Initialize();

		return context;
	}

	void FCommandContextManager::FreeContext(uint64_t fenceValue, FCommandContext* context)
	{
		ASSERT(context != nullptr);
		std::lock_guard<std::mutex> lock(mAllocationMutex);
		FGraphicsCore::CommandListManager->RetiredUsedCommandList(fenceValue, context->GetCommandList());
		mAvailableContexts[context->mType].push(context);
	}

	void FCommandContextManager::ReleaseAllTrackObjects()
	{
		std::lock_guard<std::mutex> lock(mAllocationMutex);

		for (int32_t index = 0; index < 4; ++index)
		{
			for (auto& contextPtr : mContextPool[index])
			{
				contextPtr->ReleaseTrackedObjects();
			}
		}
	}

	void FCommandContextManager::Destroy()
	{
		for (uint32_t index = 0; index < 4; ++index)
		{
			mContextPool[index].clear();
		}
	}

	FCommandContext::~FCommandContext()
	{
		mDynamicViewDescriptor.Destroy();
		mDynamicSamplerDescriptor.Destroy();
		mLinearAllocator.Destroy();
	}

	FCommandContext::FCommandContext(D3D12_COMMAND_LIST_TYPE type)
		: mType(type)
		, mDynamicViewDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		, mDynamicSamplerDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
		, mLinearAllocator(FGpuLinearAllocator::AllocatorType::CpuExclusive)
	{
		ZeroMemory(mDescriptorHeaps, sizeof(mDescriptorHeaps));
	}


	FCommandContext& FCommandContext::Begin(const std::string& id /*= L""*/, D3D12_COMMAND_LIST_TYPE type /*= D3D12_COMMAND_LIST_TYPE_DIRECT*/)
	{
		FCommandContext* newContext = FGraphicsCore::ContextManager->AllocateContext(type);
		newContext->SetID(id);
		return *newContext;
	}

	uint64_t FCommandContext::Flush(bool waitForCompletion /*= false*/)
	{
		FlushResourceBarriers();

		uint64_t fenceValue = Execute();

		if (waitForCompletion)
		{
			FGraphicsCore::CommandQueueManager->WaitForFence(fenceValue);
		}

		mCommandList->Reset(false);

		if (mCurrentRootSignature)
		{
			if (mType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
			{
				mD3DCommandList->SetComputeRootSignature(mCurrentRootSignature);
			}
			else if (mType == D3D12_COMMAND_LIST_TYPE_DIRECT)
			{
				mD3DCommandList->SetGraphicsRootSignature(mCurrentRootSignature);
			}
		}

		if (mCurrentPipelineState)
		{
			mD3DCommandList->SetPipelineState(mCurrentPipelineState);
		}

		BindDescriptorHeaps();

		return fenceValue;
	}

	uint64_t FCommandContext::Finish(bool waitForCompletion /*= false*/)
	{
		FlushResourceBarriers();

		uint64_t fenceValue = Execute();

		mLinearAllocator.RetireUsedPages(fenceValue);

		if (waitForCompletion)
		{
			FGraphicsCore::CommandQueueManager->WaitForFence(fenceValue);
		}

		FGraphicsCore::ContextManager->FreeContext(fenceValue, this);

		return fenceValue;
	}

	FGraphicsCommandContext& FCommandContext::GetGraphicsCommandContext()
	{
		ASSERT(mType == D3D12_COMMAND_LIST_TYPE_DIRECT);
		return reinterpret_cast<FGraphicsCommandContext&>(*this);
	}

	void FCommandContext::TransitionBarrier(FGpuResource& resource, D3D12_RESOURCE_STATES newState, UINT subResource /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/, bool flushImmediate /*= false*/)
	{
		mResourceStateTracker.TransitionResource(resource, newState, subResource);

		if (flushImmediate)
		{
			FlushResourceBarriers();
		}
	}

	void FCommandContext::UAVBarrier(FGpuResource& resource, bool flushImmediate /*= false*/)
	{
		mResourceStateTracker.UAVBarrier(resource);

		if (flushImmediate)
		{
			FlushResourceBarriers();
		}
	}

	void FCommandContext::AliasingBarrier(FGpuResource& resourceBefore, FGpuResource& resourceAfter, bool flushImmediate /*= false*/)
	{
		mResourceStateTracker.AliasBarrier(resourceBefore, resourceAfter);

		if (flushImmediate)
		{
			FlushResourceBarriers();
		}
	}

	void FCommandContext::FlushResourceBarriers()
	{
		mResourceStateTracker.FlushResourceBarriers(mD3DCommandList);
	}

	void FCommandContext::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap)
	{
		if (mDescriptorHeaps[type] != heap)
		{
			mDescriptorHeaps[type] = heap;
			BindDescriptorHeaps();
		}
	}

	void FCommandContext::SetDescriptorHeaps(UINT count, D3D12_DESCRIPTOR_HEAP_TYPE types[], ID3D12DescriptorHeap* heaps[])
	{
		bool anyChanged = false;

		for (UINT index = 0; index < count; ++index)
		{
			if (mDescriptorHeaps[types[index]] != heaps[index])
			{
				mDescriptorHeaps[types[index]] = heaps[index];
				anyChanged = true;
			}
		}

		if (anyChanged)
		{
			BindDescriptorHeaps();
		}
	}

	void FCommandContext::SetPipelineState(const FPipelineStateObject& pso)
	{
		ID3D12PipelineState* pipelineState = pso.GetPipelineState();
		
		if (pipelineState == mCurrentPipelineState)
		{
			return;
		}

		mD3DCommandList->SetPipelineState(pipelineState);
		mCurrentPipelineState = pipelineState;
	}

	void FCommandContext::InitializeBuffer(FGpuBuffer& dest, const void* bufferData, size_t numBytes, size_t offset /*= 0*/)
	{
		FCommandContext& context = FCommandContext::Begin("InitializeBufferContext");

		FGpuLinearAllocator::FAllocation alloc = context.mLinearAllocator.Allocate(numBytes);
		memcpy(alloc.CpuAddress, bufferData, numBytes);

		context.TransitionBarrier(dest, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		context.mD3DCommandList->CopyBufferRegion(dest.GetResource(), offset, alloc.Resource.GetResource(), alloc.Offset, numBytes);
		context.TransitionBarrier(dest, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);

		context.Finish(true);
	}

	void FCommandContext::BindDescriptorHeaps()
	{
		UINT nonNullHeaps = 0;
		ID3D12DescriptorHeap* heapsToBind[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		for (UINT index = 0; index < nonNullHeaps; ++index)
		{
			ID3D12DescriptorHeap* heapPtr = mDescriptorHeaps[index];
			if (heapPtr != nullptr)
			{
				heapsToBind[nonNullHeaps++] = heapPtr;
			}
		}

		if (nonNullHeaps > 0)
		{
			mD3DCommandList->SetDescriptorHeaps(nonNullHeaps, heapsToBind);
		}
	}

	void FCommandContext::TrackResource(FGpuResource& resource)
	{
		mTrackedObjects.emplace_back(resource.GetResource());
	}

	void FCommandContext::ReleaseTrackedObjects()
	{
		mTrackedObjects.clear();
	}

	uint64_t FCommandContext::Execute()
	{
		FGpuResourcesStateTracker::Lock();

		FCommandList* flushBarrierCommand = FGraphicsCore::CommandListManager->RequestCommandList(mType);

		uint32_t flushedBarrierCount = mResourceStateTracker.FlushPendingResourceBarriers(flushBarrierCommand->GetCommandList());

		mResourceStateTracker.CommitFinalResourceStates();
		
		std::vector<FCommandList*> commandListsToExecute{ flushBarrierCommand , mCommandList};

		uint64_t fenceValue = FGraphicsCore::CommandQueueManager->GetQueue(mType).ExecuteCommandLists(commandListsToExecute);

		FGraphicsCore::CommandListManager->RetiredUsedCommandList(fenceValue, flushBarrierCommand);

		FGpuResourcesStateTracker::Unlock();

		return fenceValue;
	}

	void FCommandContext::Initialize()
	{
		mCommandList = FGraphicsCore::CommandListManager->RequestCommandList(mType);
		mD3DCommandList = mCommandList->GetCommandList();
	}

	void FCommandContext::Reset()
	{
		mResourceStateTracker.Reset();
		mDynamicViewDescriptor.Reset();
		mDynamicSamplerDescriptor.Reset();
		ReleaseTrackedObjects();

		mCurrentRootSignature = nullptr;
		mCurrentPipelineState = nullptr;
		mCommandList = nullptr;
		mD3DCommandList = nullptr;
	}

	void FGraphicsCommandContext::ClearUAV(FGpuConstantBuffer& target)
	{
		UAVBarrier(target, true);

		D3D12_GPU_DESCRIPTOR_HANDLE handle = mDynamicViewDescriptor.CopyAndSetDescriptor(*this, target.GetUnorderedAccessView());
		const UINT clearColor[4] = {};
		mD3DCommandList->ClearUnorderedAccessViewUint(handle, target.GetUnorderedAccessView(), target.GetResource(), clearColor, 0, nullptr);

		TrackResource(target);
	}

	void FGraphicsCommandContext::ClearUAV(FColorBuffer& target)
	{
		UAVBarrier(target, true);

		D3D12_GPU_DESCRIPTOR_HANDLE handle = mDynamicViewDescriptor.CopyAndSetDescriptor(*this, target.GetUnorderedAccessView());
		CD3DX12_RECT clearRect{0, 0, static_cast<LONG>(target.GetWidth()), static_cast<LONG>(target.GetHeight()) };
		mD3DCommandList->ClearUnorderedAccessViewFloat(handle, target.GetUnorderedAccessView(), target.GetResource(), target.GetClearColor().Data, 1, &clearRect);

		TrackResource(target);
	}

	void FGraphicsCommandContext::ClearColor(FColorBuffer& target, D3D12_RECT* rect /*= nullptr*/)
	{
		TransitionBarrier(target, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearRenderTargetView(target.GetRenderTargetView(), target.GetClearColor().Data, (rect == nullptr) ? 0 : 1, rect);

		TrackResource(target);
	}

	void FGraphicsCommandContext::ClearColor(FColorBuffer& target, const FLinearColor& color, D3D12_RECT* rect /*= nullptr*/)
	{
		TransitionBarrier(target, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearRenderTargetView(target.GetRenderTargetView(), color.Data, (rect == nullptr) ? 0 : 1, rect);

		TrackResource(target);
	}

	void FGraphicsCommandContext::ClearDepth(FDepthBuffer& target)
	{
		TransitionBarrier(target, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearDepthStencilView(target.GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH, target.GetClearDepth(), target.GetClearStencil(), 0, nullptr);

		TrackResource(target);
	}

	void FGraphicsCommandContext::ClearStencil(FDepthBuffer& target)
	{
		TransitionBarrier(target, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearDepthStencilView(target.GetDepthStencilView(), D3D12_CLEAR_FLAG_STENCIL, target.GetClearDepth(), target.GetClearStencil(), 0, nullptr);

		TrackResource(target);
	}

	void FGraphicsCommandContext::ClearDepthAndStencil(FDepthBuffer& target)
	{
		TransitionBarrier(target, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearDepthStencilView(target.GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, target.GetClearDepth(), target.GetClearStencil(), 0, nullptr);

		TrackResource(target);
	}

	void FGraphicsCommandContext::SetRootSignature(const FRootSignature& rootSignature)
	{
		if (rootSignature.GetSignature() == mCurrentRootSignature)
		{
			return;
		}

		mD3DCommandList->SetGraphicsRootSignature(rootSignature.GetSignature());

		mDynamicViewDescriptor.ParseRootSignature(rootSignature);
		mDynamicSamplerDescriptor.ParseRootSignature(rootSignature);
	}

	void FGraphicsCommandContext::SetRenderTargets(UINT numRTVs, FColorBuffer* rtvs)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandels;
		
		for (UINT index = 0; index < numRTVs; ++index)
		{
			rtvHandels.push_back(rtvs[index].GetRenderTargetView());
			TrackResource(rtvs[index]);
			TransitionBarrier(rtvs[index], D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

		mD3DCommandList->OMSetRenderTargets(numRTVs, rtvHandels.data(), false, nullptr);
	}

	void FGraphicsCommandContext::SetRenderTargets(UINT numRTVs, FColorBuffer* rtvs, FDepthBuffer& depthBuffer)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandels;

		for (UINT index = 0; index < numRTVs; ++index)
		{
			rtvHandels.push_back(rtvs[index].GetRenderTargetView());
			TrackResource(rtvs[index]);
			TransitionBarrier(rtvs[index], D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles = { depthBuffer.GetDepthStencilView() };
		mD3DCommandList->OMSetRenderTargets(numRTVs, rtvHandels.data(), false, handles.data());
		TrackResource(depthBuffer);
		TransitionBarrier(depthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}

	void FGraphicsCommandContext::SetViewport(const FViewport& vp)
	{
		D3D12_VIEWPORT viewport = vp.D3DViewport();
		mD3DCommandList->RSSetViewports(1, &viewport);
	}

	void FGraphicsCommandContext::SetViewport(Scalar x, Scalar y, Scalar w, Scalar h, Scalar minDepth /*= 0.0f*/, Scalar maxDepth /*= 1.0f*/)
	{
		FViewport vp{x, y, w, h, minDepth, maxDepth};
		D3D12_VIEWPORT viewport = vp.D3DViewport();
		mD3DCommandList->RSSetViewports(1, &viewport);
	}

	void FGraphicsCommandContext::SetScissor(const D3D12_RECT& rect)
	{
		mD3DCommandList->RSSetScissorRects(1, &rect);
	}

	void FGraphicsCommandContext::SetScissor(UINT left, UINT top, UINT right, UINT bottom)
	{
		CD3DX12_RECT rect{ static_cast<LONG>(left), static_cast<LONG>(top), static_cast<LONG>(right), static_cast<LONG>(bottom) };
		mD3DCommandList->RSSetScissorRects(1, &rect);
	}

	void FGraphicsCommandContext::SetViewportAndScissor(UINT x, UINT y, UINT width, UINT height)
	{
		SetViewport(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
		SetScissor(x, y, x + width, y + height);
	}

	void FGraphicsCommandContext::SetStencilRef(UINT stencilRef)
	{
		mD3DCommandList->OMSetStencilRef(stencilRef);
	}

	void FGraphicsCommandContext::SetBlendFactor(const FLinearColor& color)
	{
		mD3DCommandList->OMSetBlendFactor(color.Data);
	}

	void FGraphicsCommandContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY type)
	{
		mD3DCommandList->IASetPrimitiveTopology(type);
	}

	void FGraphicsCommandContext::Set32BitConstants(UINT rootIndex, UINT numConstants, const void* constants)
	{
		mD3DCommandList->SetGraphicsRoot32BitConstants(rootIndex, numConstants, constants, 0);
	}

	void FGraphicsCommandContext::SetRootConstantBufferView(UINT rootIndex, size_t sizeInBytes, const void* constants)
	{
		FGpuLinearAllocator::FAllocation alloc = mLinearAllocator.Allocate(sizeInBytes);
		memcpy(alloc.CpuAddress, constants, sizeInBytes);
		mDynamicViewDescriptor.StageInlineCBV(rootIndex, alloc.GpuAddress);
	}

	void FGraphicsCommandContext::SetRootConstantBufferView(UINT rootIndex, FGpuConstantBuffer& constantBuffer, size_t bufferOffset /*= 0*/, D3D12_RESOURCE_STATES stateAfter /*= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER*/)
	{
		TransitionBarrier(constantBuffer, stateAfter);
		mDynamicViewDescriptor.StageInlineCBV(rootIndex, constantBuffer.GetGpuVirtualAddress(bufferOffset));
		TrackResource(constantBuffer);
	}

	void FGraphicsCommandContext::SetRootShaderResourceView(UINT rootIndex, size_t sizeInBytes, const void* constants)
	{
		FGpuLinearAllocator::FAllocation alloc = mLinearAllocator.Allocate(sizeInBytes);
		memcpy(alloc.CpuAddress, constants, sizeInBytes);
		mDynamicViewDescriptor.StageInlineSRV(rootIndex, alloc.GpuAddress);
	}

	void FGraphicsCommandContext::SetRootShaderResourceView(UINT rootIndex, FGpuConstantBuffer& constantBuffer, size_t bufferOffset /*= 0*/, D3D12_RESOURCE_STATES stateAfter /*= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/)
	{
		TransitionBarrier(constantBuffer, stateAfter);
		mDynamicViewDescriptor.StageInlineSRV(rootIndex, constantBuffer.GetGpuVirtualAddress(bufferOffset));
		TrackResource(constantBuffer);
	}

	void FGraphicsCommandContext::SetRootUnorderAccessView(UINT rootIndex, FGpuConstantBuffer& constantBuffer, size_t bufferOffset /*= 0*/, D3D12_RESOURCE_STATES stateAfter /*= D3D12_RESOURCE_STATE_UNORDERED_ACCESS*/)
	{
		TransitionBarrier(constantBuffer, stateAfter);
		mDynamicViewDescriptor.StageInlineUAV(rootIndex, constantBuffer.GetGpuVirtualAddress(bufferOffset));
		TrackResource(constantBuffer);
	}

	void FGraphicsCommandContext::SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, FGpuConstantBuffer& buffer, D3D12_RESOURCE_STATES stateAfter /*= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/)
	{
		TransitionBarrier(buffer, stateAfter);
		mDynamicViewDescriptor.StageDescriptors(rootIndex, descriptorOffset, 1, buffer.GetShaderResourceView());
		TrackResource(buffer);
	}

	void FGraphicsCommandContext::SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, FColorBuffer& buffer, D3D12_RESOURCE_STATES stateAfter /*= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/, UINT firstSubResource /*= 0*/, UINT numSubResources /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
	{
		if (numSubResources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
		{
			for (UINT index = firstSubResource; index < (firstSubResource + numSubResources); ++index)
			{
				TransitionBarrier(buffer, stateAfter, index);
			}
		}
		else
		{
			TransitionBarrier(buffer, stateAfter);
		}

		mDynamicViewDescriptor.StageDescriptors(rootIndex, descriptorOffset, 1, buffer.GetShaderResourceView());
		TrackResource(buffer);
	}

	void FGraphicsCommandContext::SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, FDepthBuffer& buffer, D3D12_RESOURCE_STATES stateAfter /*= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE*/, UINT firstSubResource /*= 0*/, UINT numSubResources /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
	{
		if (numSubResources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
		{
			for (UINT index = firstSubResource; index < (firstSubResource + numSubResources); ++index)
			{
				TransitionBarrier(buffer, stateAfter, index);
			}
		}
		else
		{
			TransitionBarrier(buffer, stateAfter);
		}

		mDynamicViewDescriptor.StageDescriptors(rootIndex, descriptorOffset, 1, buffer.GetShaderResourceView());
		TrackResource(buffer);
	}

	void FGraphicsCommandContext::SetDynamicSampler(UINT rootIndex, UINT descriptorOffset, D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		mDynamicSamplerDescriptor.StageDescriptors(rootIndex, descriptorOffset, 1, handle);
	}

	void FGraphicsCommandContext::SetDynamicSamplers(UINT rootIndex, UINT descriptorOffset, UINT count, D3D12_CPU_DESCRIPTOR_HANDLE handles[])
	{
		for (UINT index = 0; index < count; ++index)
		{
			mDynamicSamplerDescriptor.StageDescriptors(rootIndex, descriptorOffset + index, 1, handles[index]);	
		}
	}

	void FGraphicsCommandContext::SetIndexBuffer(FGpuIndexBuffer& indexBuffer)
	{
		std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViews = { indexBuffer.GetIndexBufferView() };
		mD3DCommandList->IASetIndexBuffer(indexBufferViews.data());
	}

	void FGraphicsCommandContext::SetVertexBuffer(UINT slot, FGpuVertexBuffer& vertexBuffer)
	{
		SetVertexBuffers(slot, 1, &vertexBuffer);
	}

	void FGraphicsCommandContext::SetVertexBuffers(UINT startSlot, UINT count, FGpuVertexBuffer* vertexBuffer)
	{
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vetexBufferViews;
		for (UINT index = 0; index < count; ++index)
		{
			vetexBufferViews.emplace_back(vertexBuffer[index].GetVertexBufferView());
		}
		mD3DCommandList->IASetVertexBuffers(startSlot, 1, vetexBufferViews.data());
	}

	void FGraphicsCommandContext::SetDynamicIndexBuffer(size_t indexCount, const uint16_t* data)
	{
		size_t dataSize = indexCount * sizeof(uint16_t);
		FGpuLinearAllocator::FAllocation alloc = mLinearAllocator.Allocate(dataSize);
		memcpy(alloc.CpuAddress, data, dataSize);

		D3D12_INDEX_BUFFER_VIEW view = {};
		view.BufferLocation = alloc.GpuAddress;
		view.Format = DXGI_FORMAT_R16_UINT;
		view.SizeInBytes = static_cast<UINT>(dataSize);

		mD3DCommandList->IASetIndexBuffer(&view);
	}

	void FGraphicsCommandContext::SetDynamicVertexBuffer(UINT slot, size_t vertexCount, size_t vertexStride, const void* data)
	{
		size_t dataSize = vertexCount * vertexStride;
		FGpuLinearAllocator::FAllocation alloc = mLinearAllocator.Allocate(dataSize);
		memcpy(alloc.CpuAddress, data, dataSize);

		D3D12_VERTEX_BUFFER_VIEW view = {};
		view.BufferLocation = alloc.GpuAddress;
		view.SizeInBytes = static_cast<UINT>(dataSize);
		view.StrideInBytes = static_cast<UINT>(vertexStride);

		mD3DCommandList->IASetVertexBuffers(slot, 1, &view);
	}

	void FGraphicsCommandContext::Draw(UINT vertexCount, UINT vertexStartOffset /*= 0*/)
	{
		DrawInstanced(vertexCount, 1, vertexStartOffset, 0);
	}

	void FGraphicsCommandContext::DrawIndexed(UINT indexCount, UINT startIndexLocation /*= 0*/, UINT baseVertexLocation /*= 0*/)
	{
		DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
	}

	void FGraphicsCommandContext::DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation /*= 0*/, UINT startInstanceLocation /*= 0*/)
	{
		FlushResourceBarriers();
		mDynamicViewDescriptor.CommitStagedDescriptorsForDraw(*this);
		mDynamicSamplerDescriptor.CommitStagedDescriptorsForDraw(*this);
		mD3DCommandList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}

	void FGraphicsCommandContext::DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
	{
		FlushResourceBarriers();
		mDynamicViewDescriptor.CommitStagedDescriptorsForDraw(*this);
		mDynamicSamplerDescriptor.CommitStagedDescriptorsForDraw(*this);
		mD3DCommandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}

	void FGraphicsCommandContext::SetUnorderAccessView(UINT rootIndex, UINT descriptorOffset, FGpuConstantBuffer& buffer, D3D12_RESOURCE_STATES stateAfter /*= D3D12_RESOURCE_STATE_UNORDERED_ACCESS*/)
	{
		TransitionBarrier(buffer, stateAfter);
		mDynamicViewDescriptor.StageDescriptors(rootIndex, descriptorOffset, 1, buffer.GetUnorderedAccessView());
		TrackResource(buffer);
	}

	void FGraphicsCommandContext::SetUnorderAccessView(UINT rootIndex, UINT descriptorOffset, FColorBuffer& buffer, D3D12_RESOURCE_STATES stateAfter /*= D3D12_RESOURCE_STATE_UNORDERED_ACCESS*/, UINT firstSubResource /*= 0*/, UINT numSubResources /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
	{
		if (numSubResources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
		{
			for (UINT index = firstSubResource; index < (firstSubResource + numSubResources); ++index)
			{
				TransitionBarrier(buffer, stateAfter, index);
			}
		}
		else
		{
			TransitionBarrier(buffer, stateAfter);
		}

		mDynamicViewDescriptor.StageDescriptors(rootIndex, descriptorOffset, 1, buffer.GetUnorderedAccessView());
		TrackResource(buffer);
	}

}