#include "PCH.h"
#include "DepthStencilState.h"

namespace Dash
{
	FDepthStencilState::FDepthStencilState(bool depthTestEnable, bool stencilTestEnable)
	{
		mDesc.DepthEnable = depthTestEnable;
		mDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		mDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		mDesc.StencilEnable = stencilTestEnable;
		mDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		mDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		mDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		mDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		mDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		mDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		mDesc.BackFace = mDesc.FrontFace;
	}

	FDepthStencilState::FDepthStencilState(bool depthTestEnable, EDepthWriteMask depthWriteMask, EDepthStencilComparisonFunc depthFunc, bool stencilTestEnable,
		uint8 stencilReadMask, uint8 stencilWriteMask)
	{
		mDesc.DepthEnable = depthTestEnable;
		mDesc.DepthWriteMask = GetD3DDepthWriteMask(depthWriteMask);
		mDesc.DepthFunc = GetD3DComparisonFunc(depthFunc);
		mDesc.StencilEnable = stencilTestEnable;
		mDesc.StencilReadMask = stencilReadMask;
		mDesc.StencilWriteMask = stencilWriteMask;
		mDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		mDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		mDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		mDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		mDesc.BackFace = mDesc.FrontFace;
	}

	void FDepthStencilState::SetDepthTestEnable(bool enable)
	{
		mDesc.DepthEnable = enable;
	}

	void FDepthStencilState::SetStencilTestEnable(bool enable)
	{
		mDesc.StencilEnable = enable;
	}

	void FDepthStencilState::SetDepthBoundsTestEnable(bool enable)
	{
		mDesc.DepthBoundsTestEnable = enable;
	}

	void FDepthStencilState::SetStencilMask(uint8 stencilReadMask, uint8 stencilWriteMask)
	{
		mDesc.StencilReadMask = stencilReadMask;
		mDesc.StencilWriteMask = stencilWriteMask;
	}

	void FDepthStencilState::SetFrontFaceStencilState(EStencilOperation stencilFailOp, EStencilOperation stencilDepthFailOp, EStencilOperation stencilPassOp, EDepthStencilComparisonFunc stencilFunc)
	{
		mDesc.FrontFace.StencilPassOp = GetD3DStencilOp(stencilFailOp);
		mDesc.FrontFace.StencilDepthFailOp = GetD3DStencilOp(stencilDepthFailOp);
		mDesc.FrontFace.StencilFailOp = GetD3DStencilOp(stencilPassOp);
		mDesc.FrontFace.StencilFunc = GetD3DComparisonFunc(stencilFunc);
	}

	void FDepthStencilState::SetBackFaceStencilState(EStencilOperation stencilFailOp, EStencilOperation stencilDepthFailOp, EStencilOperation stencilPassOp, EDepthStencilComparisonFunc stencilFunc)
	{
		mDesc.BackFace.StencilPassOp = GetD3DStencilOp(stencilFailOp);
		mDesc.BackFace.StencilDepthFailOp = GetD3DStencilOp(stencilDepthFailOp);
		mDesc.BackFace.StencilFailOp = GetD3DStencilOp(stencilPassOp);
		mDesc.BackFace.StencilFunc = GetD3DComparisonFunc(stencilFunc);
	}

	D3D12_DEPTH_WRITE_MASK FDepthStencilState::GetD3DDepthWriteMask(EDepthWriteMask mask) const
	{
		switch (mask)
		{
		case EDepthWriteMask::Zero:
			return D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ZERO;
		case EDepthWriteMask::All:
			return D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ALL;
		default:
			ASSERT(false);
			return D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ALL;
		}
	}

	D3D12_COMPARISON_FUNC FDepthStencilState::GetD3DComparisonFunc(EDepthStencilComparisonFunc func) const
	{
		switch (func)
		{
		case EDepthStencilComparisonFunc::Never:
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
		case EDepthStencilComparisonFunc::Less:
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
		case EDepthStencilComparisonFunc::Equal:
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL;
		case EDepthStencilComparisonFunc::LessEqual:
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case EDepthStencilComparisonFunc::Greater:
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
		case EDepthStencilComparisonFunc::NotEqual:
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
		case EDepthStencilComparisonFunc::GreaterEqual:
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case EDepthStencilComparisonFunc::Always:
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
		default:
			ASSERT(false);
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
		}
	}

	D3D12_STENCIL_OP FDepthStencilState::GetD3DStencilOp(EStencilOperation op) const
	{
		switch (op)
		{
		case EStencilOperation::Keep:
			return D3D12_STENCIL_OP::D3D12_STENCIL_OP_KEEP;
		case EStencilOperation::Zero:
			return D3D12_STENCIL_OP::D3D12_STENCIL_OP_ZERO;
		case EStencilOperation::Replace:
			return D3D12_STENCIL_OP::D3D12_STENCIL_OP_REPLACE;
		case EStencilOperation::IncrementSaturate:
			return D3D12_STENCIL_OP::D3D12_STENCIL_OP_INCR_SAT;
		case EStencilOperation::DecrementSaturate:
			return D3D12_STENCIL_OP::D3D12_STENCIL_OP_DECR_SAT;
		case EStencilOperation::Invert:
			return D3D12_STENCIL_OP::D3D12_STENCIL_OP_INVERT;
		case EStencilOperation::Increment:
			return D3D12_STENCIL_OP::D3D12_STENCIL_OP_INCR;
		case EStencilOperation::Decrement:
			return D3D12_STENCIL_OP::D3D12_STENCIL_OP_DECR;
		default:
			ASSERT(false);
			return D3D12_STENCIL_OP::D3D12_STENCIL_OP_KEEP;
		}
	}
}