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

	void FCopyCommandContextBase::BeginQuery(const FQueryHeapRef& queryHeap, uint32 queryIndex)
	{
		D3D12_QUERY_TYPE queryType = D3DQueryType(queryHeap->GetDesc().type);
		mD3DCommandList->BeginQuery(queryHeap->D3DQueryHeap(), queryType, queryIndex);
	}

	void FCopyCommandContextBase::EndQuery(const FQueryHeapRef& queryHeap, uint32 queryIndex)
	{
		D3D12_QUERY_TYPE queryType = D3DQueryType(queryHeap->GetDesc().type);
		mD3DCommandList->EndQuery(queryHeap->D3DQueryHeap(), queryType, queryIndex);
	}

	void FCopyCommandContextBase::ResolveQueryData(const FQueryHeapRef& queryHeap, uint32 startIndex, uint32 numQueries, const FGpuBufferRef& dest, uint32 destOffset)
	{
		TransitionBarrier(dest, EResourceState::CopyDestination, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);

		D3D12_QUERY_TYPE queryType = D3DQueryType(queryHeap->GetDesc().type);
		mD3DCommandList->ResolveQueryData(queryHeap->D3DQueryHeap(), queryType, startIndex, numQueries, dest->GetResource()->GetResource(), destOffset);
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

	void FCopyCommandContextBase::TransitionBarrier(const FGpuResourceRef& resource, EResourceState newState, uint32 subResource /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/, bool flushImmediate /*= false*/)
	{
		mResourceStateTracker.TransitionResource(resource, D3DResourceState(newState), subResource);

		if (flushImmediate)
		{
			FlushResourceBarriers();
		}
	}

	void FCopyCommandContextBase::UAVBarrier(const FGpuResourceRef& resource, bool flushImmediate /*= false*/)
	{
		mResourceStateTracker.UAVBarrier(resource);

		if (flushImmediate)
		{
			FlushResourceBarriers();
		}
	}

	void FCopyCommandContextBase::AliasingBarrier(const FGpuResourceRef& resourceBefore, const FGpuResourceRef& resourceAfter, bool flushImmediate /*= false*/)
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

	void FCopyCommandContextBase::InitializeBuffer(const FGpuBufferRef& dest, const void* bufferData, size_t numBytes, size_t offset /*= 0*/)
	{
		FCopyCommandContext& context = FCopyCommandContext::Begin("InitializeBufferContext");

		FGpuLinearAllocator::FAllocation alloc = context.mLinearAllocator.Allocate(numBytes);
		memcpy(alloc.CpuAddress, bufferData, numBytes);

		context.TransitionBarrier(dest, EResourceState::CopyDestination, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		context.mD3DCommandList->CopyBufferRegion(dest->GetResource()->GetResource(), offset, alloc.Resource.GetResource()->GetResource(), alloc.Offset, numBytes);
		context.TransitionBarrier(dest, EResourceState::Common, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);

		context.Finish(true);
	}

	void FCopyCommandContextBase::UpdateTextureBuffer(const FTextureBufferRef& dest, uint32 firstSubresource, uint32 numSubresources, const FSubResourceData* subresourceData)
	{
		ASSERT(subresourceData != nullptr);

		FCopyCommandContext& context = FCopyCommandContext::Begin("UpdateTextureBuffer");

		UINT64 requiredSize = GetRequiredIntermediateSize(dest->GetResource()->GetResource(), firstSubresource, numSubresources);
		FGpuLinearAllocator::FAllocation alloc = context.mLinearAllocator.Allocate(requiredSize);

		std::vector<D3D12_SUBRESOURCE_DATA> d3dSubResources;
		for (size_t i = 0; i < numSubresources; i++)
		{
			d3dSubResources.emplace_back(subresourceData[i].D3DSubResource());
		}

		// Resource must be in the copy-destination state.
		context.TransitionBarrier(dest, EResourceState::CopyDestination, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		UpdateSubresources(context.GetD3DCommandList(), dest->GetResource()->GetResource(), alloc.Resource.GetResource()->GetResource(), alloc.Offset, firstSubresource, numSubresources, d3dSubResources.data());
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

	void FComputeCommandContextBase::SetDescriptorHeaps(uint32 count, D3D12_DESCRIPTOR_HEAP_TYPE types[], ID3D12DescriptorHeap* heaps[])
	{
		bool anyChanged = false;

		for (uint32 index = 0; index < count; ++index)
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

	void FComputeCommandContextBase::SetComputePipelineState(FComputePSO* pso)
	{
		ASSERT(pso);

		ID3D12PipelineState* pipelineState = pso->GetPipelineState();

		ASSERT(pipelineState);

		if (pipelineState == mCurrentPipelineState)
		{
			return;
		}

		SetComputeRootSignature(pso->GetRootSignature());
		mD3DCommandList->SetPipelineState(pipelineState);
		mCurrentPipelineState = pipelineState;

		SetPipelineStateInternal(pso);
	}

	void FComputeCommandContextBase::SetComputeRootSignature(FRootSignature* rootSignature)
	{
		ASSERT(rootSignature != nullptr);

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
		uint32 nonNullHeaps = 0;
		ID3D12DescriptorHeap* heapsToBind[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		for (uint32 index = 0; index < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++index)
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

	void FCommandContext::TrackResource(const FGpuResourceRef& resource)
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

	void FCommandContext::SetPipelineStateInternal(FPipelineStateObject* inPSO)
	{
		mPSO = inPSO;
		mValidShaderStages.clear();

		FShaderPassRef shaderPass = mPSO->GetShaderPass();
		for (auto& iter : shaderPass->GetShaders())
		{
			mValidShaderStages.push_back(iter.first);
		}	

		InitParameterBindState();
	}

	void FCommandContext::InitParameterBindState()
	{
		if (mPSO == nullptr)
		{
			return;
		}
		
		ASSERT_MSG(mPSO->GetShaderPass() != nullptr, "Shader Pass Is Not Set.");

		FShaderPassRef shaderPass = mPSO->GetShaderPass();

		for (EShaderStage stage : mValidShaderStages)
		{
			uint32 shaderStageIndex = static_cast<uint32>(stage);

			InitStateForParameter(shaderPass->GetCBVParameterNum(stage), mConstantBufferBindState[shaderStageIndex]);
			InitStateForParameter(shaderPass->GetSRVParameterNum(stage), mShaderResourceViewBindState[shaderStageIndex]);
			InitStateForParameter(shaderPass->GetUAVParameterNum(stage), mUnorderAccessViewBindState[shaderStageIndex]);
			InitStateForParameter(shaderPass->GetSamplerParameterNum(stage), mSamplerBindState[shaderStageIndex]);
		}
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
		auto checkUnboundParameterFunc = [](const std::vector<bool>& bindStateMap, const FShaderPassRef& shaderPass, EShaderStage stage, EShaderParameterType type, const std::string& typeStr)
		{
			for (uint32 baseIndex = 0; baseIndex < bindStateMap.size(); ++baseIndex)
			{
				if (bindStateMap[baseIndex] == false)
				{
					DASH_LOG(LogTemp, Warning, "Shader Parameter : {}, Base Index {}, Type : {}, Shader Stage {}, Is Not Bounded!", 
						shaderPass->FindShaderVariable(type, baseIndex, stage).Name, baseIndex, typeStr, static_cast<uint32>(stage));
				}
			}
		};

		for (EShaderStage stage : mValidShaderStages)
		{
			uint32 shaderStageIndex = static_cast<uint32>(stage);

			checkUnboundParameterFunc(mConstantBufferBindState[shaderStageIndex], mPSO->GetShaderPass(), stage, EShaderParameterType::UniformBuffer, "Constant Buffer");
			checkUnboundParameterFunc(mShaderResourceViewBindState[shaderStageIndex], mPSO->GetShaderPass(), stage, EShaderParameterType::SRV, "Shader Resource View");
			checkUnboundParameterFunc(mUnorderAccessViewBindState[shaderStageIndex], mPSO->GetShaderPass(), stage, EShaderParameterType::UAV, "Unorder Access View");
			checkUnboundParameterFunc(mSamplerBindState[shaderStageIndex], mPSO->GetShaderPass(), stage, EShaderParameterType::Sampler, "Sampler");
		}
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
		mPSO = nullptr;
		mCommandList = nullptr;
		mD3DCommandList = nullptr;
	}

	void FComputeCommandContextBase::ClearUAV(const FGpuBufferRef& target)
	{
		if (!target->SupportUnorderedAccessView())
		{
			return;
		}

		UAVBarrier(target, true);

		D3D12_GPU_DESCRIPTOR_HANDLE handle = mDynamicViewDescriptor.CopyAndSetDescriptor(*this, target->GetUnorderedAccessView());
		const uint32 clearColor[4] = {};
		mD3DCommandList->ClearUnorderedAccessViewUint(handle, target->GetUnorderedAccessView(), target->GetResource()->GetResource(), clearColor, 0, nullptr);

		TrackResource(target);
	}

	void FComputeCommandContextBase::ClearUAV(const FColorBufferRef& target)
	{
		UAVBarrier(target, true);

		D3D12_GPU_DESCRIPTOR_HANDLE handle = mDynamicViewDescriptor.CopyAndSetDescriptor(*this, target->GetUnorderedAccessView());
		CD3DX12_RECT clearRect{0, 0, static_cast<LONG>(target->GetWidth()), static_cast<LONG>(target->GetHeight()) };
		mD3DCommandList->ClearUnorderedAccessViewFloat(handle, target->GetUnorderedAccessView(), target->GetResource()->GetResource(), target->GetClearColor().Data, 1, &clearRect);

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
		ASSERT_MSG(mPSO != nullptr, "Pipeline State Is Not Set.");

		FShaderPassRef shaderPass = mPSO->GetShaderPass();

		bool bParameterValid = false;

		if (shaderPass)
		{
			for (EShaderStage stage : mValidShaderStages)
			{
				uint32 shaderStageIndex = static_cast<uint32>(stage);

				const FShaderVariable& variable = shaderPass->FindShaderVariable(bufferName, stage);
				if (variable.ParamterType == EShaderParameterType::UniformBuffer)
				{
					ASSERT(sizeInBytes == variable.Size);
					SetRootConstantBufferView(variable.RootParameterIndex, sizeInBytes, constants);
					mConstantBufferBindState[shaderStageIndex][variable.BaseIndex] = true;
					bParameterValid = true;
				}
			}
		}

		if (!bParameterValid)
		{
			DASH_LOG(LogTemp, Warning, "Can't Find Constant Buffer Parameter : {}", bufferName);
		}
	}

	void FComputeCommandContextBase::SetShaderResourceView(const std::string& srvrName, const FColorBufferRef& buffer, EResourceState stateAfter, uint32 firstSubResource, uint32 numSubResources)
	{
		ASSERT_MSG(mPSO != nullptr, "Pipeline State Is Not Set.");

		SetShaderResourceView(srvrName, buffer, buffer->GetShaderResourceView(), stateAfter, firstSubResource, numSubResources);
	}

	void FComputeCommandContextBase::SetShaderResourceView(const std::string& srvrName, const FTextureBufferRef& buffer, EResourceState stateAfter, uint32 firstSubResource, uint32 numSubResources)
	{
		ASSERT_MSG(mPSO != nullptr, "Pipeline State Is Not Set.");

		SetShaderResourceView(srvrName, buffer, buffer->GetShaderResourceView(), stateAfter, firstSubResource, numSubResources);
	}

	void FComputeCommandContextBase::SetShaderResourceView(const std::string& srvrName, const FStructuredBufferRef& buffer, EResourceState stateAfter, uint32 firstSubResource, uint32 numSubResources)
	{
		ASSERT_MSG(mPSO != nullptr, "Pipeline State Is Not Set.");

		SetShaderResourceView(srvrName, buffer, buffer->GetShaderResourceView(), stateAfter, firstSubResource, numSubResources);
	}

	void FComputeCommandContextBase::SetUnorderAccessView(const std::string& uavName, const FColorBufferRef& buffer, EResourceState stateAfter, uint32 firstSubResource, uint32 numSubResources)
	{
		SetUnorderAccessView(uavName, buffer, buffer->GetUnorderedAccessView(), stateAfter, firstSubResource, numSubResources);
	}

	void FComputeCommandContextBase::SetShaderResourceView(const std::string& srvrName, const FGpuResourceRef& resource, const D3D12_CPU_DESCRIPTOR_HANDLE& srcDescriptors,
		EResourceState stateAfter /*= EResourceState::AnyShaderAccess*/, uint32 firstSubResource /*= 0*/,
		uint32 numSubResources /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
	{
		FShaderPassRef shaderPass = mPSO->GetShaderPass();

		bool bParameterValid = false;

		if (shaderPass)
		{
			for (EShaderStage stage : mValidShaderStages)
			{
				uint32 shaderStageIndex = static_cast<uint32>(stage);

				const FShaderVariable& variable = shaderPass->FindShaderVariable(srvrName, stage);
				if (variable.ParamterType == EShaderParameterType::SRV)
				{
					if (numSubResources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
					{
						for (uint32 index = firstSubResource; index < (firstSubResource + numSubResources); ++index)
						{
							TransitionBarrier(resource, stateAfter, index);
						}
					}
					else
					{
						TransitionBarrier(resource, stateAfter);
					}

					mDynamicViewDescriptor.StageDescriptors(variable.RootParameterIndex, variable.DescriptorOffset, 1, srcDescriptors);
					TrackResource(resource);

					mShaderResourceViewBindState[shaderStageIndex][variable.BaseIndex] = true;

					bParameterValid = true;
				}
			}
		}

		if (!bParameterValid)
		{
			DASH_LOG(LogTemp, Warning, "Can't Find Shader Resource Parameter : {}", srvrName);
		}
	}

	void FComputeCommandContextBase::SetUnorderAccessView(const std::string& srvrName, const FGpuResourceRef& resource, const D3D12_CPU_DESCRIPTOR_HANDLE& uavDescriptors,
		EResourceState stateAfter /*= EResourceState::UnorderedAccess*/, uint32 firstSubResource /*= 0*/, uint32 numSubResources /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
	{
		FShaderPassRef shaderPass = mPSO->GetShaderPass();

		bool bParameterValid = false;

		if (shaderPass)
		{
			for (EShaderStage stage : mValidShaderStages)
			{
				uint32 shaderStageIndex = static_cast<uint32>(stage);

				const FShaderVariable& variable = shaderPass->FindShaderVariable(srvrName, stage);
				if (variable.ParamterType == EShaderParameterType::UAV)
				{
					if (numSubResources < D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
					{
						for (uint32 index = firstSubResource; index < (firstSubResource + numSubResources); ++index)
						{
							TransitionBarrier(resource, stateAfter, index);
						}
					}
					else
					{
						TransitionBarrier(resource, stateAfter);
					}

					mDynamicViewDescriptor.StageDescriptors(variable.RootParameterIndex, variable.DescriptorOffset, 1, uavDescriptors);
					TrackResource(resource);

					mUnorderAccessViewBindState[shaderStageIndex][variable.BaseIndex] = true;

					bParameterValid = true;
				}
			}
		}

		if (!bParameterValid)
		{
			DASH_LOG(LogTemp, Warning, "Can't Find Unordered Access Parameter : {}", srvrName);
		}
	}

	void FGraphicsCommandContextBase::ClearColor(const FColorBufferRef& target, D3D12_RECT* rect /*= nullptr*/)
	{
		TransitionBarrier(target, EResourceState::RenderTarget, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearRenderTargetView(target->GetRenderTargetView(), target->GetClearColor().Data, (rect == nullptr) ? 0 : 1, rect);

		TrackResource(target);
	}

	void FGraphicsCommandContextBase::ClearColor(const FColorBufferRef& target, const FLinearColor& color, D3D12_RECT* rect /*= nullptr*/)
	{
		TransitionBarrier(target, EResourceState::RenderTarget, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearRenderTargetView(target->GetRenderTargetView(), color.Data, (rect == nullptr) ? 0 : 1, rect);

		TrackResource(target);
	}

	void FGraphicsCommandContextBase::ClearDepth(const FDepthBufferRef& target)
	{
		TransitionBarrier(target, EResourceState::DepthWrite, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearDepthStencilView(target->GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH, target->GetClearDepth(), target->GetClearStencil(), 0, nullptr);

		TrackResource(target);
	}

	void FGraphicsCommandContextBase::ClearStencil(const FDepthBufferRef& target)
	{
		TransitionBarrier(target, EResourceState::DepthWrite, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearDepthStencilView(target->GetDepthStencilView(), D3D12_CLEAR_FLAG_STENCIL, target->GetClearDepth(), target->GetClearStencil(), 0, nullptr);

		TrackResource(target);
	}

	void FGraphicsCommandContextBase::ClearDepthAndStencil(const FDepthBufferRef& target)
	{
		TransitionBarrier(target, EResourceState::DepthWrite, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
		mD3DCommandList->ClearDepthStencilView(target->GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, target->GetClearDepth(), target->GetClearStencil(), 0, nullptr);

		TrackResource(target);
	}

	void FGraphicsCommandContextBase::SetGraphicsPipelineState(FGraphicsPSO* pso)
	{
		ASSERT(pso);

		ID3D12PipelineState* pipelineState = pso->GetPipelineState();

		if (pipelineState == mCurrentPipelineState)
		{
			return;
		}

		SetGraphicsRootSignature(pso->GetRootSignature());
		mD3DCommandList->SetPipelineState(pipelineState);
		SetPrimitiveTopology(pso->GetPrimitiveTopology());
		mCurrentPipelineState = pipelineState;

		SetPipelineStateInternal(pso);
	}

	void FGraphicsCommandContextBase::SetGraphicsRootSignature(FRootSignature* rootSignature)
	{
		ASSERT(rootSignature != nullptr);

		if (rootSignature->GetSignature() == mCurrentRootSignature)
		{
			return;
		}

		mD3DCommandList->SetGraphicsRootSignature(rootSignature->GetSignature());

		mDynamicViewDescriptor.ParseRootSignature(*rootSignature);
		mDynamicSamplerDescriptor.ParseRootSignature(*rootSignature);
	}

	void FGraphicsCommandContextBase::SetRenderTargets(uint32 numRTVs, const FColorBufferRef* rtvs)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandels;
		
		for (uint32 index = 0; index < numRTVs; ++index)
		{
			rtvHandels.push_back(rtvs[index]->GetRenderTargetView());
			TrackResource(rtvs[index]);
			TransitionBarrier(rtvs[index], EResourceState::RenderTarget);
		}

		mD3DCommandList->OMSetRenderTargets(numRTVs, rtvHandels.data(), false, nullptr);
	}

	void FGraphicsCommandContextBase::SetRenderTargets(uint32 numRTVs, const FColorBufferRef* rtvs, const FDepthBufferRef& depthBuffer)
	{
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandels;

		for (uint32 index = 0; index < numRTVs; ++index)
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

	void FGraphicsCommandContextBase::SetScissor(uint32 left, uint32 top, uint32 right, uint32 bottom)
	{
		CD3DX12_RECT rect{ static_cast<LONG>(left), static_cast<LONG>(top), static_cast<LONG>(right), static_cast<LONG>(bottom) };
		mD3DCommandList->RSSetScissorRects(1, &rect);
	}

	void FGraphicsCommandContextBase::SetViewportAndScissor(uint32 x, uint32 y, uint32 width, uint32 height)
	{
		SetViewport(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
		SetScissor(x, y, x + width, y + height);
	}

	void FGraphicsCommandContextBase::SetStencilRef(uint32 stencilRef)
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

	void FGraphicsCommandContextBase::SetDynamicSampler(uint32 rootIndex, uint32 descriptorOffset, D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		mDynamicSamplerDescriptor.StageDescriptors(rootIndex, descriptorOffset, 1, handle);
	}

	void FGraphicsCommandContextBase::SetDynamicSamplers(uint32 rootIndex, uint32 descriptorOffset, uint32 count, D3D12_CPU_DESCRIPTOR_HANDLE handles[])
	{
		for (uint32 index = 0; index < count; ++index)
		{
			mDynamicSamplerDescriptor.StageDescriptors(rootIndex, descriptorOffset + index, 1, handles[index]);	
		}
	}

	void FGraphicsCommandContextBase::SetIndexBuffer(const FGpuIndexBufferRef& indexBuffer)
	{
		std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViews = { indexBuffer->GetIndexBufferView() };
		TrackResource(indexBuffer);
		TransitionBarrier(indexBuffer, indexBuffer->GetCpuAccess() ? EResourceState::GenericRead : EResourceState::IndexBuffer);
		mD3DCommandList->IASetIndexBuffer(indexBufferViews.data());
	}

	void FGraphicsCommandContextBase::SetVertexBuffer(uint32 slot, const FGpuVertexBufferRef& vertexBuffer)
	{
		SetVertexBuffers(slot, 1, &vertexBuffer);
	}

	void FGraphicsCommandContextBase::SetVertexBuffers(uint32 startSlot, uint32 count, const FGpuVertexBufferRef* vertexBuffer)
	{
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vetexBufferViews;
		for (uint32 index = 0; index < count; ++index)
		{
			vetexBufferViews.emplace_back(vertexBuffer[index]->GetVertexBufferView());
			TrackResource(vertexBuffer[index]);
			TransitionBarrier(vertexBuffer[index], vertexBuffer[index]->GetCpuAccess() ? EResourceState::GenericRead : EResourceState::VertexBuffer);
		}
		mD3DCommandList->IASetVertexBuffers(startSlot, static_cast<uint32>(vetexBufferViews.size()), vetexBufferViews.data());
	}

	void FGraphicsCommandContextBase::SetDynamicIndexBuffer(uint32 indexCount, const uint16* data)
	{
		size_t dataSize = indexCount * sizeof(uint16);
		FGpuLinearAllocator::FAllocation alloc = mLinearAllocator.Allocate(dataSize);
		memcpy(alloc.CpuAddress, data, dataSize);

		D3D12_INDEX_BUFFER_VIEW view = {};
		view.BufferLocation = alloc.GpuAddress;
		view.Format = DXGI_FORMAT_R16_UINT;
		view.SizeInBytes = static_cast<uint32>(dataSize);

		mD3DCommandList->IASetIndexBuffer(&view);
	}

	void FGraphicsCommandContextBase::SetDynamicVertexBuffer(uint32 slot, uint32 vertexCount, uint32 vertexStride, const void* data)
	{
		size_t dataSize = vertexCount * vertexStride;
		FGpuLinearAllocator::FAllocation alloc = mLinearAllocator.Allocate(dataSize);
		memcpy(alloc.CpuAddress, data, dataSize);

		D3D12_VERTEX_BUFFER_VIEW view = {};
		view.BufferLocation = alloc.GpuAddress;
		view.SizeInBytes = static_cast<uint32>(dataSize);
		view.StrideInBytes = static_cast<uint32>(vertexStride);

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

	void FGraphicsCommandContext::Draw(uint32 vertexCount, uint32 vertexStartOffset /*= 0*/)
	{
		DrawInstanced(vertexCount, 1, vertexStartOffset, 0);
	}

	void FGraphicsCommandContext::DrawIndexed(uint32 indexCount, uint32 startIndexLocation /*= 0*/, uint32 baseVertexLocation /*= 0*/)
	{
		DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
	}

	void FGraphicsCommandContext::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation /*= 0*/, uint32 startInstanceLocation /*= 0*/)
	{
		CheckUnboundShaderParameters();

		FlushResourceBarriers();
		mDynamicViewDescriptor.CommitStagedDescriptorsForDraw(*this);
		mDynamicSamplerDescriptor.CommitStagedDescriptorsForDraw(*this);
		mD3DCommandList->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
	}

	void FGraphicsCommandContext::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, int32 baseVertexLocation, uint32 startInstanceLocation)
	{
		CheckUnboundShaderParameters();

		FlushResourceBarriers();
		mDynamicViewDescriptor.CommitStagedDescriptorsForDraw(*this);
		mDynamicSamplerDescriptor.CommitStagedDescriptorsForDraw(*this);
		mD3DCommandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
	}
}