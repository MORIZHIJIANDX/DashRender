#include "PCH.h"
#include "CpuDescriptorAllocation.h"
#include "CpuDescriptorAllocatorPage.h"

namespace Dash
{
	FCpuDescriptorAllocation::FCpuDescriptorAllocation()
		: mDescriptorHandle{0}
		, mNumDescriptors(0)
		, mDescriptorSize(0)
	{
	}

	FCpuDescriptorAllocation::FCpuDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32 numDecriptors, uint32 descriptorSize, std::shared_ptr<FCpuDescriptorAllocatorPage> page)
		: mDescriptorHandle(descriptor)
		, mNumDescriptors(numDecriptors)
		, mDescriptorSize(descriptorSize)
		, mPage(page)
	{
	}

	FCpuDescriptorAllocation::~FCpuDescriptorAllocation()
	{
		Free();
	}

	FCpuDescriptorAllocation::FCpuDescriptorAllocation(FCpuDescriptorAllocation&& allocation) noexcept
		: mDescriptorHandle(allocation.mDescriptorHandle)
		, mNumDescriptors(allocation.mNumDescriptors)
		, mDescriptorSize(allocation.mDescriptorSize)
		, mPage(std::move(allocation.mPage))
	{
		allocation.mDescriptorHandle.ptr = 0;
		allocation.mNumDescriptors = 0;
		allocation.mDescriptorSize = 0;
	}

	FCpuDescriptorAllocation& FCpuDescriptorAllocation::operator=(FCpuDescriptorAllocation&& other) noexcept
	{
		Free();

		mDescriptorHandle = other.mDescriptorHandle;
		mNumDescriptors = other.mNumDescriptors;
		mDescriptorSize = other.mDescriptorSize;
		mPage = std::move(other.mPage);

		other.mDescriptorHandle.ptr = 0;
		other.mNumDescriptors = 0;
		other.mDescriptorSize = 0;

		return *this;
	}

	bool FCpuDescriptorAllocation::IsNull() const
	{
		return mNumDescriptors == 0 || mDescriptorHandle.ptr == 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FCpuDescriptorAllocation::GetDescriptorHandle(uint32 offset) const
	{
		ASSERT(offset < mNumDescriptors);
		return D3D12_CPU_DESCRIPTOR_HANDLE{mDescriptorHandle.ptr + (static_cast<size_t>(mDescriptorSize) * static_cast<size_t>(offset))};
	}

	uint32 FCpuDescriptorAllocation::GetNumDescriptors() const
	{
		return mNumDescriptors;
	}

	D3D12_DESCRIPTOR_HEAP_TYPE FCpuDescriptorAllocation::GetDescriptorType() const
	{
		if (mPage)
		{	
			return mPage->GetHeapType();
		}

		return D3D12_DESCRIPTOR_HEAP_TYPE(-1);
	}

	void FCpuDescriptorAllocation::Free()
	{
		if (IsValid() && mPage)
		{
			mPage->Free(std::move(*this));

			mDescriptorHandle.ptr = 0;
			mNumDescriptors = 0;
			mDescriptorSize = 0;
			mPage.reset();
		}
	}



}