#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "GpuResourcesStateTracker.h"

namespace Dash
{
	class FGpuResource
	{
	public:
		~FGpuResource()
		{
			Destroy();
		}

		virtual void Destroy()
		{
			FGpuResourcesStateTracker::RemoveGlobalResourceState(*this);

			mResource = nullptr;
			mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			++mVersionID;
		}

		ID3D12Resource* operator->() { return mResource.Get(); }
		const ID3D12Resource* operator->() const { return mResource.Get(); }

		ID3D12Resource* GetResource() { return mResource.Get(); }
		const ID3D12Resource* GetResource() const { return mResource.Get(); }

		Microsoft::WRL::ComPtr<ID3D12Resource> GetResourceComPtr() const { return mResource; }

		ID3D12Resource** GetAddressOf() { return mResource.GetAddressOf(); }

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() { return mGpuVirtualAddress; };

		uint32_t GetVersionID() const { return mVersionID; }

		void SetName(const std::wstring& name) 
		{  
			mResourceName = name;
#ifdef DASH_DEBUG
			if (mResource)
			{	
				mResource->SetName(name.c_str());
			}
#endif
		}

		const std::wstring& GetName() const { return mResourceName; }

	protected:
		FGpuResource()
		{
		}

		FGpuResource(ID3D12Resource* resource)
		: mResource(resource)
		{
		}

		Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
		D3D12_GPU_VIRTUAL_ADDRESS mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

		std::wstring mResourceName;
		uint32_t mVersionID = 0;
	};
}