#pragma once

#include "DX12Helper.h"
#include "GraphicTypesFwd.h"
#include "ResourceFormat.h"
#include "ResourceDescription.h"
#include "D3D12Resource.h"
#include "Utility/ThreadSafeCounter.h"

namespace Dash
{
	class FGpuResource : public FRefCount
	{
		friend class FRenderDevice;
	public:
		virtual ~FGpuResource()
		{
			Destroy();
		}

		virtual void Destroy();

		FD3D12Resource* operator->() { return mResource.GetReference(); }
		const FD3D12Resource* operator->() const { return mResource.GetReference(); }

		FD3D12Resource* GetResource() { return mResource.GetReference(); }
		const FD3D12Resource* GetResource() const { return mResource.GetReference(); }

		TRefCountPtr<FD3D12Resource> GetResourceRefPtr() const { return mResource; }

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const;

		uint32 GetVersionID() const { return mVersionID; }

		void SetName(const std::string& name) 
		{  
			mResourceName = name;
			mResource->SetName(name);
		}

		const std::string& GetName() const { return mResourceName; }

	protected:
		FGpuResource()
		{
		}

		FGpuResource(const TRefCountPtr<FD3D12Resource>& resource)
			: mResource(resource)
		{
		}

	protected:
		TRefCountPtr<FD3D12Resource> mResource = nullptr;

		std::string mResourceName;
		uint32 mVersionID = 0;
	};
}