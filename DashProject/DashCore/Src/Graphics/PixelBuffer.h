#pragma once

#include "GpuResource.h"

namespace Dash
{
	class FPixelBuffer : public FGpuResource
	{
	public:
		FPixelBuffer()
			: mWidth(0)
			, mHeight(0)
			, mArraySize(0)
			, mFormat(DXGI_FORMAT_UNKNOWN)
		{
		}

		uint32_t GetWidth() const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }
		uint32_t GetDepth() const { return mArraySize; }
		const DXGI_FORMAT& GetFormat() const { return mFormat; }

	protected:

		void AssociateWithResource(ID3D12Resource* resource, const D3D12_RESOURCE_STATES& currentState, const std::wstring& name = L"");
		void CreateTextureResource(const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue, const std::wstring& name = L"");

	protected:

		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mArraySize;
		DXGI_FORMAT mFormat;
	};
}