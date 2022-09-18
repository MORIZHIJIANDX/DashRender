#include "PCH.h"
#include "CpuDescriptorAllocatorPage.h"
#include "GraphicsCore.h"
#include "..\Utility\Exception.h"

namespace Dash
{
	CpuDescriptorAllocatorPage::CpuDescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
	: mDescriptorHeapType(type)
	, mNumFreeHandels(numDescriptors)
	, mNumDescriptorsInHeap(numDescriptors)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = type;
		desc.NumDescriptors = numDescriptors;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		DX_CALL(Graphics::Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mDescriptorHeap)));

		mBaseDescriptor = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		mDescriptorHandelIncrementSize = Graphics::Device->GetDescriptorHandleIncrementSize(type);

		AddNewBlock(0, numDescriptors);
	}

	CpuDescriptorAllocatorPage::~CpuDescriptorAllocatorPage()
	{
		mNumFreeHandels = 0;
		mNumDescriptorsInHeap = 0;
		mDescriptorHeap = nullptr;
	}

	D3D12_DESCRIPTOR_HEAP_TYPE CpuDescriptorAllocatorPage::GetHeapType() const
	{
		return mDescriptorHeapType;
	}

	bool CpuDescriptorAllocatorPage::HasSpace(uint32_t numDescriptors) const
	{
		return mFreeListBySize.lower_bound(numDescriptors) != mFreeListBySize.end();
	}

	uint32_t CpuDescriptorAllocatorPage::NumFreeHandles() const
	{
		return mNumFreeHandels;
	}

	CpuDescriptorAllocation CpuDescriptorAllocatorPage::Allocate(uint32_t numDescriptors)
	{
		if (numDescriptors > mNumFreeHandels)
		{
			return CpuDescriptorAllocation();
		}

		auto bestFitBlockIter = mFreeListBySize.lower_bound(numDescriptors);
		if (bestFitBlockIter == mFreeListBySize.end())
		{
			return CpuDescriptorAllocation();
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

		return CpuDescriptorAllocation(CD3DX12_CPU_DESCRIPTOR_HANDLE(mBaseDescriptor, blockOffset, mDescriptorHandelIncrementSize), 
										numDescriptors, 
										mDescriptorHandelIncrementSize, 
										shared_from_this());
	}

	void CpuDescriptorAllocatorPage::Free(CpuDescriptorAllocation&& allocation)
	{
		OffsetType offset = ComputeOffset(allocation.GetDescriptorHandle());
		mStaleDescriptorQueue.emplace(offset, allocation.GetNumDescriptors());
	}

	void CpuDescriptorAllocatorPage::ReleaseStaleDescriptors()
	{
		while (!mStaleDescriptorQueue.empty())
		{
			StaleDescriptorInfo& info = mStaleDescriptorQueue.front();	
			AddNewBlock(info.Offset, info.Size);
			mStaleDescriptorQueue.pop();
		}
	}

	uint32_t CpuDescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const
	{
		return static_cast<OffsetType>(descriptor.ptr - mBaseDescriptor.ptr)/mDescriptorHandelIncrementSize;
	}

	void CpuDescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t numDescriptors)
	{
		auto offsetIter = mFreeListByOffset.emplace(offset, numDescriptors);
		auto sizeIter = mFreeListBySize.emplace(numDescriptors, offsetIter.first);

		offsetIter.first->second.FreeListBySizeIter = sizeIter;
	}

	void CpuDescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t numDescriptors)
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