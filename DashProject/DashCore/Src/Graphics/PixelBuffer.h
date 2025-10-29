#pragma once

#include "GpuResource.h"

namespace Dash
{
	class FPixelBuffer : public FGpuResource
	{
		friend class FRenderDevice;
	public:
		virtual ~FPixelBuffer() {}

		virtual uint32 GetWidth() const = 0;
		virtual uint32 GetHeight() const = 0;
		virtual uint32 GetDepth() const = 0;
		virtual const EResourceFormat& GetFormat() const = 0;

	protected:
		FPixelBuffer() {}

		void AssociateWithResource(const TRefCountPtr<FD3D12Resource>& resource, EResourceState currentState, const std::string& name = "");
		void CreateTextureResource(const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue, const std::string& name = "", EResourceState initState = EResourceState::Common);
	};
}