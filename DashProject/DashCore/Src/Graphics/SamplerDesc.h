#pragma once

namespace Dash
{
	enum class ESamplerFilter
	{
		Point, Linear, Anisotropic,
	};

	enum class ESamplerAddressMode
	{
		Wrap, Mirror, Clamp, Border, MirrorOnce,
	};

	enum class ESamplerComparisonFunc
	{
		Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always,
	};

	class FSamplerDesc
	{
	public:
		FSamplerDesc();

		FSamplerDesc(ESamplerFilter filter, ESamplerAddressMode addressMode, ESamplerComparisonFunc comparisonFunc, const FLinearColor& borderColor = FLinearColor::Black, 
			float lodBias = 0.0f, uint32 maxAnisotropy = 16, float minLod = 0.0f, float maxLod = D3D12_FLOAT32_MAX);

		void SetFilter(ESamplerFilter filter);
		void SetAddressMode(ESamplerAddressMode addressMode);
		void SetComparisonFunc(ESamplerComparisonFunc comparisonFunc);
		void SetBorderColor(FLinearColor color);

		D3D12_CPU_DESCRIPTOR_HANDLE CreateDescriptor();
		const D3D12_SAMPLER_DESC& D3DSamplerDesc() const { return mD3DSampler; }

		static void DestroyAll();

	private:
		D3D12_FILTER GetD3DFilter(ESamplerFilter filter);
		D3D12_TEXTURE_ADDRESS_MODE GetD3DAddressMode(ESamplerAddressMode mode);
		D3D12_COMPARISON_FUNC GetD3DComparisonFunc(ESamplerComparisonFunc func);

	private:
		D3D12_SAMPLER_DESC mD3DSampler{};
	};
}