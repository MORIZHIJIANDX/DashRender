#include "PCH.h"
#include "CpuDescriptorAllocator.h"
#include "CpuDescriptorAllocatorPage.h"

namespace Dash
{

	CpuDescriptorAllocator::CpuDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t defaultHeapSize /*= 256*/)
	: mType(type)
	, mDefaultHeapSize(defaultHeapSize)
	{	
	}

	CpuDescriptorAllocator::~CpuDescriptorAllocator()
	{
		Destroy();
	}

	Dash::CpuDescriptorAllocation CpuDescriptorAllocator::Allocate(uint32_t numDescriptors)
	{
		std::lock_guard lock(mAllocationMutex);

		CpuDescriptorAllocation allocation;

		auto iter = mAvailableHeaps.begin();
		while (iter != mAvailableHeaps.end())
		{
			std::shared_ptr<CpuDescriptorAllocatorPage> page = mHeapPool[*iter];
			allocation = page->Allocate(numDescriptors);

			if (allocation.GetNumDescriptors() == 0)
			{
				iter = mAvailableHeaps.erase(iter);
			}
			else
			{
				++iter;
			}

			if (allocation.IsValid())
			{
				break;
			}
		}

		if (allocation.IsNull())
		{
			std::shared_ptr<CpuDescriptorAllocatorPage> page = RequestNewHeap(FMath::Max(numDescriptors, mDefaultHeapSize));
			allocation = page->Allocate(numDescriptors);
		}

		return allocation;
	}

	void CpuDescriptorAllocator::ReleaseStaleDescriptors()
	{
		std::lock_guard lock(mAllocationMutex);

		for (int32_t Index = 0; Index < mHeapPool.size(); ++Index)
		{
			mHeapPool[Index]->ReleaseStaleDescriptors();

			if (mHeapPool[Index]->NumFreeHandles() > 0)
			{
				mAvailableHeaps.insert(Index);
			}
		}
	}

	void CpuDescriptorAllocator::Destroy()
	{
		std::lock_guard lock(mAllocationMutex);

		mAvailableHeaps.clear();
		mHeapPool.clear();
	}

	std::shared_ptr<CpuDescriptorAllocatorPage> CpuDescriptorAllocator::RequestNewHeap(uint32_t heapSize /*= 0*/)
	{
		std::shared_ptr<CpuDescriptorAllocatorPage> page = std::make_shared<CpuDescriptorAllocatorPage>(mType, heapSize);

		mHeapPool.push_back(page);
		mAvailableHeaps.insert(mHeapPool.size() - 1);

		return page;
	}

}