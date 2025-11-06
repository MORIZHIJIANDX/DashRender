#include "PCH.h"
#include "RasterizerState.h"

namespace Dash
{
	FRasterizerState::FRasterizerState(ERasterizerFillMode fillMode, ERasterizerCullMode cullMode, bool frontIsClockwise, int depthBias, float depthBiasClamp,
		float slopeScaledDepthBias, bool depthClipEnable, bool multisampleEnable, bool antialiasedLineEnable)
	{
		mDesc.FillMode = GetD3DFillMode(fillMode);
		mDesc.CullMode = GetD3DCullMode(cullMode);
		mDesc.FrontCounterClockwise = !frontIsClockwise;
		mDesc.DepthBias = depthBias;
		mDesc.DepthBiasClamp = depthBiasClamp;
		mDesc.SlopeScaledDepthBias = slopeScaledDepthBias;
		mDesc.DepthClipEnable = depthClipEnable;
		mDesc.MultisampleEnable = multisampleEnable;
		mDesc.AntialiasedLineEnable = antialiasedLineEnable;
		mDesc.ForcedSampleCount = 0;
		mDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}

	void FRasterizerState::SetFillMode(ERasterizerFillMode fillMode)
	{
		mDesc.FillMode = GetD3DFillMode(fillMode);
	}

	void FRasterizerState::SetCullMode(ERasterizerCullMode cullMode)
	{
		mDesc.CullMode = GetD3DCullMode(cullMode);
	}

	void FRasterizerState::SetFrontClockwise(bool frontIsClockwise)
	{
		mDesc.FrontCounterClockwise = frontIsClockwise;
	}

	void FRasterizerState::SetDepthProperties(int32 depthBias, float depthBiasClamp, float slopeScaledDepthBias, bool depthClipEnable)
	{
		mDesc.DepthBias = depthBias;
		mDesc.DepthBiasClamp = depthBiasClamp;
		mDesc.SlopeScaledDepthBias = slopeScaledDepthBias;
		mDesc.DepthClipEnable = depthClipEnable;
	}

	void FRasterizerState::SetMultisampleEnable(bool enable)
	{
		mDesc.MultisampleEnable = enable;
	}

	void FRasterizerState::SetAntialiasedLineEnable(bool enable)
	{
		mDesc.AntialiasedLineEnable = enable;
	}

	void FRasterizerState::SetForcedSampleCount(uint32 count)
	{
		mDesc.ForcedSampleCount = count;
	}

	void FRasterizerState::SetConservativeRasterEnable(bool enable)
	{
		if (enable)
		{
			mDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;
		}
		else
		{
			mDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		}		
	}

	D3D12_FILL_MODE FRasterizerState::GetD3DFillMode(ERasterizerFillMode fillMode)
	{
		switch (fillMode)
		{
		case ERasterizerFillMode::Wireframe:
			return D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME;
		case ERasterizerFillMode::Solid:
			return D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
		default:
			ASSERT(false);
			return D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
		}
	}

	D3D12_CULL_MODE FRasterizerState::GetD3DCullMode(ERasterizerCullMode cullMode)
	{
		switch (cullMode)
		{
		case ERasterizerCullMode::None:
			return D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
		case ERasterizerCullMode::Front:
			return D3D12_CULL_MODE::D3D12_CULL_MODE_FRONT;
		case ERasterizerCullMode::Back:
			return D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
		default:
			ASSERT(false);
			return D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
		}	
	}
}


