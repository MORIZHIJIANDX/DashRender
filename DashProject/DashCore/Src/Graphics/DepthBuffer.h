#pragma once

#include "PixelBuffer.h"
#include "CpuDescriptorAllocation.h"

namespace Dash
{
	class FDepthBuffer : public FPixelBuffer
	{
	public:
		FDepthBuffer(float clearDepth = 0.0f, uint8_t clearStencil = 0)
		{
			mDesc.ClearValue = FDepthStencilClearValue{ clearDepth, clearStencil};
		}
		virtual ~FDepthBuffer(){}

		virtual uint32_t GetWidth() const { return mDesc.Magnitude.Width; }
		virtual uint32_t GetHeight() const { return mDesc.Magnitude.Height; }
		virtual uint32_t GetDepth() const { return mDesc.Magnitude.Depth; }
		virtual const EResourceFormat& GetFormat() const { return mDesc.Format; }

		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;

		float GetClearDepth() const { return mDesc.ClearValue.Depth; }
		uint8_t GetClearStencil() const { return mDesc.ClearValue.Stencil; }

		const FDepthBufferDescription& GetDesc() const { return mDesc; }

	protected:
		void Create(const std::string& name, const FDepthBufferDescription& desc);
		void Create(const std::string& name, uint32_t width, uint32_t height, EResourceFormat format);
		void Create(const std::string& name, uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, EResourceFormat format);
		
		void CreateBuffer(const std::string& name);

		void CreateViews();

		D3D12_CLEAR_VALUE GetD3DClearValue() const;

	protected:
		FDepthBufferDescription mDesc;

		FCpuDescriptorAllocation mDepthStencilView;
		FCpuDescriptorAllocation mShaderResourceView;
	};
}