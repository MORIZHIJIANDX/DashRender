#pragma once

namespace Dash
{
	enum class EDepthWriteMask
	{
		Zero, All
	};

	enum class EDepthStencilComparisonFunc
	{
		Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always
	};

	enum class EStencilOperation
	{
		Keep, Zero, Replace, IncrementSaturate, DecrementSaturate, Invert, Increment, Decrement
	};

	class FDepthStencilState
	{
	public:
		FDepthStencilState(bool depthTestEnable, bool stencilTestEnable);

		FDepthStencilState(bool depthTestEnable, EDepthWriteMask depthWriteMask = EDepthWriteMask::All, EDepthStencilComparisonFunc depthFunc = EDepthStencilComparisonFunc::LessEqual, 
			bool stencilTestEnable = false, uint8 stencilReadMask = 0, uint8 stencilWriteMask = 0);

		void SetDepthTestEnable(bool enable);
		void SetStencilTestEnable(bool enable);
		void SetDepthBoundsTestEnable(bool enable);
		void SetStencilMask(uint8 stencilReadMask = 0, uint8 stencilWriteMask = 0);
		void SetFrontFaceStencilState(EStencilOperation stencilFailOp, EStencilOperation stencilDepthFailOp, EStencilOperation stencilPassOp, EDepthStencilComparisonFunc stencilFunc);
		void SetBackFaceStencilState(EStencilOperation stencilFailOp, EStencilOperation stencilDepthFailOp, EStencilOperation stencilPassOp, EDepthStencilComparisonFunc stencilFunc);
		
		const CD3DX12_DEPTH_STENCIL_DESC1& D3DDepthStencilState() const { return mDesc; }

	private:
		D3D12_DEPTH_WRITE_MASK GetD3DDepthWriteMask(EDepthWriteMask mask) const;
		D3D12_COMPARISON_FUNC  GetD3DComparisonFunc(EDepthStencilComparisonFunc func) const;
		D3D12_STENCIL_OP	   GetD3DStencilOp(EStencilOperation op) const;

	private:
		CD3DX12_DEPTH_STENCIL_DESC1 mDesc{};
	};
}