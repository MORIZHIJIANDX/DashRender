#include "PCH.h"
#include "CpuDescriptorAllocation.h"
#include "CpuDescriptorAllocatorPage.h"

namespace Dash
{
	CpuDescriptorAllocation::CpuDescriptorAllocation()
		: mDescriptorHandle{0}
		, mNumDescriptors(0)
		, mDescriptorSize(0)
	{
	}

	CpuDescriptorAllocation::CpuDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t numDecriptors, uint32_t descriptorSize, std::shared_ptr<CpuDescriptorAllocatorPage> page)
		: mDescriptorHandle(descriptor)
		, mNumDescriptors(numDecriptors)
		, mDescriptorSize(descriptorSize)
		, mPage(page)
	{
	}

	CpuDescriptorAllocation::~CpuDescriptorAllocation()
	{
		Free();
	}

	CpuDescriptorAllocation::CpuDescriptorAllocation(CpuDescriptorAllocation&& allocation) noexcept
		: mDescriptorHandle(allocation.mDescriptorHandle)
		, mNumDescriptors(allocation.mNumDescriptors)
		, mDescriptorSize(allocation.mDescriptorSize)
		, mPage(std::move(allocation.mPage))
	{
		allocation.mDescriptorHandle.ptr = 0;
		allocation.mNumDescriptors = 0;
		allocation.mDescriptorSize = 0;
	}

	CpuDescriptorAllocation& CpuDescriptorAllocation::operator=(CpuDescriptorAllocation&& other) noexcept
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

	bool CpuDescriptorAllocation::IsNull() const
	{
		return mNumDescriptors == 0 || mDescriptorHandle.ptr == 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CpuDescriptorAllocation::GetDescriptorHandle(uint32_t offset) const
	{
		ASSERT(offset < mNumDescriptors);
		return D3D12_CPU_DESCRIPTOR_HANDLE{mDescriptorHandle.ptr + mDescriptorSize * offset};
	}

	uint32_t CpuDescriptorAllocation::GetNumDescriptors() const
	{
		return mNumDescriptors;
	}

	D3D12_DESCRIPTOR_HEAP_TYPE CpuDescriptorAllocation::GetDescriptorType() const
	{
		if (mPage)
		{	
			return mPage->GetHeapType();
		}

		return D3D12_DESCRIPTOR_HEAP_TYPE(-1);
	}

	void CpuDescriptorAllocation::Free()
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