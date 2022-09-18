#pragma once

#include "CpuDescriptorAllocation.h"

namespace Dash
{
	class CpuDescriptorAllocatorPage;
		
	class CpuDescriptorAllocator
	{
	public:
		CpuDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t defaultHeapSize = 256);
		~CpuDescriptorAllocator();

		CpuDescriptorAllocation Allocate(uint32_t numDescriptors);

		void ReleaseStaleDescriptors();

		void Destroy();
	private:
		std::shared_ptr<CpuDescriptorAllocatorPage> RequestNewHeap(uint32_t heapSize = 0);
		
	private:
		using DescriptorHeapPool = std::vector<std::shared_ptr<CpuDescriptorAllocatorPage>>;

		D3D12_DESCRIPTOR_HEAP_TYPE mType;
		uint32_t mDefaultHeapSize;

		DescriptorHeapPool mHeapPool;
		std::set<uint32_t> mAvailableHeaps;

		std::mutex mAllocationMutex;
	};
}