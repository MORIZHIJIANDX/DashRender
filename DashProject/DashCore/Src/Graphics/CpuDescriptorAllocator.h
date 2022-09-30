#pragma once

#include "CpuDescriptorAllocation.h"

namespace Dash
{
	class FCpuDescriptorAllocatorPage;
		
	class FCpuDescriptorAllocator
	{
	public:
		FCpuDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t defaultHeapSize = 256);
		~FCpuDescriptorAllocator();

		FCpuDescriptorAllocation Allocate(uint32_t numDescriptors = 1);

		void ReleaseStaleDescriptors();

		void Destroy();
	private:
		std::shared_ptr<FCpuDescriptorAllocatorPage> RequestNewHeap(uint32_t heapSize = 0);
		
	private:
		using DescriptorHeapPool = std::vector<std::shared_ptr<FCpuDescriptorAllocatorPage>>;

		D3D12_DESCRIPTOR_HEAP_TYPE mType;
		uint32_t mDefaultHeapSize;

		DescriptorHeapPool mHeapPool;
		std::set<uint32_t> mAvailableHeaps;

		std::mutex mAllocationMutex;
	};

	class FCpuDescriptorAllocatorManager
	{
	public:
		FCpuDescriptorAllocatorManager();
		~FCpuDescriptorAllocatorManager() {};

		FCpuDescriptorAllocation Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1);

		FCpuDescriptorAllocation AllocateCbvDescriptor(uint32_t numDescriptors = 1);
		FCpuDescriptorAllocation AllocateSrvDescriptor(uint32_t numDescriptors = 1);
		FCpuDescriptorAllocation AllocateUavDescriptor(uint32_t numDescriptors = 1);

		FCpuDescriptorAllocation AllocateSamplerDescriptor(uint32_t numDescriptors = 1);

		void Destroy();

	private:
		FCpuDescriptorAllocator mCbvSrvUavAllocator;
		FCpuDescriptorAllocator mSamplerAllocator;
	};
}