#include "PCH.h"
#include "PixelBuffer.h"
#include "d3dx12.h"
#include "GraphicsCore.h"
#include "../Utility/Exception.h"

namespace Dash
{
	void FPixelBuffer::AssociateWithResource(ID3D12Resource* resource, const D3D12_RESOURCE_STATES& currentState, const std::wstring& name)
	{
		ASSERT(resource != nullptr);

		mResource.Attach(resource);

		D3D12_RESOURCE_DESC desc = resource->GetDesc();
		
		mWidth = desc.Width;
		mHeight = desc.Height;
		mArraySize = desc.DepthOrArraySize;
		mFormat = desc.Format;

		FGpuResourcesStateTracker::AddGlobalResourceState(*this, currentState);

#ifdef DASH_DEBUG
		if (!name.empty())
		{
			mResource->SetName(name.c_str());
		}	
#endif // DASH_DEBUG
	}

	void FPixelBuffer::CreateTextureResource(const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue, const std::wstring& name)
	{
		Destroy();

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
		DX_CALL(FGraphicsCore::Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(&mResource)));

		FGpuResourcesStateTracker::AddGlobalResourceState(*this, D3D12_RESOURCE_STATE_COMMON);

		mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

#ifdef DASH_DEBUG
		if (!name.empty())
		{
			mResource->SetName(name.c_str());
		}
#endif // DASH_DEBUG
	}
}