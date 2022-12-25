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

	void FPixelBuffer::CreateTextureResource(const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue, const std::string& name)
	{
		Destroy();

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
		DX_CALL(FGraphicsCore::Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, &clearValue, mResource));

		SetName(name);

		FGpuResourcesStateTracker::AddGlobalResourceState(this->GetResource(), D3D12_RESOURCE_STATE_COMMON);

		mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
	}
}