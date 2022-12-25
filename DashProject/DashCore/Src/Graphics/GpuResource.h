#pragma once
#include <wrl.h>
#include "DX12Helper.h"
#include "ResourceFormat.h"
#include "ResourceDescription.h"

namespace Dash
{
	class FGpuResource;
	using FGpuResourceRef = std::shared_ptr<FGpuResource>;

	class FGpuResource
	{
	public:
		virtual ~FGpuResource()
		{
			Destroy();
		}

		virtual void Destroy();

		ID3D12Resource* operator->() { return mResource.Get(); }
		const ID3D12Resource* operator->() const { return mResource.Get(); }

		ID3D12Resource* GetResource() { return mResource.Get(); }
		const ID3D12Resource* GetResource() const { return mResource.Get(); }

		Microsoft::WRL::ComPtr<ID3D12Resource> GetResourceComPtr() const { return mResource; }

		ID3D12Resource** GetAddressOf() { return mResource.GetAddressOf(); }

		//D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() { return mGpuVirtualAddress; };

		uint32_t GetVersionID() const { return mVersionID; }

		void SetName(const std::string& name) 
		{  
			mResourceName = name;
			SetD3D12DebugName(mResource.Get(), name);
		}

		const std::string& GetName() const { return mResourceName; }

	protected:
		FGpuResource()
		{
		}

		FGpuResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource)
			: mResource(resource)
		{
		}

		Microsoft::WRL::ComPtr<ID3D12Resource> mResource = nullptr;
		D3D12_GPU_VIRTUAL_ADDRESS mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

		std::string mResourceName;
		uint32_t mVersionID = 0;
	};
}