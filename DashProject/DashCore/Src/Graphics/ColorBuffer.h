#pragma once

#include "PixelBuffer.h"
#include "CpuDescriptorAllocation.h"
#include "Math/Color.h"

namespace Dash
{
	//class FColorBuffer;
	//using FColorBufferRef = std::shared_ptr<FColorBuffer>;

	class FColorBuffer : public FPixelBuffer
	{
	public:
		FColorBuffer(const FLinearColor& clearColor = FLinearColor{})
		{
			mDesc.ClearValue = clearColor;
		}

		virtual ~FColorBuffer() {}

		virtual uint32_t GetWidth() const { return mDesc.Magnitude.Width; }
		virtual uint32_t GetHeight() const { return mDesc.Magnitude.Height; }
		virtual uint32_t GetDepth() const { return mDesc.Magnitude.Depth; }
		virtual const EResourceFormat& GetFormat() const { return mDesc.Format; }

		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32_t mipIndex = 0) const;

		void SetClearColor(const FLinearColor& clearColor);
		FLinearColor GetClearColor() const { return mDesc.ClearValue; }

		void SetMsaaMode(uint32_t numSamples, uint32_t quality = 0);

		uint32_t GetNumMips() const { return mDesc.MipCount; }

		const FColorBufferDescription& GetDesc() const { return mDesc; }

	protected:
		void Create(const std::string& name, ID3D12Resource* resource, EResourceState initStates = EResourceState::Common);
		void Create(const std::string& name, const FColorBufferDescription& desc, const FLinearColor& clearColor = FLinearColor{});
		void Create(const std::string& name, uint32_t width, uint32_t height, uint32_t numMips, EResourceFormat format);
		void CreateArray(const std::string& name, uint32_t width, uint32_t height, uint32_t arrayCount, uint32_t numMips, EResourceFormat format);
		
		static inline uint32_t ComputeNumMips(uint32_t width, uint32_t height)
		{	
			uint32_t highBit;
			_BitScanReverse((unsigned long*)&highBit, width | height);
			return highBit + 1;
		}

		D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(const D3D12_RESOURCE_DESC& resourceDesc, UINT mipSlice, UINT arraySlice = 0, UINT planeSlice = 0) const;

		void CreateViews();

		D3D12_CLEAR_VALUE GetD3DClearValue() const;

	protected:

		FColorBufferDescription mDesc;

		FCpuDescriptorAllocation mRenderTargetView;
		FCpuDescriptorAllocation mShaderResourceView;
		FCpuDescriptorAllocation mUnorderedAccessView;
	};
}