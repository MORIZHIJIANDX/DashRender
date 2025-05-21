#include "PCH.h"
#include "CommandContext.h"
#include "GraphicsCore.h"
#include "RootSignature.h"
#include "SubResourceData.h"
#include <pix3.h>

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

		for (int32 contextType = 0; contextType < 4; ++contextType)
		{
			while (!mRetiredContexts[contextType].empty() && FGraphicsCore::CommandQueueManager->IsFenceCompleted(mRetiredContexts[contextType].front().first))
			{
				mRetiredContexts[contextType].front().second->ReleaseTrackedObjects();
				mAvailableContexts[contextType].push(mRetiredContexts[contextType].front().second);
				mRetiredContexts[contextType].pop_front();
			}
		}

		auto& availableContext = mAvailableContexts[type];

		FCommandContext* context = nullptr;
		if (availableContext.empty())
		{
			switch (type)
			{
			case D3D12_COMMAND_LIST_TYPE_DIRECT:
				context = new FGraphicsCommandContext();
				break;
			case D3D12_COMMAND_LIST_TYPE_COMPUTE:
				context = new FComputeCommandContext();
				break;
			case D3D12_COMMAND_LIST_TYPE_COPY:
				context = new FCopyCommandContext();
				break;
			default:
				break;
			}
			
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

	void FCommandContextManager::FreeContext(uint64 fenceValue, FCommandContext* context)
	{
		ASSERT(context != nullptr);
		std::lock_guard<std::mutex> lock(mAllocationMutex);
		FGraphicsCore::CommandListManager->RetiredUsedCommandList(fenceValue, context->GetCommandList());
		mRetiredContexts[context->mType].emplace_back(std::make_pair(fenceValue, context));
	}

	void FCommandContextManager::ResetAllContext()
	{
		std::lock_guard<std::mutex> lock(mAllocationMutex);

		for (int32 index = 0; index < 4; ++index)
		{
			for (auto& contextPtr : mContextPool[index])
			{
				contextPtr->Reset();
			}
		}
	}

	void FCommandContextManager::Destroy()
	{
		for (uint32 index = 0; index < 4; ++index)
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


	FCommandContext* FCommandContext::Begin(const std::string& id /*= L""*/, D3D12_COMMAND_LIST_TYPE type /*= D3D12_COMMAND_LIST_TYPE_DIRECT*/)
	{
		FCommandContext* newContext = FGraphicsCore::ContextManager->AllocateContext(type);
		newContext->PIXBeginEvent(id);
		return newContext;
	}

	void FCopyCommandContextBase::BeginQuery(FQueryHeapRef queryHeap, uint32 queryIndex)
	{
		D3D12_QUERY_TYPE queryType = D3DQueryType(queryHeap->GetDesc().type);
		mD3DCommandList->BeginQuery(queryHeap->D3DQueryHeap(), queryType, queryIndex);
	}

	void FCopyCommandContextBase::EndQuery(FQueryHeapRef queryHeap, uint32 queryIndex)
	{
		D3D12_QUERY_TYPE queryType = D3DQueryType(queryHeap->GetDesc().type);
		mD3DCommandList->EndQuery(queryHeap->D3DQueryHeap(), queryType, queryIndex);
	}

	void FCopyCommandContextBase::ResolveQueryData(FQueryHeapRef queryHeap, uint32 startIndex, uint32 numQueries, FGpuBufferRef dest, uint32 destOffset)
	{
		TransitionBarrier(dest, EResourceState::CopyDestination, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);

		D3D12_QUERY_TYPE queryType = D3DQueryType(queryHeap->GetDesc().type);
		mD3DCommandList->ResolveQueryData(queryHeap->D3DQueryHeap(), queryType, startIndex, numQueries, dest->GetResource(), destOffset);
	}

	uint64 FCopyCommandContextBase::Flush(bool waitForCompletion /*= false*/)
	{
		FlushResourceBarriers();

		uint64 fenceValue = Execute();

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

	uint64 FCopyCommandContextBase::Finish(bool waitForCompletion /*= false*/)
	{
		FlushResourceBarriers();

		PIXEndEvent();

		uint64 fenceValue = Execute();

		if (waitForCompletion)
		{
			FGraphicsCore::CommandQueueManager->WaitForFence(fenceValue);
		}

		FGraphicsCore::ContextManager->FreeContext(fenceValue, this);
		mLinearAllocator.RetireUsedPages(fenceValue);

		return fenceValue;
	}

	void FCopyCommandContextBase::TransitionBarrier(FGpuResourceRef resource, EResourceState newState, UINT subResource /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/, bool flushImmediate /*= false*/)
	{
		mResourceStateTracker.TransitionResource(resource, D3DResourceState(newState), subResource);

		if (flushImmediate)
		{
			FlushResourceBarriers();
		}
	}

	void FCopyCommandContextBase::UAVBarrier(FGpuResourceRef resource, bool flushImmediate /*= false*/)
	{
		mResourceStateTracker.UAVBarrier(resource);

		if (flushImmediate)
		{
			FlushResourceBarriers();
		}
	}

	void FCopyCommandContextBase::AliasingBarrier(FGpuResourceRef resourceBefore, FGpuResourceRef resourceAfter, bool flushImmediate /*= false*/)
	{
		mResourceStateTracker.AliasBarrier(resourceBefore, resourceAfter);

		if (flushImmediate)
		{
			FlushResourceBarriers();
		}
	}

	void FCopyCommandContextBase::FlushResourceBarriers()
	{
		mResourceStateTracker.FlushResourceBarriers(mCommandList);
	}

	void FCopyCommandContextBase::InitializeBuffer(FGpuBufferRef dest, const void* bufferData, size_t numBytes, size_t offset /*= 0*/)
	{
		FCopyCommandContext& context = FCopyCommandContext::Begin("InitializeBufferContext");

		FGpuLinearAllocator::FAllocation alloc = context.mLinearAllocator.Allocate(numBytes);
		memcpy(alloc.CpuAddress, bufferData, numBytes);

		context.TransitionBarrier(dest, EResourceState::CopyDestination, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		context.mD3DCommandList->CopyBufferRegion(dest->GetResource(), offset, alloc.Resource.GetResource(), alloc.Offset, numBytes);
		context.TransitionBarrier(dest, EResourceState::Common, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);

		context.Finish(true);
	}

	void FCopyCommandContextBase::UpdateTextureBuffer(FTextureBufferRef dest, uint32 firstSubresource, uint32 numSubresources, const FSubResourceData* subresourceData)
	{
		ASSERT(subresourceData != nullptr);

		FCopyCommandContext& context = FCopyCommandContext::Begin("UpdateTextureBuffer");

		UINT64 requiredSize = GetRequiredIntermediateSize(dest->GetResource(), firstSubresource, numSubresources);
		FGpuLinearAllocator::FAllocation alloc = context.mLinearAllocator.Allocate(requiredSize);

		std::vector<D3D12_SUBRESOURCE_DATA> d3dSubResources;
		for (size_t i = 0; i < numSubresources; i++)
		{
			d3dSubResources.emplace_back(subresourceData[i].D3DSubResource());
		}

		// Resource must be in the copy-destination state.
		context.TransitionBarrier(dest, EResourceState::CopyDestination, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		UpdateSubresources(context.GetD3DCommandList(), dest->GetResource(), alloc.Resource.GetResource(), alloc.Offset, firstSubresource, numSubresources, d3dSubResources.data());
		context.Finish(true);
	}

	void FComputeCommandContextBase::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap)
	{
		if (mDescriptorHeaps[type] != heap)
		{
			mDescriptorHeaps[type] = heap;
			BindDescriptorHeaps();
		}
	}

	void FComputeCommandContextBase::SetDescriptorHeaps(UINT count, D3D12_DESCRIPTOR_HEAP_TYPE types[], ID3D12DescriptorHeap* heaps[])
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

	void FComputeCommandContextBase::SetComputePipelineState(FComputePSORef pso)
	{
		ASSERT(pso->IsFinalized());

		ID3D12PipelineState* pipelineState = pso->GetPipelineState();

		if (pipelineState == mCurrentPipelineState)
		{
			return;
		}

		SetComputeRootSignature(pso->GetRootSignature());
		mD3DCommandList->SetPipelineState(pipelineState);
		mCurrentPipelineState = pipelineState;

		mPSORef = pso;

		InitParameterBindState();
	}

	void FComputeCommandContextBase::SetComputeRootSignature(FRootSignatureRef rootSignature)
	{
		ASSERT(rootSignature != nullptr);
		ASSERT(rootSignature->IsFinalized());

		if (rootSignature->GetSignature() == mCurrentRootSignature)
		{
			return;
		}

		mD3DCommandList->SetComputeRootSignature(rootSignature->GetSignature());

		mDynamicViewDescriptor.ParseRootSignature(*rootSignature);
		mDynamicSamplerDescriptor.ParseRootSignature(*rootSignature);
	}

	void FCommandContext::PIXBeginEvent(const std::string& label)
	{
#ifdef DASH_DEBUG
		::PIXBeginEvent(mCommandList->GetCommandList(), 0, label.c_str());
#endif
	}

	void FCommandContext::PIXEndEvent()
	{
#ifdef DASH_DEBUG
		::PIXEndEvent(mCommandList->GetCommandList());
#endif
	}

	void FCommandContext::PIXSetMarker(const std::string& label)
	{
#ifdef DASH_DEBUG
		::PIXSetMarker(mCommandList->GetCommandList(), 0, label.c_str());
#endif	
	}

	void FCommandContext::BindDescriptorHeaps()
	{
		UINT nonNullHeaps = 0;
		ID3D12DescriptorHeap* heapsToBind[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		for (UINT index = 0; index < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++index)
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

	void FCommandContext::TrackResource(FGpuResourceRef resource)
	{
		mTrackedObjects.emplace_back(resource);
	}

	void FCommandContext::ReleaseTrackedObjects()
	{
		mTrackedObjects.clear();
	}

	uint64 FCommandContext::Execute()
	{
		FGpuResourcesStateTracker::Lock();

		FCommandList* flushBarrierCommand = mResourceStateTracker.FlushPendingResourceBarriers(mType);

		mResourceStateTracker.CommitFinalResourceStates();
		
		if (flushBarrierCommand != nullptr)
		{
			std::vector<FCommandList*> commandListsToExecute{ flushBarrierCommand, mCommandList };

			uint64 fenceValue = FGraphicsCore::CommandQueueManager->GetQueue(mType).ExecuteCommandLists(commandListsToExecute);

			FGraphicsCore::CommandListManager->RetiredUsedCommandList(fenceValue, flushBarrierCommand);

			FGpuResourcesStateTracker::Unlock();

			return fenceValue;
		}
		else
		{
			std::vector<FCommandList*> commandListsToExecute{ mCommandList };

			uint64 fenceValue = FGraphicsCore::CommandQueueManager->GetQueue(mType).ExecuteCommandLists(commandListsToExecute);

			FGpuResourcesStateTracker::Unlock();

			return fenceValue;
		}
	}

	void FCommandContext::InitParameterBindState()
	{
		if (mPSORef == nullptr)
		{
			return;
		}
		
		ASSERT_MSG(mPSORef->GetShaderPass() != nullptr, "Shader Pass Is Not Set.");

		FShaderPassRef shaderPass = mPSORef->GetShaderPass();

		InitStateForParameter(shaderPass->GetCBVParameterNum(), mConstantBufferBindState);
		InitStateForParameter(shaderPass->GetSRVParameterNum(), mShaderResourceViewBindState);
		InitStateForParameter(shaderPass->GetUAVParameterNum(), mUnorderAccessViewBindState);
		InitStateForParameter(shaderPass->GetSamplerParameterNum(), mSamplerBindState);
	}

	void FCommandContext::InitStateForParameter(size_t parameterNum, std::vector<bool>& bindStateMap)
	{
		bindStateMap.clear();
		for (size_t parameterIndex = 0; parameterIndex < parameterNum; ++parameterIndex)
		{
			bindStateMap.emplace_back(false);
		}	
	}

	void FCommandContext::CheckUnboundShaderParameters()
	{
		auto checkUnboundParameterFunc = [](const std::vector<bool>& bindStateMap, const std::vector<FShaderParameter>& parameters, const std::string& type)
		{
			for (size_t parameterIndex = 0; parameterIndex < bindStateMap.size(); ++parameterIndex)
			{
				if (bindStateMap[parameterIndex] == false)
				{
					LOG_WARNING << "Shader Parameter : " << parameters[parameterIndex].Name << ", Type : " << type << ", Is Not Bounded!";
				}
			}
		};

		checkUnboundParameterFunc(mConstantBufferBindState, mPSORef->GetShaderPass()->GetCBVParameters(), "Constant Buffer");
		checkUnboundParameterFunc(mShaderResourceViewBindState, mPSORef->GetShaderPass()->GetSRVParameters(), "Shader Resource View");
		checkUnboundParameterFunc(mUnorderAccessViewBindState, mPSORef->GetShaderPass()->GetUAVParameters(), "Unorder Access View");
		checkUnboundParameterFunc(mSamplerBindState, mPSORef->GetShaderPass()->GetCBVParameters(), "Sampler");
	}

	void FCommandContext::Initialize()
	{
		mCommandList = FGraphicsCore::CommandListManager->RequestCommandList(mType);
		mD3DCommandList = mCommandList->GetCommandList();
	}

	void FCommandContext::Reset()
	{
		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			mDescriptorHeaps[i] = nullptr;
		}

		mResourceStateTracker.Reset();
		mDynamicViewDescriptor.Reset();
		mDynamicSamplerDescriptor.Reset();
		ReleaseTrackedObjects();

		mCurrentRootSignature = nullptr;
		mCurrentPipelineState = nullptr;
		mPSORef = nullptr;
		mCommandList = nullptr;
		mD3DCommandList = nullptr;
	}

	void FComputeCommandContextBase::ClearUAV(FGpuBufferRef target)
	{
		if (!target->SupportUnorderedAccessView())
		{
			return;
		}

		UAVBarrier(target, true);

		D3D12_GPU_DESCRIPTOR_HANDLE handle = mDynamicViewDescriptor.CopyAndSetDescriptor(*this, target->GetUnorderedAccessView());
		const UINT clearColor[4] = {};
		mD3DCommandList->ClearUnorderedAccessViewUint(handle, target->GetUnorderedAccessView(), target->GetResource(), clearColor, 0, nullptr);

		TrackResource(target);
	}

	void FComputeCommandContextBase::ClearUAV(FColorBufferRef target)
	{
		UAVBarrier(target, true);

		D3D12_GPU_DESCRIPTOR_HANDLE handle = mDynamicViewDescriptor.CopyAndSetDescriptor(*this, target->GetUnorderedAccessView());
		CD3DX12_RECT clearRect{0, 0, static_cast<LONG>(target->GetWidth()), static_cast<LONG>(target->GetHeight()) };
		mD3DCommandList->ClearUnorderedAccessViewFloat(handle, target->GetUnorderedAccessView(), target->GetResource(), target->GetClearColor().Data, 1, &clearRect);

		TrackResource(target);
	}

	void FComputeCommandContextBase::Dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ)
	{
		CheckUnboundShaderParameters();

		FlushResourceBarriers();
		mDynamicViewDescriptor.CommitStagedDescriptorsForDispatch(*this);
		mDynamicSamplerDescriptor.CommitStagedDescriptorsForDispatch(*this);

		mD3DCommandList->Dispatch(groupCountX, groupCountY, groupCountZ);
	}

	void FComputeCommandContextBase::SetRootConstantBufferView(UINT rootIndex, size_t sizeInBytes, const void* constants)
	{
		FGpuLinearAllocator::FAllocation alloc = mLinearAllocator.Allocate(sizeInBytes);
		memcpy(alloc.CpuAddress, constants, sizeInBytes);
		mDynamicViewDescriptor.StageInlineCBV(rootIndex, alloc.GpuAddress);
	}

	void FComputeCommandContextBase::SetRootConstantBufferView(const std::string& bufferName, size_t sizeInBytes, const void* constants)
	{
		ASSERT_MSG(mPSORef != nullptr, "Pipeline State Is Not Set.");

		FShaderPassRef shaderPass = mPSORef->GetShaderPass();

		if (shaderPass)
		{
			int32 shaderParameterIndex = shaderPass->FindCBVParameterByName(bufferName);
			if (shaderParameterIndex != INDEX_NONE)
			{
				const std::vector<FShaderParameter>& parameters = shaderPass->GetCBVParameters();
				ASSERT(sizeInBytes == parameters[shaderParameterIndex].Size);
				SetRootConstantBufferView(parameters[shaderParameterIndex].RootParameterIndex, sizeInBytes, constants);
				mConstantBufferBindState[shaderParameterIndex] = true;
			}
			else
			{
				LOG_WARNING << "Can't Find Constant Buffer Parameter : " << bufferName;
			}
		}
	}

	void FComputeCommandContextBase::SetShaderResourceView(const std::string& srvrName, FColorBufferRef buffer, EResourceState stateAfter, UINT firstSubResource, UINT numSubResources)
	{
		ASSERT_MSG(mPSORef != nullptr, "Pipeline State Is Not Set.");

		FShaderPassRef shaderPass = mPSORef->GetShaderPass();

		if (shaderPass)
		{
			int32 shaderParameterIndex = shaderPass->FindSRVParameterByName(srvrName);
			if (shaderParameterIndex != INDEX_NONE)
			{
				const std::vector<FShaderParameter>& parameters = shaderPass->GetSRVParameters();
				SetShaderResourceView(parameters[shaderParameterIndex].RootParameterIndex, parameters[shaderParameterIndex].DescriptorOffset, buffer, buffer->GetShaderResourceView(), stateAfter, firstSubResource, numSubResources);
				mShaderResourceViewBindState[shaderParameterIndex] = true;
			}
			else
			{
				LOG_WARNING << "Can't Find Shader Resource Parameter : " << srvrName;
			}
		}
	}

	void FComputeCommandContextBase::SetShaderResourceView(const std::string& srvrName, FTextureBufferRef buffer, EResourceState stateAfter, UINT firstSubResource, UINT numSubResources)
	{
		ASSERT_MSG(mPSORef != nullptr, "Pipeline State Is Not Set.");

		FShaderPassRef shaderPass = mPSORef->GetShaderPass();

		if (shaderPass)
		{
			int32 shaderParameterIndex = shaderPass->FindSRVParameterByName(srvrName);
			if (shaderParameterIndex != INDEX_NONE)
			{
				const std::vector<FShaderParameter>& parameters = shaderPass->GetSRVParameters();
				SetShaderResourceView(parameters[shaderParameterIndex].RootParameterIndex, parameters[shaderParameterIndex].DescriptorOffset, buffer, buffer->GetShaderResourceView(), stateAfter, firstSubResource, numSubResources);
				mShaderResourceViewBindState[shaderParameterIndex] = true;
			}
			else
			{
				LOG_WARNING << "Can't Find Shader Resource Parameter : " << srvrName;
			}
		}
	}

	void FComputeCommandContextBase::SetShaderResourceView(const std::string& srvrName, FStructuredBufferRef buffer, EResourceState stateAfter, UINT firstSubResource, UINT numSubResources)
	{
		ASSERT_MSG(mPSORef != nullptr, "Pipeline State Is Not Set.");

		FShaderPassRef shaderPass = mPSORef->GetShaderPass();

		if (shaderPass)
		{
			int32 shaderParameterIndex = shaderPass->FindSRVParameterByName(srvrName);
			if (shaderParameterIndex != INDEX_NONE)
			{
				const std::vector<FShaderParameter>& parameters = shaderPass->GetSRVParameters();
				SetShaderResourceView(parameters[shaderParameterIndex].RootParameterIndex, parameters[shaderParameterIndex].DescriptorOffset, buffer, buffer->GetShaderResourceView(), stateAfter, firstSubResource, numSubResources);
				mShaderResourceViewBindState[shaderParameterIndex] = true;
			}
			else
			{
				LOG_WARNING << "Can't Find Shader Resource Parameter : " << srvrName;
			}
		}
	}

	void FComputeCommandContextBase::SetUnorderAccessView(const std::string& uavName, FColorBufferRef buffer, EResourceState stateAfter, UINT firstSubResource, UINT numSubResources)
	{
		FShaderPassRef shaderPass = mPSORef->GetShaderPass();

		if (shaderPass)
		{
			int32 shaderParameterIndex = shaderPass->FindUAVParameterByName(uavName);
			if (shaderParameterIndex != INDEX_NONE)
			{
				const std::vector<FShaderParameter>& parameters = shaderPass->GetUAVParameters();
				SetUnorderAccessView(parameters[shaderParameterIndex].RootParameterIndex, parameters[shaderParameterIndex].DescriptorOffset, buffer, stateAfter, firstSubResource, numSubResources);
				mUnorderAccessViewBindState[shaderParameterIndex] = true;
			}
			else
			{
				LOG_WARNING << "Can't Find Unorder Access Parameter : " << uavName;
			}
		}
	}

	void FComputeCommandContextBase::SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, FDepthBufferRef buffer, EResourceState stateAfter /*= EResourceState::AnyShaderAccess*/, UINT firstSubResource /*= 0*/, UINT numSubResources /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
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

		mDynamicViewDescriptor.StageDescriptors(rootIndex, descriptorOffset, 1, buffer->GetShaderResourceView());
		TrackResource(buffer);
	}

	void FComputeCommandContextBase::SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, FGpuResourceRef resource, const D3D12_CPU_DESCRIPTOR_HANDLE& srcDescriptors,
		EResourceState stateAfter /*= EResourceState::AnyShaderAccess*/, UINT firstSubResource /*= 0*/,
		UINT numSubResources /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
	{
		if (numSubResources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
		{
			for (UINT index = firstSubResource; index < (firstSubResource + numSubResources); ++index)
			{
				TransitionBarrier(resource, stateAfter, index);
			}
		}
		else
		{
			TransitionBarrier(resource, stateAfter);
		}

		mDynamicViewDescriptor.StageDescriptors(rootIndex, descriptorOffset, 1, srcDescriptors);
		TrackResource(resource);
	}

	void FComputeCommandContextBase::SetUnorderAccessView(UINT rootIndex, UINT descriptorOffset, FColorBufferRef buffer, EResourceState stateAfter /*= EResourceState::UnorderedAccess*/, UINT firstSubResource /*= 0*/, UINT numSubResources /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
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

		mDynamicViewDescriptor.StageDescriptors(rootIndex, descriptorOffset, 1, buffer->GetUnorderedAccessView());
		TrackResource(buffer);
	}

	void FGraphicsCommandContextBase::ClearColor(FColorBufferRef target, D3D12_RECT* rect /*= nullptr*/)
	{
		TransitionBarrier(target, EResourceState::RenderTarget, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearRenderTargetView(target->GetRenderTargetView(), target->GetClearColor().Data, (rect == nullptr) ? 0 : 1, rect);

		TrackResource(target);
	}

	void FGraphicsCommandContextBase::ClearColor(FColorBufferRef target, const FLinearColor& color, D3D12_RECT* rect /*= nullptr*/)
	{
		TransitionBarrier(target, EResourceState::RenderTarget, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearRenderTargetView(target->GetRenderTargetView(), color.Data, (rect == nullptr) ? 0 : 1, rect);

		TrackResource(target);
	}

	void FGraphicsCommandContextBase::ClearDepth(FDepthBufferRef target)
	{
		TransitionBarrier(target, EResourceState::DepthWrite, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearDepthStencilView(target->GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH, target->GetClearDepth(), target->GetClearStencil(), 0, nullptr);

		TrackResource(target);
	}

	void FGraphicsCommandContextBase::ClearStencil(FDepthBufferRef target)
	{
		TransitionBarrier(target, EResourceState::DepthWrite, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearDepthStencilView(target->GetDepthStencilView(), D3D12_CLEAR_FLAG_STENCIL, target->GetClearDepth(), target->GetClearStencil(), 0, nullptr);

		TrackResource(target);
	}

	void FGraphicsCommandContextBase::ClearDepthAndStencil(FDepthBufferRef target)
	{
		TransitionBarrier(target, EResourceState::DepthWrite, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearDepthStencilView(target->GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, target->GetClearDepth(), target->GetClearStencil(), 0, nullptr);

		TrackResource(target);
	}

	void FGraphicsCommandContextBase::SetGraphicsPipelineState(FGraphicsPSORef pso)
	{
		ASSERT(pso->IsFinalized());

		ID3D12PipelineState* pipelineState = pso->GetPipelineState();

		if (pipelineState == mCurrentPipelineState)
		{
			return;
		}

		SetGraphicsRootSignature(pso->GetRootSignature());
		mD3DCommandList->SetPipelineState(pipelineState);
		SetPrimitiveTopology(pso->GetPrimitiveTopology());
		mCurrentPipelineState = pipelineState;

		mPSORef = pso;

		InitParameterBindState();
	}

	void FGraphicsCommandContextBase::SetGraphicsRootSignature(FRootSignatureRef rootSignature)
	{
		ASSERT(rootSignature != nullptr);
		ASSERT(rootSignature->IsFinalized());

		if (rootSignature->GetSignature() == mCurrentRootSignature)
		{
			return;
		}

		mD3DCommandList->SetGraphicsRootSignature(rootSignature->GetSignature());

		mDynamicViewDescriptor.ParseRootSignature(*rootSignature);
		mDynamicSamplerDescriptor.ParseRootSignature(*rootSignature);
	}

	void FGraphicsCommandContextBase::SetRenderTargets(UINT numRTVs, FColorBufferRef* rtvs)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandels;
		
		for (UINT index = 0; index < numRTVs; ++index)
		{
			rtvHandels.push_back(rtvs[index]->GetRenderTargetView());
			TrackResource(rtvs[index]);
			TransitionBarrier(rtvs[index], EResourceState::RenderTarget);
		}

		mD3DCommandList->OMSetRenderTargets(numRTVs, rtvHandels.data(), false, nullptr);
	}

	void FGraphicsCommandContextBase::SetRenderTargets(UINT numRTVs, FColorBufferRef* rtvs, FDepthBufferRef depthBuffer)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandels;

		for (UINT index = 0; index < numRTVs; ++index)
		{
			rtvHandels.push_back(rtvs[index]->GetRenderTargetView());
			TrackResource(rtvs[index]);
			TransitionBarrier(rtvs[index], EResourceState::RenderTarget);
		}

		if (depthBuffer)
		{
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles = { depthBuffer->GetDepthStencilView() };
			mD3DCommandList->OMSetRenderTargets(numRTVs, rtvHandels.data(), false, handles.data());
			TrackResource(depthBuffer);
			TransitionBarrier(depthBuffer, EResourceState::DepthWrite);
		}
		else
		{
			mD3DCommandList->OMSetRenderTargets(numRTVs, rtvHandels.data(), false, nullptr);
		}
	}

	void FGraphicsCommandContextBase::SetViewport(const FViewport& vp)
	{
		D3D12_VIEWPORT viewport = vp.D3DViewport();
		mD3DCommandList->RSSetViewports(1, &viewport);
	}

	void FGraphicsCommandContextBase::SetViewport(Scalar x, Scalar y, Scalar w, Scalar h, Scalar minDepth /*= 0.0f*/, Scalar maxDepth /*= 1.0f*/)
	{
		FViewport vp{x, y, w, h, minDepth, maxDepth};
		D3D12_VIEWPORT viewport = vp.D3DViewport();
		mD3DCommandList->RSSetViewports(1, &viewport);
	}

	void FGraphicsCommandContextBase::SetScissor(const D3D12_RECT& rect)
	{
		mD3DCommandList->RSSetScissorRects(1, &rect);
	}

	void FGraphicsCommandContextBase::SetScissor(UINT left, UINT top, UINT right, UINT bottom)
	{
		CD3DX12_RECT rect{ static_cast<LONG>(left), static_cast<LONG>(top), static_cast<LONG>(right), static_cast<LONG>(bottom) };
		mD3DCommandList->RSSetScissorRects(1, &rect);
	}

	void FGraphicsCommandContextBase::SetViewportAndScissor(UINT x, UINT y, UINT width, UINT height)
	{
		SetViewport(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
		SetScissor(x, y, x + width, y + height);
	}

	void FGraphicsCommandContextBase::SetStencilRef(UINT stencilRef)
	{
		mD3DCommandList->OMSetStencilRef(stencilRef);
	}

	void FGraphicsCommandContextBase::SetBlendFactor(const FLinearColor& color)
	{
		mD3DCommandList->OMSetBlendFactor(color.Data);
	}

	void FGraphicsCommandContextBase::SetPrimitiveTopology(EPrimitiveTopology primitiveTopology)
	{
		mD3DCommandList->IASetPrimitiveTopology(D3DPrimitiveTopology(primitiveTopology));
	}

	void FGraphicsCommandContextBase::SetDynamicSampler(UINT rootIndex, UINT descriptorOffset, D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		mDynamicSamplerDescriptor.StageDescriptors(rootIndex, descriptorOffset, 1, handle);
	}

	void FGraphicsCommandContextBase::SetDynamicSamplers(UINT rootIndex, UINT descriptorOffset, UINT count, D3D12_CPU_DESCRIPTOR_HANDLE handles[])
	{
		for (UINT index = 0; index < count; ++index)
		{
			mDynamicSamplerDescriptor.StageDescriptors(rootIndex, descriptorOffset + index, 1, handles[index]);	
		}
	}

	void FGraphicsCommandContextBase::SetIndexBuffer(FGpuIndexBufferRef indexBuffer)
	{
		std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViews = { indexBuffer->GetIndexBufferView() };
		TrackResource(indexBuffer);
		TransitionBarrier(indexBuffer, indexBuffer->GetCpuAccess() ? EResourceState::GenericRead : EResourceState::IndexBuffer);
		mD3DCommandList->IASetIndexBuffer(indexBufferViews.data());
	}

	void FGraphicsCommandContextBase::SetVertexBuffer(UINT slot, FGpuVertexBufferRef vertexBuffer)
	{
		SetVertexBuffers(slot, 1, &vertexBuffer);
	}

	void FGraphicsCommandContextBase::SetVertexBuffers(UINT startSlot, UINT count, const FGpuVertexBufferRef* vertexBuffer)
	{
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vetexBufferViews;
		for (UINT index = 0; index < count; ++index)
		{
			vetexBufferViews.emplace_back(vertexBuffer[index]->GetVertexBufferView());
			TrackResource(vertexBuffer[index]);
			TransitionBarrier(vertexBuffer[index], vertexBuffer[index]->GetCpuAccess() ? EResourceState::GenericRead : EResourceState::VertexBuffer);
		}
		mD3DCommandList->IASetVertexBuffers(startSlot, static_cast<UINT>(vetexBufferViews.size()), vetexBufferViews.data());
	}

	void FGraphicsCommandContextBase::SetDynamicIndexBuffer(size_t indexCount, const uint16* data)
	{
		size_t dataSize = indexCount * sizeof(uint16);
		FGpuLinearAllocator::FAllocation alloc = mLinearAllocator.Allocate(dataSize);
		memcpy(alloc.CpuAddress, data, dataSize);

		D3D12_INDEX_BUFFER_VIEW view = {};
		view.BufferLocation = alloc.GpuAddress;
		view.Format = DXGI_FORMAT_R16_UINT;
		view.SizeInBytes = static_cast<UINT>(dataSize);

		mD3DCommandList->IASetIndexBuffer(&view);
	}

	void FGraphicsCommandContextBase::SetDynamicVertexBuffer(UINT slot, size_t vertexCount, size_t vertexStride, const void* data)
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

	void FGraphicsCommandContextBase::SetDepthBounds(float min, float max)
	{
		mD3DCommandList->OMSetDepthBounds(min, max);
	}

	FCopyCommandContext& FCopyCommandContext::Begin(const std::string& id /*= L""*/)
	{
		FCopyCommandContext* newContext = reinterpret_cast<FCopyCommandContext*>(FCommandContext::Begin(id, D3D12_COMMAND_LIST_TYPE_COPY));
		return *newContext;
	}

	FComputeCommandContext& FComputeCommandContext::Begin(const std::string& id /*= L""*/)
	{
		FComputeCommandContext* newContext = reinterpret_cast<FComputeCommandContext*>(FCommandContext::Begin(id, D3D12_COMMAND_LIST_TYPE_COMPUTE));
		return *newContext;
	}

	FGraphicsCommandContext& FGraphicsCommandContext::Begin(const std::string& id /*= L""*/)
	{
		FGraphicsCommandContext* newContext = reinterpret_cast<FGraphicsCommandContext*>(FCommandContext::Begin(id, D3D12_COMMAND_LIST_TYPE_DIRECT));
		return *newContext;
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
		CheckUnboundShaderParameters();

		FlushResourceBarriers();
		mDynamicViewDescriptor.CommitStagedDescriptorsForDraw(*this);
		mDynamicSamplerDescriptor.CommitStagedDescriptorsForDraw(*this);
		mD3DCommandList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}

	void FGraphicsCommandContext::DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
	{
		CheckUnboundShaderParameters();

		FlushResourceBarriers();
		mDynamicViewDescriptor.CommitStagedDescriptorsForDraw(*this);
		mDynamicSamplerDescriptor.CommitStagedDescriptorsForDraw(*this);
		mD3DCommandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}
}