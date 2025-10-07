#include "PCH.h"
#include "GpuResource.h"
#include "GpuResourcesStateTracker.h"

namespace Dash
{
	void FGpuResource::Destroy()
	{
		FGpuResourcesStateTracker::RemoveGlobalResourceState(this->mResource.GetReference());

		mResource = nullptr;
		mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

		++mVersionID;
	}

	D3D12_GPU_VIRTUAL_ADDRESS Dash::FGpuResource::GetGpuVirtualAddress()
	{
		return mGpuVirtualAddress;
	}
}


