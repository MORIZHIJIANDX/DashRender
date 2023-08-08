#pragma once

#include "d3dx12.h"
#include "GpuResource.h"
#include "CommandQueue.h"

namespace Dash
{
	class FGpuResourcesStateTracker
	{
	public:
		FGpuResourcesStateTracker();
		~FGpuResourcesStateTracker();

		/**
		 * Push a resource barrier to the resource state tracker.
		 *
		 * @param barrier The resource barrier to push to the resource state tracker.
		 */
		void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);

		/**
		 * Push a transition resource barrier to the resource state tracker.
		 *
		 * @param resource The resource to transition.
		 * @param stateAfter The state to transition the resource to.
		 * @param subResource The subresource to transition. By default, this is D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
		 * which indicates that all subresources should be transitioned to the same state.
		 */
		void TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter, UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		void TransitionResource(std::shared_ptr<FGpuResource> resource, D3D12_RESOURCE_STATES stateAfter, UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		/**
		 * Push a UAV resource barrier for the given resource.
		 *
		 * @param resource The resource to add a UAV barrier for. Can be NULL which
		 * indicates that any UAV access could require the barrier.
		 */
		void UAVBarrier(ID3D12Resource* resource = nullptr);
		void UAVBarrier(FGpuResourceRef resource);

		/**
		 * Push an aliasing barrier for the given resource.
		 *
		 * @param beforeResource The resource currently occupying the space in the heap.
		 * @param afterResource The resource that will be occupying the space in the heap.
		 *
		 * Either the beforeResource or the afterResource parameters can be NULL which
		 * indicates that any placed or reserved resource could cause aliasing.
		 */
		void AliasBarrier(ID3D12Resource* resourceBefore = nullptr, ID3D12Resource* resourceAfter = nullptr);
		void AliasBarrier(FGpuResourceRef resourceBefore, FGpuResourceRef resourceAfter);

		/**
		 * Flush any pending resource barriers to the command list.
		 *
		 * @return The number of resource barriers that were flushed to the command list.
		 */
		FCommandList* FlushPendingResourceBarriers(D3D12_COMMAND_LIST_TYPE commandListType);

		/**
		 * Flush any (non-pending) resource barriers that have been pushed to the resource state
		 * tracker.
		 */
		uint32_t FlushResourceBarriers(ID3D12GraphicsCommandList* commandList);

		/**
		 * Commit final resource states to the global resource state map.
		 * This must be called when the command list is closed.
		 */
		void CommitFinalResourceStates();

		/**
		 * Reset state tracking. This must be done when the command list is reset.
		 */
		void Reset();

		/**
		 * The global state must be locked before flushing pending resource barriers
		 * and committing the final resource state to the global resource state.
		 * This ensures consistency of the global resource state between command list
		 * executions.
		 */
		static void Lock();

		/**
		 * Unlocks the global resource state after the final states have been committed
		 * to the global resource state array.
		 */
		static void Unlock();

		/**
		 * Add a resource with a given state to the global resource state array (map).
		 * This should be done when the resource is created for the first time.
		 */
		static void AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
		static void AddGlobalResourceState(FGpuResourceRef resource, D3D12_RESOURCE_STATES state);

		/**
		 * Remove a resource from the global resource state array (map).
		 * This should only be done when the resource is destroyed.
		 */
		static void RemoveGlobalResourceState(ID3D12Resource* resource);
		static void RemoveGlobalResourceState(FGpuResourceRef resource);

	private:
		// An array (vector) of resource barriers.
		using ResourceBarriers = std::vector<D3D12_RESOURCE_BARRIER>;

		// Pending resource transitions are committed before a command list
		// is executed on the command queue. This guarantees that resources will
		// be in the expected state at the beginning of a command list.
		ResourceBarriers mPendingResourceBarriers;

		// Resource barriers that need to be committed to the command list.
		ResourceBarriers mResourceBarriers;

		// Tracks the state of a particular resource and all of its subresources.
		struct ResourceState
		{
			// Initialize all of the subresources within a resource to the given state.
			explicit ResourceState(D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON)
				: State(state)
			{}

			// Set a subresource to a particular state.
			void SetSubResourceState(UINT subResource, D3D12_RESOURCE_STATES state)
			{
				if (subResource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
				{
					State = state;
					mSubResourceStates.clear();
				}
				else
				{
					mSubResourceStates[subResource] = state;
				}
			}
		
			// Get the state of a (sub)resource within the resource.
			// If the specified subresource is not found in the SubresourceState array (map)
			// then the state of the resource (D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) is
			// returned.
			D3D12_RESOURCE_STATES GetSubResourceState(UINT subResource) const
			{
				D3D12_RESOURCE_STATES state = State;
				const auto iter = mSubResourceStates.find(subResource);
				if (iter != mSubResourceStates.end())
				{
					state = iter->second;
				}
				return state;
			}

			// If the SubresourceState array (map) is empty, then the State variable defines
			// the state of all of the subresources.
			D3D12_RESOURCE_STATES State;
			std::map<UINT, D3D12_RESOURCE_STATES> mSubResourceStates;
		};

		using ResourceStateMap = std::unordered_map<ID3D12Resource*, ResourceState>;

		// The final (last known state) of the resources within a command list.
		// The final resource state is committed to the global resource state when the
		// command list is closed but before it is executed on the command queue.
		ResourceStateMap mFinalResourceStates;

		// The global resource state array (map) stores the state of a resource
		// between command list execution.
		static ResourceStateMap GlobalResourceStates;

		// The mutex protects shared access to the GlobalResourceState map.
		static std::mutex GlobalMutex;
		static bool       IsLocked;
	}; 
}