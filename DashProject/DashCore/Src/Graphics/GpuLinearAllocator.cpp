#include "PCH.h"
#include "GpuLinearAllocator.h"
#include "GraphicsCore.h"
#include "CommandQueue.h"
#include "../Utility/Exception.h"

namespace Dash
{
	FGpuLinearAllocator::AllocatorType FGpuLinearAllocator::PageManager::AutoAllocatorType = CpuExclusive;
	FGpuLinearAllocator::PageManager FGpuLinearAllocator::AllocatorPageManger[2];
	 
	bool FGpuLinearAllocator::Page::HasSpace(size_t sizeInBytes, size_t alignment) const
	{
		size_t alignedSize = FMath::AlignUp(sizeInBytes, alignment);
		size_t alignedOffset = FMath::AlignUp(mOffset, alignment);

		return alignedOffset + alignedSize <= mPageSie;
	}

	FGpuLinearAllocator::Allocation FGpuLinearAllocator::Page::Allocate(size_t sizeInBytes, size_t alignment)
	{
		size_t alignedSize = FMath::AlignUp(sizeInBytes, alignment);
		mOffset = FMath::AlignUp(mOffset, alignment);

		FGpuLinearAllocator::Allocation allocation{*this, mOffset, alignedSize};
		allocation.CpuAddress = static_cast<uint8_t*>(mCpuAddress) + mOffset;
		allocation.GpuAddress = mGpuAddress + mOffset;

		mOffset += alignedSize;

		return allocation;
	} 

	FGpuLinearAllocator::PageManager::PageManager()
	{
		mAllocatorType = AutoAllocatorType;
		AutoAllocatorType = (FGpuLinearAllocator::AllocatorType)(AutoAllocatorType + 1);
		ASSERT(AutoAllocatorType <= NumAllocatorTypes);
	}

	FGpuLinearAllocator::Page* FGpuLinearAllocator::PageManager::RequestPage()
	{
		std::lock_guard<std::mutex> lock(mMutex);

		while (!mRetiredPages.empty() && Graphics::QueueManager->IsFenceCompleted(mRetiredPages.front().first))
		{
			mAvailablePages.push(mRetiredPages.front().second);
			mRetiredPages.pop();
		}

		FGpuLinearAllocator::Page* newPage = nullptr;

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

	FGpuLinearAllocator::Page* FGpuLinearAllocator::PageManager::RequestLargePage(size_t pageSize)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		while (!mRetiredLargePages.empty() && Graphics::QueueManager->IsFenceCompleted(mRetiredLargePages.front().first))
		{
			Page* pageToFree = mRetiredLargePages.front().second;
			auto iter = std::find_if(mLargePagePool.begin(), mLargePagePool.end(),[&pageToFree](std::unique_ptr<FGpuLinearAllocator::Page>& page)
			{ 
				return page.get() == pageToFree; 
			});

			if (iter != mLargePagePool.end())
			{
				mLargePagePool.erase(iter);
			}
		}

		Page* newPage = CreateNewPage(pageSize);
		mLargePagePool.emplace_back(newPage);

		return nullptr;
	}

	FGpuLinearAllocator::Page* FGpuLinearAllocator::PageManager::CreateNewPage(size_t pageSize /*= 0*/)
	{
		D3D12_HEAP_PROPERTIES heapProps;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		D3D12_RESOURCE_STATES resourceState;
	
		if (mAllocatorType == GpuExclusive)
		{
			heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
			
			resourceDesc.Width = (pageSize == 0) ? GpuDefaultPageSize : pageSize;
			resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

			resourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		}
		else
		{
			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

			resourceDesc.Width = (pageSize == 0) ? CpuDefaultPageSize : pageSize;
			resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			resourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
		}

		ID3D12Resource* Buffer;
		DX_CALL(Graphics::Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, resourceState, nullptr, IID_PPV_ARGS(&Buffer)));

		return new Page(Buffer, resourceState, resourceDesc.Width);
	}

	void FGpuLinearAllocator::PageManager::DiscardPages(uint64_t fenceValue, const std::vector<Page*>& pages, bool isLargePage)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (isLargePage)
		{
			for (Page* page : pages)
			{
				mRetiredLargePages.push(std::make_pair(fenceValue, page));
			}
		}
		else
		{
			for (Page* page : pages)
			{
				mRetiredPages.push(std::make_pair(fenceValue, page));
			}
		}
	}

	void FGpuLinearAllocator::PageManager::Destroy()
	{
		std::lock_guard<std::mutex> lock(mMutex);

		mPagePool.clear();
		mLargePagePool.clear();
	}

	FGpuLinearAllocator::Allocation FGpuLinearAllocator::Allocate(size_t sizeInBytes, size_t alignment)
	{
		const size_t alignmentMask = alignment - 1;

		// Assert that it's a power of two.
		ASSERT((alignmentMask & alignment) == 0);

		// Align the allocation
		const size_t alignedSize = FMath::AlignUpWithMask(sizeInBytes, alignmentMask);

		if (alignedSize > mDefaultPageSize)
		{
			Page* newPage = AllocatorPageManger[mAllocatorType].RequestLargePage(alignedSize);
			mRetiredLargePages.push_back(newPage);
			Allocation allocation(*newPage, 0, alignedSize);
			return allocation;
		}

		if (mCurrentPage == nullptr || !mCurrentPage->HasSpace(alignedSize, alignment))
		{
			if (mCurrentPage)
			{
				mRetiredPages.push_back(mCurrentPage);
			}

			mCurrentPage = AllocatorPageManger->RequestPage();
		}

		return mCurrentPage->Allocate(alignedSize, alignment);
	}

	void FGpuLinearAllocator::RetireUsedPages(uint64_t fenceValue)
	{
		if (mCurrentPage)
		{
			mRetiredPages.push_back(mCurrentPage);
			mCurrentPage = nullptr;
		}

		if (!mRetiredPages.empty())
		{	
			AllocatorPageManger[mAllocatorType].DiscardPages(fenceValue, mRetiredPages, false);
			mRetiredPages.clear();
		}

		if (!mRetiredLargePages.empty())
		{
			AllocatorPageManger[mAllocatorType].DiscardPages(fenceValue, mRetiredLargePages, true);
			mRetiredLargePages.clear();
		}
	}

	void FGpuLinearAllocator::Destroy()
	{
		for (int Index = GpuExclusive; Index < NumAllocatorTypes; ++Index)
		{
			AllocatorPageManger[Index].Destroy();
		}
	}
}