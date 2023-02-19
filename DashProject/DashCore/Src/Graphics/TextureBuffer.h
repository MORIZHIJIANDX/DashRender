#pragma once

#include "PixelBuffer.h"
#include "CpuDescriptorAllocation.h"

namespace Dash
{
	class FTextureBuffer : public FPixelBuffer
	{
	public:
		virtual ~FTextureBuffer() {};

		virtual uint32_t GetWidth() const { return mDesc.Magnitude.Width; }
		virtual uint32_t GetHeight() const { return mDesc.Magnitude.Height; }
		virtual uint32_t GetDepth() const { return mDesc.Magnitude.Depth; }
		virtual const EResourceFormat& GetFormat() const { return mDesc.Format; }

		uint32_t GetNumMips() const { return mDesc.MipCount; }

		const FTextureBufferDescription& GetDesc() const { return mDesc; }

		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;

	protected:
		void Create(const std::string& name, const FTextureBufferDescription& desc);
		void Create(const std::string& name, uint32_t width, uint32_t height, uint32_t numMips, EResourceFormat format);
		void Create(const std::string& name, uint32_t width, uint32_t height, uint32_t arrayCount, uint32_t numMips, EResourceFormat format);

		void CreateBuffer(const std::string& name);

		void CreateViews();

	protected:
		FTextureBufferDescription mDesc;

		FCpuDescriptorAllocation mShaderResourceView;
	};
	
}