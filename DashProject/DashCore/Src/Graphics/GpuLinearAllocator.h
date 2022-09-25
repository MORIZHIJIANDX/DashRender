#pragma once

#include "GpuResource.h"

namespace Dash
{
	// Constant blocks must be multiples of 16 constants @ 16 bytes each
	#define DEFAULT_ALIGN 256

	class GpuLinearAllocator
	{
	public:
		enum AllocatorType
		{
			Invalid = -1,
			GpuExclusive = 0,	// DEFAULT GPU-writeable (via UAV)
			CpuExclusive = 1,	// UPLOAD CPU-writeable (but write combined)

			NumAllocatorTypes
		};

		enum
		{
			GpuDefaultPageSize = 0x10000,	// 64K
			CpuDefaultPageSize = 0x200000	// 2MB
		};

		struct Allocation
		{
			Allocation(FGpuResource& resource, size_t thisOffset, size_t thisSize)
				: Resource(resource)
				, Offset(thisOffset)
				, Size(thisSize)
			{
			}

			FGpuResource& Resource;
			size_t Offset;
			size_t Size;
			void* CpuAddress = nullptr;
			D3D12_GPU_VIRTUAL_ADDRESS GpuAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
		};

	protected:
		class Page : public FGpuResource
		{
		public:
			Page(ID3D12Resource* resource, D3D12_RESOURCE_STATES defaultState, size_t pageSize)
				: mPageSie(pageSize)
				, mOffset(0)
			{
				mResource.Attach(resource);
				GpuResourcesStateTracker::AddGlobalResourceState(resource, defaultState);
				mGpuAddress = mResource->GetGPUVirtualAddress();
				mResource->Map(0, nullptr, &mCpuAddress);
			}

			// Check to see if the page has room to satisfy the requested
			// allocation.
			bool HasSpace(size_t sizeInBytes, size_t alignment) const;

			Allocation Allocate(size_t sizeInBytes, size_t alignment);

			~Page()
			{
				mResource->Unmap(0, nullptr);
				mCpuAddress = nullptr;
			}

		private:
			void* mCpuAddress = nullptr;
			D3D12_GPU_VIRTUAL_ADDRESS mGpuAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

			size_t mPageSie = 0;
			size_t mOffset = 0;
		};

		class PageManager
		{
		public:
			PageManager();

			Page* RequestPage();
			Page* RequestLargePage(size_t pageSize = 0);

			Page* CreateNewPage(size_t pageSize = 0);

			// Discarded pages will get recycled.
			void DiscardPages(uint64_t fenceID, const std::vector<Page*>& pages, bool isLargePage = false);

			void Destroy();

		private:
			static AllocatorType AutoAllocatorType;

			AllocatorType mAllocatorType;
			std::vector<std::unique_ptr<Page>> mPagePool;
			std::vector<std::unique_ptr<Page>> mLargePagePool;
			std::queue<std::pair<uint64_t, Page*>> mRetiredPages;
			std::queue<std::pair<uint64_t, Page*>> mRetiredLargePages; //DeletionQueue
			std::queue<Page*> mAvailablePages;
			std::mutex mMutex;
		};

	public:
		
		GpuLinearAllocator(AllocatorType type) 
		: mAllocatorType(type)
		, mDefaultPageSize(0)
		, mCurrentPage(nullptr)
		{
			ASSERT(type > Invalid && type < NumAllocatorTypes);
			mDefaultPageSize = type == GpuExclusive ? GpuDefaultPageSize : CpuDefaultPageSize;
		}

		Allocation Allocate(size_t sizeInBytes, size_t alignment = DEFAULT_ALIGN);

		void RetireUsedPages(uint64_t fenceValue);

		static void Destroy();

	private:
		AllocatorType mAllocatorType;
		size_t mDefaultPageSize;

		Page* mCurrentPage = nullptr;
		std::vector<Page*> mRetiredPages;	
		std::vector<Page*> mRetiredLargePages;

		static PageManager AllocatorPageManger[2];
	};
}