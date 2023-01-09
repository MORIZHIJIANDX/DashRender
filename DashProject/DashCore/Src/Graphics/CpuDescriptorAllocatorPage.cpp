#include "PCH.h"
#include "CpuDescriptorAllocatorPage.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"
#include "RenderDevice.h"

namespace Dash
{
	FCpuDescriptorAllocatorPage::FCpuDescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
	: mDescriptorHeapType(type)
	, mNumFreeHandels(numDescriptors)
	, mNumDescriptorsInHeap(numDescriptors)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = type;
		desc.NumDescriptors = numDescriptors;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		DX_CALL(FGraphicsCore::Device->CreateDescriptorHeap(&desc, mDescriptorHeap));

		SetD3D12DebugName(mDescriptorHeap.Get(), "CpuDescriptorHeap");

		mBaseDescriptor = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		mDescriptorHandelIncrementSize = FGraphicsCore::Device->GetDescriptorHandleIncrementSize(type);

		AddNewBlock(0, numDescriptors);
	}

	FCpuDescriptorAllocatorPage::~FCpuDescriptorAllocatorPage()
	{
		mNumFreeHandels = 0;
		mNumDescriptorsInHeap = 0;
		mDescriptorHeap = nullptr;
	}

	D3D12_DESCRIPTOR_HEAP_TYPE FCpuDescriptorAllocatorPage::GetHeapType() const
	{
		return mDescriptorHeapType;
	}

	bool FCpuDescriptorAllocatorPage::HasSpace(uint32_t numDescriptors) const
	{
		return mFreeListBySize.lower_bound(numDescriptors) != mFreeListBySize.end();
	}

	uint32_t FCpuDescriptorAllocatorPage::NumFreeHandles() const
	{
		return mNumFreeHandels;
	}

	FCpuDescriptorAllocation FCpuDescriptorAllocatorPage::Allocate(uint32_t numDescriptors)
	{
		std::lock_guard<std::mutex> lock(mAllocationMutex);

		if (numDescriptors > mNumFreeHandels)
		{
			return FCpuDescriptorAllocation();
		}

		auto bestFitBlockIter = mFreeListBySize.lower_bound(numDescriptors);
		if (bestFitBlockIter == mFreeListBySize.end())
		{
			return FCpuDescriptorAllocation();
		}

		SizeType blockSize = bestFitBlockIter->first;
		auto offsetIter = bestFitBlockIter->second;

		OffsetType blockOffset = offsetIter->first;

		mFreeListBySize.erase(bestFitBlockIter);
		mFreeListByOffset.erase(offsetIter);

		auto newBlockOffset = blockOffset + numDescriptors;
		auto newBlockSize = blockSize - numDescriptors;

		if (newBlockSize > 0)
		{
			AddNewBlock(newBlockOffset, newBlockSize);
		}

		mNumFreeHandels -= numDescriptors;

		return FCpuDescriptorAllocation(CD3DX12_CPU_DESCRIPTOR_HANDLE(mBaseDescriptor, blockOffset, mDescriptorHandelIncrementSize), 
										numDescriptors, 
										mDescriptorHandelIncrementSize, 
										shared_from_this());
	}

	void FCpuDescriptorAllocatorPage::Free(FCpuDescriptorAllocation&& allocation)
	{
		OffsetType offset = ComputeOffset(allocation.GetDescriptorHandle());

		std::lock_guard<std::mutex> lock(mAllocationMutex);

		mStaleDescriptorQueue.emplace(offset, allocation.GetNumDescriptors());
	}

	void FCpuDescriptorAllocatorPage::ReleaseStaleDescriptors()
	{
		std::lock_guard<std::mutex> lock(mAllocationMutex);

		while (!mStaleDescriptorQueue.empty())
		{
			FStaleDescriptorInfo& info = mStaleDescriptorQueue.front();	
			AddNewBlock(info.Offset, info.Size);
			mStaleDescriptorQueue.pop();
		}
	}

	uint32_t FCpuDescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const
	{
		return static_cast<OffsetType>(descriptor.ptr - mBaseDescriptor.ptr)/mDescriptorHandelIncrementSize;
	}

	void FCpuDescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t numDescriptors)
	{
		auto offsetIter = mFreeListByOffset.emplace(offset, numDescriptors);
		auto sizeIter = mFreeListBySize.emplace(numDescriptors, offsetIter.first);

		offsetIter.first->second.FreeListBySizeIter = sizeIter;
	}

	void FCpuDescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t numDescriptors)
	{
		auto nextBlockIter = mFreeListByOffset.upper_bound(offset);
		
		auto prevBlockIter = nextBlockIter;
		if (prevBlockIter != mFreeListByOffset.begin())
		{
			--prevBlockIter;
		}
		else
		{
			prevBlockIter = mFreeListByOffset.end();
		}

		mNumFreeHandels += numDescriptors;

		if (prevBlockIter != mFreeListByOffset.end() && ((prevBlockIter->first + prevBlockIter->second.Size) == offset))
		{
			offset = prevBlockIter->first;
			numDescriptors += prevBlockIter->second.Size;

			mFreeListBySize.erase(prevBlockIter->second.FreeListBySizeIter);
			mFreeListByOffset.erase(prevBlockIter);
		}

		if (nextBlockIter != mFreeListByOffset.end() && (nextBlockIter->first == (offset + numDescriptors)))
		{
			numDescriptors += nextBlockIter->second.Size;

			mFreeListBySize.erase(nextBlockIter->second.FreeListBySizeIter);
			mFreeListByOffset.erase(nextBlockIter);
		}

		AddNewBlock(offset, numDescriptors);
	}

}