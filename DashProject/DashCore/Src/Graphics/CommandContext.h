#pragma once

#include "CommandQueue.h"
#include "ColorBuffer.h"
#include "DynamicDescriptorHeap.h"

namespace Dash
{
	class FCommandContext;
	class FGraphicsCommandContext;
	class FComputeCommandContext;

	class FCommandContextManager
	{
	public:
		FCommandContextManager();

		FCommandContext* AllocateContext(D3D12_COMMAND_LIST_TYPE type);
		void FreeContext(FCommandContext* context);
		void Destroy();

	private:
		std::vector<std::unique_ptr<FCommandContext>> mContextPool[4];
		std::queue<FCommandContext*> mAvailableContexts[4];
		std::mutex mAllocationMutex;
	};

	class FCommandContext
	{
		friend FCommandContextManager;
	private:
		
		FCommandContext(D3D12_COMMAND_LIST_TYPE type);

		/**
		 * Reset the command list. This should only be called by the CommandQueue
		 * before the command list is returned from CommandQueue::GetCommandList.
		 */
		void Reset();

	public:
		~FCommandContext();

		//Disable Copy
		FCommandContext(const FCommandContext&) = delete;
		FCommandContext& operator=(const FCommandContext&) = delete;

		// Flush existing commands to the GPU but keep the context alive
		uint64_t Flush(bool waitForCompletion = false);

		// Flush existing commands and release the current context
		uint64_t Finish(bool waitForCompletion = false);

		// Prepare to render by reserving a command list and command allocator
		void Initialize();

		FGraphicsCommandContext& GetGraphicsCommandContext();
		FComputeCommandContext& GetComputeCommandContext();

		FCommandList* GetCommandList();
		ID3D12GraphicsCommandList* GetD3DCommandList();

		void TransitionBarrier(FGpuResource& resource, D3D12_RESOURCE_STATES newState, bool flushImmediate = false);
		void UAVBarrier(FGpuResource& resource, bool flushImmediate = false);
		void AliasingBarrier(FGpuResource& resource, bool flushImmediate = false);
		void FlushResourceBarriers();

		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap);
		void SetDescriptorHeaps(UINT count, D3D12_DESCRIPTOR_HEAP_TYPE types[], ID3D12DescriptorHeap* heaps[]);

	protected:

		void BindDescriptorHeaps();

		FCommandList* mCommandList;
		ID3D12CommandList* mD3DCommandList;

		std::shared_ptr<FGpuResourcesStateTracker> mResourceStateTracker;

		// Keep track of the currently bound descriptor heaps. Only change descriptor
		// heaps if they are different than the currently bound descriptor heaps.
		ID3D12DescriptorHeap* mDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		FDynamicDescriptorHeap mDynamicViewDescriptor;
		FDynamicDescriptorHeap mDynamicSamplerDescriptor;

		using FTrackedObjects = std::vector<Microsoft::WRL::ComPtr<ID3D12Object>>;
		FTrackedObjects mTrackedObjects;
	};
}