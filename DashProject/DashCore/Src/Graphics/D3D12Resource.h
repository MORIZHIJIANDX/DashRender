#pragma once
#include "Utility/ThreadSafeCounter.h"
#include "Utility/RefCounting.h"

namespace Dash
{
	class FD3D12Heap : public FRefCount
	{
	public:
		FD3D12Heap(){}
		virtual ~FD3D12Heap();

		ID3D12Heap* GetHeap() const { return mHeap.GetReference(); }
		void SetHeap(ID3D12Heap* heap, const std::string& name);

		std::string GetName() const { return mHeapName; }
		const D3D12_HEAP_DESC& GetHeapDesc() const { return mHeapDesc; }

	private:
		TRefCountPtr<ID3D12Heap> mHeap;
		D3D12_HEAP_DESC mHeapDesc{};
		std::string mHeapName;
	};

	class FD3D12Resource : public FRefCount
	{
	public:
		FD3D12Resource(const TRefCountPtr<ID3D12Resource>& resource, const D3D12_RESOURCE_DESC& desc, FD3D12Heap* heap = nullptr, D3D12_HEAP_TYPE heaptype = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT);
		virtual ~FD3D12Resource();

		operator ID3D12Resource&() { return *mResource; }
		ID3D12Resource* GetResource() const { return mResource.GetReference(); }

		void* Map(const D3D12_RANGE* readRange = nullptr);
		void Unmap();

		ID3D12Pageable* GetPageable();
		const D3D12_RESOURCE_DESC& GetDesc() const { return mResourceDesc; }
		D3D12_HEAP_TYPE GetHeapType() const { return mHeapType; }
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return mGPUVirtualAddress; }
		void SetGPUVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS address) { mGPUVirtualAddress = address; }
		void* GetMappedAddress() const { return mMappedAddress; }

		uint16 GetMipLevels() const { return mResourceDesc.MipLevels; }
		uint16 GetArraySize() const { return (mResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D) ? 1 : mResourceDesc.DepthOrArraySize; }
		uint8 GetPlaneCount() const { return mPlaneCount; }
		uint16 GetSubresourceCount() const { return mSubresourceCount; }

		void SetName(const std::string_view& name);
		std::string GetName() const { return mResourcName; }

		bool IsPlacedResource() const { return mHeap.GetReference() != nullptr; }
		FD3D12Heap* GetHeap() const { return mHeap; };

		bool IsDepthStencilResource() const { return mIsDepthStencil; }

	private:
		TRefCountPtr<ID3D12Resource> mResource;
		D3D12_RESOURCE_DESC mResourceDesc;
		TRefCountPtr<FD3D12Heap> mHeap;
		D3D12_HEAP_TYPE mHeapType;
	
		D3D12_GPU_VIRTUAL_ADDRESS mGPUVirtualAddress;
		void* mMappedAddress;

		std::string mResourceName;
		int32 mNumMapCalls = 0;

		uint8 mPlaneCount;
		uint16 mSubresourceCount;
		bool mIsDepthStencil;

		std::string mResourcName;
	};
}