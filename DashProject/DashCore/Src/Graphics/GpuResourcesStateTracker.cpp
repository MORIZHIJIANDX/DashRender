#include "PCH.h"
#include "GpuResourcesStateTracker.h"
#include "GpuResource.h"
#include "d3dx12.h"

namespace Dash
{
	FGpuResourcesStateTracker::ResourceStateMap FGpuResourcesStateTracker::GlobalResourceStates = {};
	std::mutex FGpuResourcesStateTracker::GlobalMutex;
	bool FGpuResourcesStateTracker::IsLocked = false;

	FGpuResourcesStateTracker::FGpuResourcesStateTracker()
	{
	}

	FGpuResourcesStateTracker::~FGpuResourcesStateTracker()
	{
	}

	void FGpuResourcesStateTracker::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier)
	{
		if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
		{
			D3D12_RESOURCE_TRANSITION_BARRIER transitionBarrier = barrier.Transition;
			const auto& iter = mFinalResourceStates.find(transitionBarrier.pResource);
			if (iter != mFinalResourceStates.end())
			{
				ResourceState& currentFinalState = iter->second;

				if (transitionBarrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
				{
					if (currentFinalState.mSubResourceStates.empty())
					{
						D3D12_RESOURCE_STATES currentResourceState = currentFinalState.GetSubResourceState(transitionBarrier.Subresource);
						if (currentResourceState != transitionBarrier.StateAfter)
						{
							D3D12_RESOURCE_BARRIER newBarrier = barrier;
							newBarrier.Transition.StateBefore = currentResourceState;
							mResourceBarriers.push_back(newBarrier);
						}
					}
					else
					{
						D3D12_RESOURCE_DESC resourceDesc = transitionBarrier.pResource->GetDesc();
						UINT32 numSubResources = resourceDesc.MipLevels * resourceDesc.DepthOrArraySize ;

						for (UINT32 subResourceIndex = 0; subResourceIndex < numSubResources; ++numSubResources)
						{
							D3D12_RESOURCE_STATES currentSubResourceState = currentFinalState.GetSubResourceState(subResourceIndex);
							if (currentSubResourceState != transitionBarrier.StateAfter)
							{
								D3D12_RESOURCE_BARRIER newBarrier = barrier;
								newBarrier.Transition.Subresource = subResourceIndex;
								newBarrier.Transition.StateBefore = currentSubResourceState;
								mResourceBarriers.push_back(newBarrier);
							}
						}
					}
				}
				else
				{
					 D3D12_RESOURCE_STATES currentSubResourceState = currentFinalState.GetSubResourceState(transitionBarrier.Subresource);
					 if (currentSubResourceState != transitionBarrier.StateAfter)
					 {
						D3D12_RESOURCE_BARRIER newBarrier = barrier;
						newBarrier.Transition.StateBefore = currentSubResourceState;
						mResourceBarriers.push_back(newBarrier);
					 }
				}
			}
			else
			{
				mPendingResourceBarriers.push_back(barrier);
			}

			mFinalResourceStates[transitionBarrier.pResource].SetSubResourceState(transitionBarrier.Subresource, transitionBarrier.StateAfter);
		}
		else
		{
			mResourceBarriers.push_back(barrier);
		}
	}

	void FGpuResourcesStateTracker::TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter, UINT subResource /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
	{
		if (resource)
		{
			ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_COMMON, stateAfter, subResource));
		}
	}


	void FGpuResourcesStateTracker::TransitionResource(FGpuResource& resource, D3D12_RESOURCE_STATES stateAfter, UINT subResource /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
	{
		if (resource.GetResource())
		{
			ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(resource.GetResource(), D3D12_RESOURCE_STATE_COMMON, stateAfter, subResource));
		}
	}

	void FGpuResourcesStateTracker::UAVBarrier(ID3D12Resource* resource /*= nullptr*/)
	{
		ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(resource));
	}

	void FGpuResourcesStateTracker::UAVBarrier(FGpuResource& resource)
	{
		if (resource.GetResource())
		{
			ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(resource.GetResource()));
		}
	}

	void FGpuResourcesStateTracker::AliasBarrier(ID3D12Resource* resourceBefore /*= nullptr*/, ID3D12Resource* resourceAfter /*= nullptr*/)
	{
		ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Aliasing(resourceBefore, resourceAfter));
	}

	void FGpuResourcesStateTracker::AliasBarrier(FGpuResource& resourceBefore, FGpuResource& resourceAfter)
	{
		if (resourceBefore.GetResource() && resourceAfter.GetResource())
		{
			ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Aliasing(resourceBefore.GetResource(), resourceAfter.GetResource()));
		}
	}

	uint32_t FGpuResourcesStateTracker::FlushPendingResourceBarriers(ID3D12GraphicsCommandList* commandList)
	{
		ASSERT(IsLocked == true);
		ASSERT(commandList != nullptr);

		std::vector<D3D12_RESOURCE_BARRIER> resourceBarriers;
		resourceBarriers.reserve(mPendingResourceBarriers.size());

		for (auto& resourceBarrier : mPendingResourceBarriers)
		{
			if (resourceBarrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
			{
				D3D12_RESOURCE_TRANSITION_BARRIER& transitionBarrier = resourceBarrier.Transition;

				const auto& iter = GlobalResourceStates.find(transitionBarrier.pResource);
				if (iter != GlobalResourceStates.end())
				{	
					ResourceState& currentGlobalState = iter->second;
					if (transitionBarrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
					{
						if (currentGlobalState.mSubResourceStates.empty())
						{
							D3D12_RESOURCE_STATES currentResourceState = currentGlobalState.GetSubResourceState(transitionBarrier.Subresource);
							if (currentResourceState != transitionBarrier.StateAfter)
							{
								D3D12_RESOURCE_BARRIER newBarrier = resourceBarrier;
								newBarrier.Transition.StateBefore = currentResourceState;
								resourceBarriers.push_back(newBarrier);
							}
						}
						else
						{
							D3D12_RESOURCE_DESC resourceDesc = transitionBarrier.pResource->GetDesc();
							UINT32 numSubResources = resourceDesc.MipLevels * resourceDesc.DepthOrArraySize;

							for (UINT32 subResourceIndex = 0; subResourceIndex < numSubResources; ++numSubResources)
							{
								D3D12_RESOURCE_STATES currentSubResourceState = currentGlobalState.GetSubResourceState(subResourceIndex);
								if (currentSubResourceState != transitionBarrier.StateAfter)
								{
									D3D12_RESOURCE_BARRIER newBarrier = resourceBarrier;
									newBarrier.Transition.Subresource = subResourceIndex;
									newBarrier.Transition.StateBefore = currentSubResourceState;
									resourceBarriers.push_back(newBarrier);
								}
							}
						}
					}
					else
					{
						D3D12_RESOURCE_STATES currentSubResourceState = currentGlobalState.GetSubResourceState(transitionBarrier.Subresource);
						if (currentSubResourceState != transitionBarrier.StateAfter)
						{
							D3D12_RESOURCE_BARRIER newBarrier = resourceBarrier;
							newBarrier.Transition.StateBefore = currentSubResourceState;
							resourceBarriers.push_back(newBarrier);
						}
					}
				}
			}
		}

		UINT32 num = static_cast<UINT32>(resourceBarriers.size());
		if (num > 0)
		{
			commandList->ResourceBarrier(num, resourceBarriers.data());
		}

		mPendingResourceBarriers.clear();

		return num;
	}

	uint32_t FGpuResourcesStateTracker::FlushResourceBarriers(ID3D12GraphicsCommandList* commandList)
	{
		ASSERT(commandList != nullptr);

		UINT32 num = static_cast<UINT32>(mResourceBarriers.size());
		if (num > 0)
		{
			commandList->ResourceBarrier(num, mResourceBarriers.data());
			mResourceBarriers.clear();
		}

		return num;
	}

	void FGpuResourcesStateTracker::CommitFinalResourceStates()
	{
		ASSERT(IsLocked == true);

		for (const auto& resourceState : mFinalResourceStates)
		{
			GlobalResourceStates[resourceState.first] = resourceState.second;
		}
	}

	void FGpuResourcesStateTracker::Reset()
	{
		mPendingResourceBarriers.clear();
		mResourceBarriers.clear();
		mFinalResourceStates.clear();
	}

	void FGpuResourcesStateTracker::Lock()
	{
		GlobalMutex.lock();
		IsLocked = true;
	}

	void FGpuResourcesStateTracker::Unlock()
	{
		GlobalMutex.unlock();
		IsLocked = false;
	}

	void FGpuResourcesStateTracker::AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state)
	{
		if (resource)
		{
			std::lock_guard<std::mutex> lock(GlobalMutex);

			GlobalResourceStates[resource].SetSubResourceState(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
		}
	}

	void FGpuResourcesStateTracker::AddGlobalResourceState(FGpuResource& resource, D3D12_RESOURCE_STATES state)
	{
		if (resource.GetResource())
		{
			std::lock_guard<std::mutex> lock(GlobalMutex);

			GlobalResourceStates[resource.GetResource()].SetSubResourceState(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, state);
		}
	}

	void FGpuResourcesStateTracker::RemoveGlobalResourceState(ID3D12Resource* resource)
	{
		if (resource)
		{
			std::lock_guard<std::mutex> lock(GlobalMutex);

			GlobalResourceStates.erase(resource);
		}
	}

	void FGpuResourcesStateTracker::RemoveGlobalResourceState(FGpuResource& resource)
	{
		if (resource.GetResource())
		{
			std::lock_guard<std::mutex> lock(GlobalMutex);

			GlobalResourceStates.erase(resource.GetResource());
		}
	}

}