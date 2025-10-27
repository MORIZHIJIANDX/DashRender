#include "PCH.h"
#include "PixelBuffer.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"
#include "GpuResourcesStateTracker.h"
#include "RenderDevice.h"

namespace Dash
{	
	void FPixelBuffer::AssociateWithResource(const TRefCountPtr<FD3D12Resource>& resource, EResourceState currentState, const std::string& name)
	{
		ASSERT(resource != nullptr);
		
		Destroy();

		mResource = resource;

		SetName(name);

		FGpuResourcesStateTracker::AddGlobalResourceState(this->GetResource(), D3DResourceState(currentState));	
	}

	void FPixelBuffer::CreateTextureResource(const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue, const std::string& name, EResourceState initState)
	{
		Destroy();

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_STATES initD3DState = D3DResourceState(initState);
		const D3D12_CLEAR_VALUE* clearValuePtr = resourceDesc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) ? &clearValue : nullptr;

		TRefCountPtr<ID3D12Resource> d3d12Resource = FGraphicsCore::Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, initD3DState, clearValuePtr);
		mResource = MakeRefCounted<FD3D12Resource>(d3d12Resource, resourceDesc);

		SetName(name);

		FGpuResourcesStateTracker::AddGlobalResourceState(this->GetResource(), initD3DState);
	}
}