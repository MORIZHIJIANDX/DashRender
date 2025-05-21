#include "PCH.h"
#include "CpuDescriptorAllocator.h"
#include "CpuDescriptorAllocatorPage.h"

namespace Dash
{

	FCpuDescriptorAllocator::FCpuDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 defaultHeapSize /*= 256*/)
	: mType(type)
	, mDefaultHeapSize(defaultHeapSize)
	{	
	}

	FCpuDescriptorAllocator::~FCpuDescriptorAllocator()
	{
		Destroy();
	}

	FCpuDescriptorAllocation FCpuDescriptorAllocator::Allocate(uint32 numDescriptors)
	{
		std::lock_guard lock(mAllocationMutex);

		FCpuDescriptorAllocation allocation;

		auto iter = mAvailableHeaps.begin();
		while (iter != mAvailableHeaps.end())
		{
			std::shared_ptr<FCpuDescriptorAllocatorPage> page = mHeapPool[*iter];
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
			std::shared_ptr<FCpuDescriptorAllocatorPage> page = RequestNewHeap(FMath::Max(numDescriptors, mDefaultHeapSize));
			allocation = page->Allocate(numDescriptors);
		}

		return allocation;
	}

	void FCpuDescriptorAllocator::ReleaseStaleDescriptors()
	{
		std::lock_guard lock(mAllocationMutex);

		for (int32 Index = 0; Index < mHeapPool.size(); ++Index)
		{
			mHeapPool[Index]->ReleaseStaleDescriptors();

			if (mHeapPool[Index]->NumFreeHandles() > 0)
			{
				mAvailableHeaps.insert(Index);
			}
		}
	}

	void FCpuDescriptorAllocator::Destroy()
	{
		std::lock_guard lock(mAllocationMutex);

		mAvailableHeaps.clear();
		mHeapPool.clear();
	}

	std::shared_ptr<FCpuDescriptorAllocatorPage> FCpuDescriptorAllocator::RequestNewHeap(uint32 heapSize /*= 0*/)
	{
		std::shared_ptr<FCpuDescriptorAllocatorPage> page = std::make_shared<FCpuDescriptorAllocatorPage>(mType, heapSize);

		mHeapPool.push_back(page);
		mAvailableHeaps.insert(static_cast<uint32>(mHeapPool.size()) - 1);

		return page;
	}

	FCpuDescriptorAllocatorManager::FCpuDescriptorAllocatorManager()
		: mCbvSrvUavAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		, mRTVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		, mDSVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		, mSamplerAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	{
	}

	FCpuDescriptorAllocation FCpuDescriptorAllocatorManager::Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 numDescriptors)
	{
		if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		{
			return mCbvSrvUavAllocator.Allocate(numDescriptors);
		}
		else
		{
			return mSamplerAllocator.Allocate(numDescriptors);
		}
	}

	FCpuDescriptorAllocation FCpuDescriptorAllocatorManager::AllocateCBVDescriptor(uint32 numDescriptors)
	{
		return mCbvSrvUavAllocator.Allocate(numDescriptors);
	}

	FCpuDescriptorAllocation FCpuDescriptorAllocatorManager::AllocateSRVDescriptor(uint32 numDescriptors)
	{
		return mCbvSrvUavAllocator.Allocate(numDescriptors);
	}

	FCpuDescriptorAllocation FCpuDescriptorAllocatorManager::AllocateUAVDescriptor(uint32 numDescriptors)
	{
		return mCbvSrvUavAllocator.Allocate(numDescriptors);
	}

	FCpuDescriptorAllocation FCpuDescriptorAllocatorManager::AllocateRTVDescriptor(uint32 numDescriptors)
	{
		return mRTVAllocator.Allocate(numDescriptors);
	}

	FCpuDescriptorAllocation FCpuDescriptorAllocatorManager::AllocateDSVDescriptor(uint32 numDescriptors)
	{
		return mDSVAllocator.Allocate(numDescriptors);
	}

	FCpuDescriptorAllocation FCpuDescriptorAllocatorManager::AllocateSamplerDescriptor(uint32 numDescriptors)
	{
		return mSamplerAllocator.Allocate(numDescriptors);
	}

	void FCpuDescriptorAllocatorManager::ReleaseStaleDescriptors()
	{
		mCbvSrvUavAllocator.ReleaseStaleDescriptors();
		mRTVAllocator.ReleaseStaleDescriptors();
		mDSVAllocator.ReleaseStaleDescriptors();
		mSamplerAllocator.ReleaseStaleDescriptors();
	}

	void FCpuDescriptorAllocatorManager::Destroy()
	{
		mCbvSrvUavAllocator.Destroy();
		mRTVAllocator.Destroy();
		mDSVAllocator.Destroy();
		mSamplerAllocator.Destroy();
	}

}