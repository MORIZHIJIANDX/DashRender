#include "PCH.h"
#include "PixelBuffer.h"
#include "d3dx12.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"
#include "GpuResourcesStateTracker.h"
#include "RenderDevice.h"

namespace Dash
{	
	void FPixelBuffer::AssociateWithResource(ID3D12Resource* resource, EResourceState currentState, const std::string& name)
	{
		ASSERT(resource != nullptr);
		
		Destroy();

		mResource.Attach(resource);

		D3D12_RESOURCE_DESC desc = resource->GetDesc();

		SetName(name);

		FGpuResourcesStateTracker::AddGlobalResourceState(this->GetResource(), D3DResourceState(currentState));	
	}

	void FPixelBuffer::CreateTextureResource(const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue, const std::string& name, EResourceState initState)
	{
		Destroy();

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_STATES initD3DState = D3DResourceState(initState);
		const D3D12_CLEAR_VALUE* clearValuePtr = resourceDesc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) ? &clearValue : nullptr;
		DX_CALL(FGraphicsCore::Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, initD3DState, clearValuePtr, mResource));

		SetName(name);

		FGpuResourcesStateTracker::AddGlobalResourceState(this->GetResource(), initD3DState);

		mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
	}
}