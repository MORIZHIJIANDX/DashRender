#include "PCH.h"
#include "GpuLinearAllocator.h"

namespace Dash
{
	GpuLinearAllocator::AllocatorType GpuLinearAllocator::PageManager::AutoAllocatorType = CpuExclusive;
	GpuLinearAllocator::PageManager GpuLinearAllocator::AllocatorPageManger[2];
	 
	bool GpuLinearAllocator::Page::HasSpace(size_t sizeInBytes, size_t alignment) const
	{
		size_t alignedSize = FMath::AlignUp(sizeInBytes, alignment);
		size_t alignedOffset = FMath::AlignUp(mOffset, alignment);

		return alignedOffset + alignedSize <= mPageSie;
	}

	GpuLinearAllocator::Allocation GpuLinearAllocator::Page::Allocate(size_t sizeInBytes, size_t alignment)
	{
		size_t alignedSize = FMath::AlignUp(sizeInBytes, alignment);
		mOffset = FMath::AlignUp(mOffset, alignment);

		GpuLinearAllocator::Allocation allocation{*this, mOffset, alignedSize};
		allocation.CpuAddress = static_cast<uint8_t*>(mCpuAddress) + mOffset;
		allocation.GpuAddress = mGpuAddress + mOffset;

		mOffset += alignedSize;

		return allocation;
	} 

	GpuLinearAllocator::PageManager::PageManager()
	{
		mAllocatorType = AutoAllocatorType;
		AutoAllocatorType = (GpuLinearAllocator::AllocatorType)(AutoAllocatorType + 1);
		ASSERT(AutoAllocatorType <= NumAllocatorTypes);
	}

	GpuLinearAllocator::Page* GpuLinearAllocator::PageManager::RequestPage()
	{
		std::lock_guard<std::mutex> lock(mMutex);

		auto IsFenceCompleted = [](int64_t fence)
		{
			return true;
		};

		while (!mRetiredPages.empty() && IsFenceCompleted(mRetiredPages.front().first))
		{
			mAvailablePages.push(mRetiredPages.front().second);
			mRetiredPages.pop();
		}

		GpuLinearAllocator::Page* newPage = nullptr;

		if (!mAvailablePages.empty())
		{
			newPage = mAvailablePages.front();
		}
		else
		{
			newPage = CreateNewPage();
			mPagePool.emplace_back(newPage);
		}

		return newPage;
	}

	GpuLinearAllocator::Page* GpuLinearAllocator::PageManager::CreateNewPage(size_t pageSize /*= 0*/)
	{
		
	}

	void GpuLinearAllocator::PageManager::DiscardPages(uint64_t fenceID, const std::vector<Page*>& pages)
	{
		
	}

}