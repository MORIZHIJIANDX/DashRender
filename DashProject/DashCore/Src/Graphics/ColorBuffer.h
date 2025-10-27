#pragma once

#include "PixelBuffer.h"
#include "CpuDescriptorAllocation.h"
#include "Math/Color.h"

namespace Dash
{
	class FColorBuffer : public FPixelBuffer
	{
		friend class FRenderDevice;
	public:
		virtual ~FColorBuffer() {}

		virtual uint32 GetWidth() const { return mDesc.Magnitude.Width; }
		virtual uint32 GetHeight() const { return mDesc.Magnitude.Height; }
		virtual uint32 GetDepth() const { return mDesc.Magnitude.Depth; }
		virtual const EResourceFormat& GetFormat() const { return mDesc.Format; }

		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32 mipIndex = 0) const;

		void SetClearColor(const FLinearColor& clearColor);
		FLinearColor GetClearColor() const { return mDesc.ClearValue; }

		void SetMsaaMode(uint32 numSamples, uint32 quality = 0);

		uint32 GetNumMips() const { return mDesc.MipCount; }

		const FColorBufferDescription& GetDesc() const { return mDesc; }

	protected:
		FColorBuffer(const FLinearColor& clearColor = FLinearColor{})
		{
			mDesc.ClearValue = clearColor;
		}

		void Create(const std::string& name, const TRefCountPtr<ID3D12Resource>& resource, EResourceState initStates = EResourceState::Common);
		void Create(const std::string& name, const FColorBufferDescription& desc);
		void Create(const std::string& name, uint32 width, uint32 height, uint32 numMips, EResourceFormat format);
		void CreateArray(const std::string& name, uint32 width, uint32 height, uint32 arrayCount, uint32 numMips, EResourceFormat format);

		void CreateBuffer(const std::string& name);
		
		static inline uint32 ComputeNumMips(uint32 width, uint32 height)
		{	
			uint32 highBit;
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