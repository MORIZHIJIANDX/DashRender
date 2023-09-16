#pragma once

#include "d3dx12.h"

namespace Dash
{
	class FSubResourceData
	{
	public:
		FSubResourceData(const void* data, size_t rowPitch, size_t slicePitch);
		~FSubResourceData();

		const size_t GetRowPitch() const { return mSubResource.RowPitch; }
		const size_t GetSlicePitch() const { return mSubResource.SlicePitch; }
		const D3D12_SUBRESOURCE_DATA& D3DSubResource() const { return mSubResource; }

	private:
		D3D12_SUBRESOURCE_DATA mSubResource;
	};
}