#pragma once

#include "d3dx12.h"

namespace Dash
{
	enum class ERasterizerFillMode
	{
		Wireframe, Solid
	};

	enum class ERasterizerCullMode
	{
		None, Front, Back
	};

	class FRasterizerState
	{
	public:
		FRasterizerState(ERasterizerFillMode fillMode = ERasterizerFillMode::Solid, ERasterizerCullMode cullMode = ERasterizerCullMode::Back, bool frontIsClockwise = true, 
			int depthBias = 0, float depthBiasClamp = 0.0f, float slopeScaledDepthBias = 0.0f, bool depthClipEnable = true, bool multisampleEnable = false, bool antialiasedLineEnable = false);

		void SetFillMode(ERasterizerFillMode fillMode);
		void SetCullMode(ERasterizerCullMode cullMode);
		void SetFrontClockwise(bool frontIsClockwise);
		void SetDepthProperties(int32_t depthBias = 0, float depthBiasClamp = 0.0f, float slopeScaledDepthBias = 0.0f, bool depthClipEnable = true);
		void SetMultisampleEnable(bool enable);
		void SetAntialiasedLineEnable(bool enable);
		void SetForcedSampleCount(UINT count);
		void SetConservativeRasterEnable(bool enable);

		const CD3DX12_RASTERIZER_DESC& D3DRasterizerState() const { return mDesc; };

	private:
		D3D12_FILL_MODE GetD3DFillMode(ERasterizerFillMode fillMode);
		D3D12_CULL_MODE GetD3DCullMode(ERasterizerCullMode cullMode);

	private:
		CD3DX12_RASTERIZER_DESC mDesc{};
	};
}