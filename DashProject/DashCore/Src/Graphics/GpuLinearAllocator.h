#pragma once

#include "GpuResource.h"

namespace Dash
{
	// Constant blocks must be multiples of 16 constants @ 16 bytes each
	#define DEFAULT_ALIGN 256

	class FGpuLinearAllocator
	{
	public:
		enum AllocatorType
		{
			Invalid = -1,
			GpuExclusive = 0,	// DEFAULT GPU - writable (via UAV)
			CpuExclusive = 1,	// UPLOAD CPU - writable (but write combined)

			NumAllocatorTypes
		};

		enum
		{
			GpuDefaultPageSize = 0x10000,	// 64K
			CpuDefaultPageSize = 0x200000	// 2MB
		};

		struct FAllocation
		{
			FAllocation(FGpuResource& resource, size_t thisOffset, size_t thisSize)
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
		class FPage : public FGpuResource
		{
		public:
			FPage(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES defaultState, size_t pageSize);

			// Check to see if the page has room to satisfy the requested
			// allocation.
			bool HasSpace(size_t sizeInBytes, size_t alignment) const;

			FAllocation Allocate(size_t sizeInBytes, size_t alignment);

			void Reset();

			~FPage()
			{
				mResource->Unmap(0, nullptr);
				mCpuAddress = nullptr;
			}

		private:
			void* mCpuAddress = nullptr;

			size_t mPageSie = 0;
			size_t mOffset = 0;
		};

		class FPageManager
		{
		public:
			FPageManager();

			FPage* RequestPage();
			FPage* RequestLargePage(size_t pageSize = 0);

			FPage* CreateNewPage(size_t pageSize = 0);

			// Discarded pages will get recycled.
			void DiscardPages(uint64_t fenceID, const std::vector<FPage*>& pages, bool isLargePage = false);

			void Destroy();

		private:
			static AllocatorType AutoAllocatorType;

			AllocatorType mAllocatorType;
			std::vector<std::unique_ptr<FPage>> mPagePool;
			std::vector<std::unique_ptr<FPage>> mLargePagePool;
			std::queue<std::pair<uint64_t, FPage*>> mRetiredPages;
			std::queue<std::pair<uint64_t, FPage*>> mRetiredLargePages; //DeletionQueue
			std::queue<FPage*> mAvailablePages;
			std::mutex mMutex;
		};

	public:
		
		FGpuLinearAllocator(AllocatorType type) 
		: mAllocatorType(type)
		, mDefaultPageSize(0)
		, mCurrentPage(nullptr)
		{
			ASSERT(type > AllocatorType::Invalid && type < AllocatorType::NumAllocatorTypes);
			mDefaultPageSize = type == AllocatorType::GpuExclusive ? GpuDefaultPageSize : CpuDefaultPageSize;
		}

		FAllocation Allocate(size_t sizeInBytes, size_t alignment = DEFAULT_ALIGN);

		void RetireUsedPages(uint64_t fenceValue);

		static void Destroy();

	private:
		AllocatorType mAllocatorType;
		size_t mDefaultPageSize;

		FPage* mCurrentPage = nullptr;
		std::vector<FPage*> mRetiredPages;	
		std::vector<FPage*> mRetiredLargePages;

		static FPageManager AllocatorPageManger[2];
	};
}