#pragma once

#include <d3d12.h>
#include "Utility/BitwiseEnum.h"
#include "RenderTarget.h"

namespace Dash
{
	enum class EBlendState
	{
		Zero, One, SrcColor, InvSrcColor, SrcAlpha, InvSrcAlpha, 
		DestAlpha, InvDestAlpha, DestColor, InvDestColor, 
		SrcAlphaSat, 
		BlendFactor, InvBlendFactor, 
		Src1Color, Src1Alpha, InvSrc1Alpha
	};

	enum class EBlendOperation
	{
		Add, Subtract, ReverseSubtract, Min, Max
	};

	enum class EBlendLogicOperation
	{
		Clear, Set, Copy, CopyInverted, Noop, Invert, And, Nand, Or, Nor, Xor, Equalv, AndReverse, AndInverted, OrReverse, OrInverted
	};

	enum class ERenderTargetWriteMask : uint8_t
	{
		None = 0,
		Red = 1,
		Green = 2,
		Blue = 4,
		Alpha = 8,
		All = Red | Green | Blue | Alpha
	};
	
	ENABLE_BITMASK_OPERATORS(ERenderTargetWriteMask);

	class FBlendState
	{
	public:
		FBlendState(EBlendState srcBlend = EBlendState::SrcAlpha, EBlendState destBlend = EBlendState::InvSrcAlpha, EBlendOperation blendOp = EBlendOperation::Add,
			EBlendState srcBlendAlpha = EBlendState::One, EBlendState destBlendAlpha = EBlendState::InvSrcAlpha, EBlendOperation blendOpAlpha = EBlendOperation::Add,
			EBlendLogicOperation logicOp = EBlendLogicOperation::Noop, ERenderTargetWriteMask renderTargetWriteMask = ERenderTargetWriteMask::All);

		FBlendState(bool blendEnable, bool logicOpEnable, 
			EBlendState srcBlend = EBlendState::SrcAlpha, EBlendState destBlend = EBlendState::InvSrcAlpha, EBlendOperation blendOp = EBlendOperation::Add,
			EBlendState srcBlendAlpha = EBlendState::One, EBlendState destBlendAlpha = EBlendState::InvSrcAlpha, EBlendOperation blendOpAlpha = EBlendOperation::Add, 
			EBlendLogicOperation logicOp = EBlendLogicOperation::Noop, ERenderTargetWriteMask renderTargetWriteMask = ERenderTargetWriteMask::All);

		void SetSourceValues(EBlendState srcColorBlend, EBlendState srcAlphaBlend, ERenderTarget rt = ERenderTarget::RT0);
		void SetDestinationValues(EBlendState destColorBlend, EBlendState destAlphaBlend, ERenderTarget rt = ERenderTarget::RT0);
		void SetFunctions(EBlendOperation colorOp, EBlendOperation alphaOp, ERenderTarget rt = ERenderTarget::RT0);
		void SetBlendingEnabled(bool enabled, ERenderTarget rt = ERenderTarget::RT0);
		void SetIndependentBlendEnable(bool enabled);

		const D3D12_BLEND_DESC& D3DBlendState() const { return mDesc; }

	private:
		D3D12_BLEND GetD3DBlend(EBlendState state);
		D3D12_BLEND_OP GetD3DBlendOp(EBlendOperation op);
		D3D12_LOGIC_OP GetD3DLogicOp(EBlendLogicOperation op);
		D3D12_COLOR_WRITE_ENABLE GetD3DColorWriteMask(ERenderTargetWriteMask mask);
		 
	private:
		D3D12_BLEND_DESC mDesc;
	};
}