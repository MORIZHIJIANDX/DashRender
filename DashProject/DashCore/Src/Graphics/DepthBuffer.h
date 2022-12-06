#pragma once

#include "PixelBuffer.h"
#include "CpuDescriptorAllocation.h"

namespace Dash
{
	class FDepthBuffer : public FPixelBuffer
	{
	public:
		FDepthBuffer(float clearDepth = 0.0f, uint8_t clearStencil = 0)
			: mClearDepth(clearDepth)
			, mClearStencil(clearStencil)
		{}

		void Create(const std::string& name, const D3D12_RESOURCE_DESC& desc, float clearDepth = 0.0f, uint8_t clearStencil = 0);

		void Create(const std::string& name, uint32_t width, uint32_t height, EResourceFormat format);

		void Create(const std::string& name, uint32_t width, uint32_t height, uint32_t sampleCount, EResourceFormat format);

		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;

		float GetClearDepth() const { return mClearDepth; }
		uint8_t GetClearStencil() const { return mClearStencil; }

	protected:
			
		void CreateViews();

	protected:

		float mClearDepth = 0.0f;
		uint8_t mClearStencil = 0;
		
		FCpuDescriptorAllocation mDepthStencilView;
		FCpuDescriptorAllocation mShaderResourceView;
	};
}