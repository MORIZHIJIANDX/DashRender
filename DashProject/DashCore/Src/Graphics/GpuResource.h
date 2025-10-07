#pragma once

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

		ID3D12Resource* operator->() { return mResource.GetReference(); }
		const ID3D12Resource* operator->() const { return mResource.GetReference(); }

		ID3D12Resource* GetResource() { return mResource.GetReference(); }
		const ID3D12Resource* GetResource() const { return mResource.GetReference(); }

		TRefCountPtr<ID3D12Resource> GetResourceComPtr() const { return mResource; }

		//ID3D12Resource** GetAddressOf() { return mResource.Ge(); }

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress();

		uint32 GetVersionID() const { return mVersionID; }

		void SetName(const std::string& name) 
		{  
			mResourceName = name;
			SetD3D12DebugName(mResource.GetReference(), name);
		}

		const std::string& GetName() const { return mResourceName; }

	protected:
		FGpuResource()
		{
		}

		FGpuResource(TRefCountPtr<ID3D12Resource> resource)
			: mResource(resource)
		{
			if (mResource)
			{
				mGpuVirtualAddress = mResource->GetGPUVirtualAddress();
			}
		}

	protected:

		TRefCountPtr<ID3D12Resource> mResource = nullptr;
		D3D12_GPU_VIRTUAL_ADDRESS mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

		std::string mResourceName;
		uint32 mVersionID = 0;
	};
}