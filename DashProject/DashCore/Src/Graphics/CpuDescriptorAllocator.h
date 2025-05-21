#pragma once

#include "CpuDescriptorAllocation.h"

namespace Dash
{
	class FCpuDescriptorAllocatorPage;
		
	class FCpuDescriptorAllocator
	{
	public:
		FCpuDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 defaultHeapSize = 256);
		~FCpuDescriptorAllocator();

		FCpuDescriptorAllocation Allocate(uint32 numDescriptors = 1);

		void ReleaseStaleDescriptors();

		void Destroy();
	private:
		std::shared_ptr<FCpuDescriptorAllocatorPage> RequestNewHeap(uint32 heapSize = 0);
		
	private:
		using DescriptorHeapPool = std::vector<std::shared_ptr<FCpuDescriptorAllocatorPage>>;

		D3D12_DESCRIPTOR_HEAP_TYPE mType;
		uint32 mDefaultHeapSize;

		DescriptorHeapPool mHeapPool;
		std::set<uint32> mAvailableHeaps;

		std::mutex mAllocationMutex;
	};

	class FCpuDescriptorAllocatorManager
	{
	public:
		FCpuDescriptorAllocatorManager();
		~FCpuDescriptorAllocatorManager() {};

		FCpuDescriptorAllocation Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 numDescriptors = 1);

		FCpuDescriptorAllocation AllocateCBVDescriptor(uint32 numDescriptors = 1);
		FCpuDescriptorAllocation AllocateSRVDescriptor(uint32 numDescriptors = 1);
		FCpuDescriptorAllocation AllocateUAVDescriptor(uint32 numDescriptors = 1);
		FCpuDescriptorAllocation AllocateRTVDescriptor(uint32 numDescriptors = 1);
		FCpuDescriptorAllocation AllocateDSVDescriptor(uint32 numDescriptors = 1);

		FCpuDescriptorAllocation AllocateSamplerDescriptor(uint32 numDescriptors = 1);

		void ReleaseStaleDescriptors();

		void Destroy();

	private:
		FCpuDescriptorAllocator mCbvSrvUavAllocator;
		FCpuDescriptorAllocator mRTVAllocator;
		FCpuDescriptorAllocator mDSVAllocator;
		FCpuDescriptorAllocator mSamplerAllocator;
	};
}