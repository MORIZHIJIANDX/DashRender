#include "PCH.h"
#include "GpuLinearAllocator.h"
#include "GraphicsCore.h"
#include "../Utility/Exception.h"

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

	GpuLinearAllocator::Page* GpuLinearAllocator::PageManager::RequestLargePage(size_t pageSize)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		auto IsFenceCompleted = [](int64_t fence)
		{
			return true;
		};

		while (!mRetiredLargePages.empty() && IsFenceCompleted(mRetiredLargePages.front().first))
		{
			Page* pageToFree = mRetiredLargePages.front().second;
			auto iter = std::find_if(mLargePagePool.begin(), mLargePagePool.end(),[&pageToFree](std::unique_ptr<GpuLinearAllocator::Page>& page)
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

	GpuLinearAllocator::Page* GpuLinearAllocator::PageManager::CreateNewPage(size_t pageSize /*= 0*/)
	{
		D3D12_HEAP_PROPERTIES heapProps;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDesc;
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

		return new Page(Buffer, resourceState);
	}

	void GpuLinearAllocator::PageManager::DiscardPages(uint64_t fenceID, const std::vector<Page*>& pages, bool isLargePage)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (isLargePage)
		{
			for (Page* page : pages)
			{
				mRetiredLargePages.push(std::make_pair(fenceID, page));
			}
		}
		else
		{
			for (Page* page : pages)
			{
				mRetiredPages.push(std::make_pair(fenceID, page));
			}
		}
	}

	void GpuLinearAllocator::PageManager::Destroy()
	{
		std::lock_guard<std::mutex> lock(mMutex);

		mPagePool.clear();
		mLargePagePool.clear();
	}

	GpuLinearAllocator::Allocation GpuLinearAllocator::Allocate(size_t sizeInBytes, size_t alignment)
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

	void GpuLinearAllocator::RetireUsedPages(uint64_t fenceID)
	{
		if (mCurrentPage)
		{
			mRetiredPages.push_back(mCurrentPage);
			mCurrentPage = nullptr;
		}

		if (!mRetiredPages.empty())
		{	
			AllocatorPageManger[mAllocatorType].DiscardPages(fenceID, mRetiredPages, false);
			mRetiredPages.clear();
		}

		if (!mRetiredLargePages.empty())
		{
			AllocatorPageManger[mAllocatorType].DiscardPages(fenceID, mRetiredLargePages, true);
			mRetiredLargePages.clear();
		}
	}

	void GpuLinearAllocator::Destroy()
	{
		for (int Index = GpuExclusive; Index < NumAllocatorTypes; ++Index)
		{
			AllocatorPageManger[Index].Destroy();
		}
	}
}