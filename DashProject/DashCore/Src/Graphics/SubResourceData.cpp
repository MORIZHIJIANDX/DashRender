#include "PCH.h"
#include "SubResourceData.h"

namespace Dash
{
	FSubResourceData::FSubResourceData(const void* data, size_t rowPitch, size_t slicePitch)
	{
		ASSERT(data != nullptr);

		mSubResource.pData = data;
		mSubResource.RowPitch = rowPitch;
		mSubResource.SlicePitch = slicePitch;
	}

	FSubResourceData::~FSubResourceData()
	{
	}
}