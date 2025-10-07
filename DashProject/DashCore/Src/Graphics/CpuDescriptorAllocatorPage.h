#pragma once

#include "CpuDescriptorAllocation.h"
#include "DX12Helper.h"

namespace Dash
{
	class FCpuDescriptorAllocatorPage : public std::enable_shared_from_this<FCpuDescriptorAllocatorPage>
	{
	public:
		FCpuDescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 numDescriptors);
		~FCpuDescriptorAllocatorPage();

		D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const;

		bool HasSpace(uint32 numDescriptors) const;

		uint32 NumFreeHandles() const;

		FCpuDescriptorAllocation Allocate(uint32 numDescriptors);

		void Free(FCpuDescriptorAllocation&& allocation);

		void ReleaseStaleDescriptors();

	private:
		uint32 ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const;

		void AddNewBlock(uint32 offset, uint32 numDescriptors);

		void FreeBlock(uint32 offset, uint32 numDescriptors);

	private:
		using OffsetType = uint32;
		using SizeType = uint32;

		struct FFreeBlockInfo;

		using FreeListByOffset = std::map<OffsetType, FFreeBlockInfo>;

		using FreeListBySize = std::multimap<SizeType, FreeListByOffset::iterator>;

		struct FFreeBlockInfo
		{
			FFreeBlockInfo(SizeType size)
				: Size(size)
			{}

			SizeType Size;
			FreeListBySize::iterator FreeListBySizeIter;
		};

		struct FStaleDescriptorInfo
		{
			FStaleDescriptorInfo(OffsetType offset, SizeType size)
			: Offset(offset)
			, Size(size)
			{}

			OffsetType Offset;
			SizeType Size;
		};

		using StaleDescriptorQueue = std::queue<FStaleDescriptorInfo>;

		FreeListByOffset mFreeListByOffset;
		FreeListBySize mFreeListBySize;
		StaleDescriptorQueue mStaleDescriptorQueue;

		TRefCountPtr<ID3D12DescriptorHeap> mDescriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE mDescriptorHeapType;
		D3D12_CPU_DESCRIPTOR_HANDLE mBaseDescriptor;

		uint32 mDescriptorHandelIncrementSize = 0;
		uint32 mNumFreeHandels = 0;
		uint32 mNumDescriptorsInHeap = 0;

		std::mutex mAllocationMutex;
	};
}