#pragma once

#include "d3dx12.h"

namespace Dash
{
	class FRootSignature;

	class FPipelineStateObject
	{
	public:
		FPipelineStateObject(const std::wstring& name);

		static void DestroyAll();

		void SetRootSignature(const FRootSignature& rootSignature);

		const FRootSignature& GetRootSignature() const
		{
			ASSERT(mRootSignature != nullptr);
			return *mRootSignature;
		}

		ID3D12PipelineState* GetPipelineState() const { return mPSO; }

	protected:
		std::wstring mName;

		FRootSignature* mRootSignature;

		ID3D12PipelineState* mPSO;
	};

	class FGraphicsPSO : public FPipelineStateObject
	{
	public:
		FGraphicsPSO();

		void SetBlendState(const D3D12_BLEND_DESC& blendDesc);
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterDesc);

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC mPSODesc;
		std::shared_ptr<const D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	};
}