#include "PCH.h"
#include "PixelBuffer.h"
#include "d3dx12.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"

namespace Dash
{	
	void FPixelBuffer::AssociateWithResource(ID3D12Resource* resource, EResourceState currentState, const std::string& name)
	{
		ASSERT(resource != nullptr);
		
		Destroy();

		mResource.Attach(resource);

		D3D12_RESOURCE_DESC desc = resource->GetDesc();
		
		mWidth = static_cast<uint32_t>(desc.Width);
		mHeight = desc.Height;
		mArraySize = desc.DepthOrArraySize;
		mFormat = ResourceFormatFromD3DFormat(desc.Format);

		SetName(name);

		FGpuResourcesStateTracker::AddGlobalResourceState(*this, D3DResourceState(currentState));	
	}

	void FPixelBuffer::CreateTextureResource(const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue, const std::string& name)
	{
		Destroy();

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
		DX_CALL(FGraphicsCore::Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(&mResource)));

		SetName(name);

		FGpuResourcesStateTracker::AddGlobalResourceState(*this, D3D12_RESOURCE_STATE_COMMON);

		mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

		mWidth = static_cast<uint32_t>(resourceDesc.Width);
		mHeight = resourceDesc.Height;
		mArraySize = resourceDesc.DepthOrArraySize;
		mFormat = ResourceFormatFromD3DFormat(resourceDesc.Format);
	}
}