#include "PCH.h"
#include "GpuResource.h"
#include "GpuResourcesStateTracker.h"

namespace Dash
{
	void FGpuResource::Destroy()
	{
		FGpuResourcesStateTracker::RemoveGlobalResourceState(this->mResource.GetReference());

		mResource = nullptr;

		++mVersionID;
	}

	D3D12_GPU_VIRTUAL_ADDRESS FGpuResource::GetGpuVirtualAddress() const
	{
		return mResource->GetGPUVirtualAddress();
	}
}


