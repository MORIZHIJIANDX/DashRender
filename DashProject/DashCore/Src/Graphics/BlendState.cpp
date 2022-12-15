#include "PCH.h"
#include "BlendState.h"

namespace Dash
{
	FBlendState::FBlendState(bool blendEnable, bool logicOpEnable, EBlendState srcBlend, EBlendState destBlend, EBlendOperation blendOp, EBlendState srcBlendAlpha, EBlendState destBlendAlpha, EBlendOperation blendOpAlpha, EBlendLogicOperation logicOp, ERenderTargetWriteMask renderTargetWriteMask)
	{
		mDesc.AlphaToCoverageEnable = false;
		mDesc.IndependentBlendEnable = false;
		mDesc.RenderTarget[0].BlendEnable = blendEnable;
		mDesc.RenderTarget[0].LogicOpEnable = logicOpEnable;
		mDesc.RenderTarget[0].SrcBlend = GetD3DBlend(srcBlend);
		mDesc.RenderTarget[0].DestBlend = GetD3DBlend(destBlend);
		mDesc.RenderTarget[0].BlendOp = GetD3DBlendOp(blendOp);
		mDesc.RenderTarget[0].SrcBlendAlpha = GetD3DBlend(srcBlendAlpha);
		mDesc.RenderTarget[0].DestBlendAlpha = GetD3DBlend(destBlendAlpha);
		mDesc.RenderTarget[0].BlendOpAlpha = GetD3DBlendOp(blendOpAlpha);
		mDesc.RenderTarget[0].LogicOp = GetD3DLogicOp(logicOp);
		mDesc.RenderTarget[0].RenderTargetWriteMask = GetD3DColorWriteMask(renderTargetWriteMask);
	}

	FBlendState::FBlendState(EBlendState srcBlend, EBlendState destBlend, EBlendOperation blendOp, EBlendState srcBlendAlpha, EBlendState destBlendAlpha, EBlendOperation blendOpAlpha, EBlendLogicOperation logicOp, ERenderTargetWriteMask renderTargetWriteMask)
	{
		mDesc.AlphaToCoverageEnable = false;
		mDesc.IndependentBlendEnable = false;
		mDesc.RenderTarget[0].BlendEnable = true;
		mDesc.RenderTarget[0].LogicOpEnable = true;
		mDesc.RenderTarget[0].SrcBlend = GetD3DBlend(srcBlend);
		mDesc.RenderTarget[0].DestBlend = GetD3DBlend(destBlend);
		mDesc.RenderTarget[0].BlendOp = GetD3DBlendOp(blendOp);
		mDesc.RenderTarget[0].SrcBlendAlpha = GetD3DBlend(srcBlendAlpha);
		mDesc.RenderTarget[0].DestBlendAlpha = GetD3DBlend(destBlendAlpha);
		mDesc.RenderTarget[0].BlendOpAlpha = GetD3DBlendOp(blendOpAlpha);
		mDesc.RenderTarget[0].LogicOp = GetD3DLogicOp(logicOp);
		mDesc.RenderTarget[0].RenderTargetWriteMask = GetD3DColorWriteMask(renderTargetWriteMask);
	}

	void FBlendState::SetSourceValues(EBlendState srcColorBlend, EBlendState srcAlphaBlend, ERenderTarget rt)
	{
		auto rtIdx = std::underlying_type<ERenderTarget>::type(rt);
		mDesc.RenderTarget[rtIdx].SrcBlend = GetD3DBlend(srcColorBlend);
		mDesc.RenderTarget[rtIdx].SrcBlendAlpha = GetD3DBlend(srcAlphaBlend);
	}

	void FBlendState::SetDestinationValues(EBlendState destColorBlend, EBlendState destAlphaBlend, ERenderTarget rt)
	{
		auto rtIdx = std::underlying_type<ERenderTarget>::type(rt);
		mDesc.RenderTarget[rtIdx].DestBlend = GetD3DBlend(destColorBlend);
		mDesc.RenderTarget[rtIdx].DestBlendAlpha = GetD3DBlend(destAlphaBlend);
	}

	void FBlendState::SetFunctions(EBlendOperation colorOp, EBlendOperation alphaOp, ERenderTarget rt)
	{
		auto rtIdx = std::underlying_type<ERenderTarget>::type(rt);
		mDesc.RenderTarget[rtIdx].BlendOp = GetD3DBlendOp(colorOp);
		mDesc.RenderTarget[rtIdx].BlendOpAlpha = GetD3DBlendOp(alphaOp);
	}

	void FBlendState::SetBlendingEnabled(bool enabled, ERenderTarget rt)
	{
		auto rtIdx = std::underlying_type<ERenderTarget>::type(rt);
		mDesc.RenderTarget[rtIdx].BlendEnable = enabled;
	}

	void FBlendState::SetIndependentBlendEnable(bool enabled)
	{
		mDesc.IndependentBlendEnable = enabled;
	}

	D3D12_BLEND FBlendState::GetD3DBlend(EBlendState state)
	{
		switch (state)
		{
		case EBlendState::Zero:
			return D3D12_BLEND::D3D12_BLEND_ZERO;
		case EBlendState::One:
			return D3D12_BLEND::D3D12_BLEND_ONE;
		case EBlendState::SrcColor:
			return D3D12_BLEND::D3D12_BLEND_SRC_COLOR;
		case EBlendState::InvSrcColor:
			return D3D12_BLEND::D3D12_BLEND_INV_SRC_COLOR;
		case EBlendState::SrcAlpha:
			return D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
		case EBlendState::InvSrcAlpha:
			return D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
		case EBlendState::DestAlpha:
			return D3D12_BLEND::D3D12_BLEND_DEST_ALPHA;
		case EBlendState::InvDestAlpha:
			return D3D12_BLEND::D3D12_BLEND_INV_DEST_ALPHA;
		case EBlendState::DestColor:
			return D3D12_BLEND::D3D12_BLEND_DEST_COLOR;
		case EBlendState::InvDestColor:
			return D3D12_BLEND::D3D12_BLEND_INV_DEST_COLOR;
		case EBlendState::SrcAlphaSat:
			return D3D12_BLEND::D3D12_BLEND_SRC_ALPHA_SAT;
		case EBlendState::BlendFactor:
			return D3D12_BLEND::D3D12_BLEND_BLEND_FACTOR;
		case EBlendState::InvBlendFactor:
			return D3D12_BLEND::D3D12_BLEND_INV_BLEND_FACTOR;
		case EBlendState::Src1Color:
			return D3D12_BLEND::D3D12_BLEND_SRC1_COLOR;
		case EBlendState::Src1Alpha:
			return D3D12_BLEND::D3D12_BLEND_SRC1_ALPHA;
		case EBlendState::InvSrc1Alpha:
			return D3D12_BLEND::D3D12_BLEND_INV_SRC1_ALPHA;
		default:
			ASSERT(false);
			return D3D12_BLEND::D3D12_BLEND_ONE;
		}
	}

	D3D12_BLEND_OP FBlendState::GetD3DBlendOp(EBlendOperation op)
	{
		switch (op)
		{
		case EBlendOperation::Add:
			return D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
		case EBlendOperation::Subtract:
			return D3D12_BLEND_OP::D3D12_BLEND_OP_SUBTRACT;
		case EBlendOperation::ReverseSubtract:
			return D3D12_BLEND_OP::D3D12_BLEND_OP_REV_SUBTRACT;
		case EBlendOperation::Min:
			return D3D12_BLEND_OP::D3D12_BLEND_OP_MIN;
		case EBlendOperation::Max:
			return D3D12_BLEND_OP::D3D12_BLEND_OP_MAX;
		default:
			ASSERT(false);
			return D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
		}
	}

	D3D12_LOGIC_OP FBlendState::GetD3DLogicOp(EBlendLogicOperation op)
	{
		switch (op)
		{
		case EBlendLogicOperation::Clear:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_CLEAR;
		case EBlendLogicOperation::Set:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_SET;
		case EBlendLogicOperation::Copy:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_COPY;
		case EBlendLogicOperation::CopyInverted:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_COPY_INVERTED;
		case EBlendLogicOperation::Noop:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOOP;
		case EBlendLogicOperation::Invert:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_INVERT;
		case EBlendLogicOperation::And:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_AND;
		case EBlendLogicOperation::Nand:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_NAND;
		case EBlendLogicOperation::Or:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_OR;
		case EBlendLogicOperation::Nor:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOR;
		case EBlendLogicOperation::Xor:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_XOR;
		case EBlendLogicOperation::Equalv:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_EQUIV;
		case EBlendLogicOperation::AndReverse:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_AND_REVERSE;
		case EBlendLogicOperation::AndInverted:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_AND_INVERTED;
		case EBlendLogicOperation::OrReverse:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_OR_REVERSE;
		case EBlendLogicOperation::OrInverted:
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_OR_INVERTED;
		default:
			ASSERT(false);
			return D3D12_LOGIC_OP::D3D12_LOGIC_OP_NOOP;
		}
	}


	ENABLE_BITMASK_OPERATORS(D3D12_COLOR_WRITE_ENABLE);

	D3D12_COLOR_WRITE_ENABLE FBlendState::GetD3DColorWriteMask(ERenderTargetWriteMask mask)
	{
		D3D12_COLOR_WRITE_ENABLE writeMask = static_cast<D3D12_COLOR_WRITE_ENABLE>(0);
		if (EnumMaskContains(mask, ERenderTargetWriteMask::Red)) writeMask |= D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_RED;
		if (EnumMaskContains(mask, ERenderTargetWriteMask::Green)) writeMask |= D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_GREEN;
		if (EnumMaskContains(mask, ERenderTargetWriteMask::Blue)) writeMask |= D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_BLUE;
		if (EnumMaskContains(mask, ERenderTargetWriteMask::Alpha)) writeMask |= D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALPHA;
		if (EnumMaskContains(mask, ERenderTargetWriteMask::All)) writeMask |= D3D12_COLOR_WRITE_ENABLE::D3D12_COLOR_WRITE_ENABLE_ALL;
		return writeMask;
	}
}