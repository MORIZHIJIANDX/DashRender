#pragma once

#include "d3dx12.h"
#include "CpuDescriptorAllocation.h"

namespace Dash
{
	class CpuDescriptorAllocatorPage : public std::enable_shared_from_this<CpuDescriptorAllocatorPage>
	{
	public:
		CpuDescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
		~CpuDescriptorAllocatorPage();

		D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const;

		bool HasSpace(uint32_t numDescriptors) const;

		uint32_t NumFreeHandles() const;

		CpuDescriptorAllocation Allocate(uint32_t numDescriptors);

		void Free(CpuDescriptorAllocation&& allocation);

		void ReleaseStaleDescriptors();

	private:
		uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const;

		void AddNewBlock(uint32_t offset, uint32_t numDescriptors);

		void FreeBlock(uint32_t offset, uint32_t numDescriptors);

	private:
		using OffsetType = uint32_t;
		using SizeType = uint32_t;

		struct FreeBlockInfo;

		using FreeListByOffset = std::map<OffsetType, FreeBlockInfo>;

		using FreeListBySize = std::multimap<SizeType, FreeListByOffset::iterator>;

		struct FreeBlockInfo
		{
			FreeBlockInfo(SizeType size)
				: Size(size)
			{}

			SizeType Size;
			FreeListBySize::iterator FreeListBySizeIter;
		};

		struct StaleDescriptorInfo
		{
			StaleDescriptorInfo(OffsetType offset, SizeType size)
			: Offset(offset)
			, Size(size)
			{}

			OffsetType Offset;
			SizeType Size;
		};

		using StaleDescriptorQueue = std::queue<StaleDescriptorInfo>;

		FreeListByOffset mFreeListByOffset;
		FreeListBySize mFreeListBySize;
		StaleDescriptorQueue mStaleDescriptorQueue;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE mDescriptorHeapType;
		D3D12_CPU_DESCRIPTOR_HANDLE mBaseDescriptor;

		uint32_t mDescriptorHandelIncrementSize = 0;
		uint32_t mNumFreeHandels = 0;
		uint32_t mNumDescriptorsInHeap = 0;

		std::mutex mAllocationMutex;
	};
}