#include "PCH.h"
#include "D3D12Resource.h"
#include "DX12Helper.h"
#include "ResourceFormat.h"

namespace Dash
{
	FD3D12Heap::~FD3D12Heap()
	{
	}

	void FD3D12Heap::SetHeap(ID3D12Heap* heap, const std::string& name)
	{
		*mHeap.GetInitReference() = heap;
		mHeapName = name;
		mHeapDesc = mHeap->GetDesc();
		SetD3D12DebugName(mHeap.GetReference(), name);
	}

	FD3D12Resource::FD3D12Resource(const TRefCountPtr<ID3D12Resource>& resource, const D3D12_RESOURCE_DESC& desc, FD3D12Heap* heap, D3D12_HEAP_TYPE heapType)
		: mResource(resource)
		, mResourceDesc(desc)
		, mHeap(heap)
		, mHeapType(heapType)
		, mGPUVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL)
		, mMappedAddress(nullptr)
		, mPlaneCount(GetFormatPlaneCount(desc.Format))
		, mSubresourceCount(0)
		, mIsDepthStencil(IsDepthStencilFormat(desc.Format))
	{
		mSubresourceCount = GetMipLevels() * GetArraySize() * GetPlaneCount();
		if (mResource && mResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER)
		{
			mGPUVirtualAddress = mResource->GetGPUVirtualAddress();
		}
	}

	FD3D12Resource::~FD3D12Resource()
	{
	}

	void* FD3D12Resource::Map(const D3D12_RANGE* readRange)
	{
		if (mNumMapCalls == 0)
		{
			ASSERT(mResource);
			ASSERT(mMappedAddress == nullptr);
			DX_CALL(mResource->Map(0, readRange, &mMappedAddress));
		}
		else
		{
			ASSERT(mResource);
			ASSERT(mMappedAddress != nullptr);
		}
		++mNumMapCalls;

		return mMappedAddress;
	}

	void FD3D12Resource::Unmap()
	{
		ASSERT(mResource);
		ASSERT(mMappedAddress != nullptr);
		ASSERT(mNumMapCalls > 0);

		--mNumMapCalls;
		if (mNumMapCalls == 0)
		{
			mResource->Unmap(0, nullptr);
			mMappedAddress = nullptr;
		}
	}

	ID3D12Pageable* FD3D12Resource::GetPageable()
	{
		if (IsPlacedResource())
		{
			return mHeap->GetHeap();
		}
		else
		{
			return mResource;
		}
	}

	void FD3D12Resource::SetName(const std::string_view& name)
	{
		mResourceName = name;
		SetD3D12DebugName(mResource.GetReference(), mResourceName);
	}
}