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
			, mFormat()
		{}

		uint32_t GetWidth() const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }
		uint32_t GetDepth() const { return mArraySize; }
		const EResourceFormat& GetFormat() const { return mFormat; }

	protected:

		void AssociateWithResource(ID3D12Resource* resource, EResourceState currentState, const std::string& name = "");
		void CreateTextureResource(const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue, const std::string& name = "");

	protected:

		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mArraySize;
		EResourceFormat mFormat;
	};
}