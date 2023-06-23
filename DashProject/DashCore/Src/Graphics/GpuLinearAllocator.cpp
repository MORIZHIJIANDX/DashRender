#include "PCH.h"
#include "GpuLinearAllocator.h"
#include "GraphicsCore.h"
#include "CommandQueue.h"
#include "DX12Helper.h"
#include "GpuResourcesStateTracker.h"
#include "RenderDevice.h"

namespace Dash
{
	FGpuLinearAllocator::AllocatorType FGpuLinearAllocator::FPageManager::AutoAllocatorType = GpuExclusive;
	FGpuLinearAllocator::FPageManager FGpuLinearAllocator::AllocatorPageManger[2];
	 
	FGpuLinearAllocator::FPage::FPage(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES defaultState, size_t pageSize)
		: mPageSie(pageSize)
		, mOffset(0)
	{
		mResource = resource;
		FGpuResourcesStateTracker::AddGlobalResourceState(resource.Get(), defaultState);
		mGpuAddress = mResource->GetGPUVirtualAddress();
		mResource->Map(0, nullptr, &mCpuAddress);
	}

	bool FGpuLinearAllocator::FPage::HasSpace(size_t sizeInBytes, size_t alignment) const
	{
		size_t alignedSize = FMath::AlignUp(sizeInBytes, alignment);
		size_t alignedOffset = FMath::AlignUp(mOffset, alignment);

		return alignedOffset + alignedSize <= mPageSie;
	}

	FGpuLinearAllocator::FAllocation FGpuLinearAllocator::FPage::Allocate(size_t sizeInBytes, size_t alignment)
	{
		size_t alignedSize = FMath::AlignUp(sizeInBytes, alignment);
		mOffset = FMath::AlignUp(mOffset, alignment);

		FGpuLinearAllocator::FAllocation allocation{*this, mOffset, alignedSize};
		allocation.CpuAddress = static_cast<uint8_t*>(mCpuAddress) + mOffset;
		allocation.GpuAddress = mGpuAddress + mOffset;

		mOffset += alignedSize;

		return allocation;
	} 

	FGpuLinearAllocator::FPageManager::FPageManager()
	{
		mAllocatorType = AutoAllocatorType;
		AutoAllocatorType = (FGpuLinearAllocator::AllocatorType)(AutoAllocatorType + 1);
		ASSERT(AutoAllocatorType <= NumAllocatorTypes);
	}

	FGpuLinearAllocator::FPage* FGpuLinearAllocator::FPageManager::RequestPage()
	{
		std::lock_guard<std::mutex> lock(mMutex);

		while (!mRetiredPages.empty() && FGraphicsCore::CommandQueueManager->IsFenceCompleted(mRetiredPages.front().first))
		{
			mAvailablePages.push(mRetiredPages.front().second);
			mRetiredPages.pop();
		}

		FGpuLinearAllocator::FPage* newPage = nullptr;

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

	FGpuLinearAllocator::FPage* FGpuLinearAllocator::FPageManager::RequestLargePage(size_t pageSize)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		while (!mRetiredLargePages.empty() && FGraphicsCore::CommandQueueManager->IsFenceCompleted(mRetiredLargePages.front().first))
		{
			FPage* pageToFree = mRetiredLargePages.front().second;
			auto iter = std::find_if(mLargePagePool.begin(), mLargePagePool.end(),[&pageToFree](std::unique_ptr<FGpuLinearAllocator::FPage>& page)
			{ 
				return page.get() == pageToFree; 
			});

			if (iter != mLargePagePool.end())
			{
				mLargePagePool.erase(iter);
			}
		}

		FPage* newPage = CreateNewPage(pageSize);
		mLargePagePool.emplace_back(newPage);

		return newPage;
	}

	FGpuLinearAllocator::FPage* FGpuLinearAllocator::FPageManager::CreateNewPage(size_t pageSize /*= 0*/)
	{
		D3D12_HEAP_PROPERTIES heapProps{};
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

		D3D12_RESOURCE_STATES resourceState{};
	
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

		Microsoft::WRL::ComPtr<ID3D12Resource> Buffer;
		DX_CALL(FGraphicsCore::Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, resourceState, nullptr, Buffer));

		SetD3D12DebugName(Buffer.Get(), "CpuLinearAllocatorPage");

		return new FPage(Buffer, resourceState, resourceDesc.Width);
	}

	void FGpuLinearAllocator::FPageManager::DiscardPages(uint64_t fenceValue, const std::vector<FPage*>& pages, bool isLargePage)
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (isLargePage)
		{
			for (FPage* page : pages)
			{
				mRetiredLargePages.push(std::make_pair(fenceValue, page));
			}
		}
		else
		{
			for (FPage* page : pages)
			{
				mRetiredPages.push(std::make_pair(fenceValue, page));
			}
		}
	}

	void FGpuLinearAllocator::FPageManager::Destroy()
	{
		std::lock_guard<std::mutex> lock(mMutex);

		mPagePool.clear();
		mLargePagePool.clear();
	}

	FGpuLinearAllocator::FAllocation FGpuLinearAllocator::Allocate(size_t sizeInBytes, size_t alignment)
	{
		const size_t alignmentMask = alignment - 1;

		// Assert that it's a power of two.
		ASSERT((alignmentMask & alignment) == 0);

		// Align the allocation
		const size_t alignedSize = FMath::AlignUpWithMask(sizeInBytes, alignmentMask);

		if (alignedSize > mDefaultPageSize)
		{
			FPage* newPage = AllocatorPageManger[mAllocatorType].RequestLargePage(alignedSize);
			mRetiredLargePages.push_back(newPage);
			FAllocation allocation(*newPage, 0, alignedSize);
			return allocation;
		}

		if (mCurrentPage == nullptr || !mCurrentPage->HasSpace(alignedSize, alignment))
		{
			if (mCurrentPage)
			{
				mRetiredPages.push_back(mCurrentPage);
			}

			mCurrentPage = AllocatorPageManger[mAllocatorType].RequestPage();
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