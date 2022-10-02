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

		D3D12_RESOURCE_DESC DescribeTexture2D(uint32_t width, uint32_t height, uint32_t depthOrArraySize, uint32_t numMips, DXGI_FORMAT format, UINT flag);

		void AssociateWithResource(ID3D12Resource* resource, const D3D12_RESOURCE_STATES& currentState, const std::wstring& name = L"");
		void CreateTextureResource(const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue, const std::wstring& name = L"");

		static DXGI_FORMAT GetBaseFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetUAVFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDepthFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetStencilFormat(DXGI_FORMAT format);
		static size_t BytesPerPixel(DXGI_FORMAT format);

	protected:

		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mArraySize;
		DXGI_FORMAT mFormat;
	};
}