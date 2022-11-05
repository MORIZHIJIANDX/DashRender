#pragma once

#include "PixelBuffer.h"
#include "CpuDescriptorAllocation.h"
#include "../Math/Color.h"

namespace Dash
{
	class FColorBuffer : public FPixelBuffer
	{
	public:
		FColorBuffer(const FLinearColor& clearColor = FLinearColor{})
			: mClearColor(clearColor)
		{}

		void Create(const std::string& name, ID3D12Resource* resource, D3D12_RESOURCE_STATES initStates = D3D12_RESOURCE_STATE_COMMON);

		void Create(const std::string& name, const D3D12_RESOURCE_DESC& desc, const FLinearColor& clearColor = FLinearColor{});

		void Create(const std::string& name, uint32_t width, uint32_t height, uint32_t numMips, DXGI_FORMAT format);

		void CreateArray(const std::string& name, uint32_t width, uint32_t height, uint32_t arrayCount, DXGI_FORMAT format);

		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(uint32_t mipIndex = 0) const;

		void SetClearColor(const FLinearColor& clearColor);
		FLinearColor GetClearColor() const { return mClearColor; }

		void SetMsaaMode(uint32_t numSamples, uint32_t quality = 0);

		uint32_t GetNumMips() const { return mNumMips; }

	protected:
		
		static inline uint32_t ComputeNumMips(uint32_t width, uint32_t height)
		{	
			uint32_t highBit;
			_BitScanReverse((unsigned long*)&highBit, width | height);
			return highBit + 1;
		}

		D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(const D3D12_RESOURCE_DESC& resourceDesc, UINT mipSlice, UINT arraySlice = 0, UINT planeSlice = 0) const;

		void CreateViews();

		D3D12_RESOURCE_FLAGS CombineResourceFlgs() const;

	protected:

		FLinearColor mClearColor;

		uint32_t mNumMips = 0;
		uint32_t mMsaaNumSmples = 1;
		uint32_t mMsaaQuality = 0;

		FCpuDescriptorAllocation mRenderTargetView;
		FCpuDescriptorAllocation mShaderResourceView;
		FCpuDescriptorAllocation mUnorderedAccessView;
	};
}