#pragma once

#include "d3dx12.h"
#include "InputAssemblerLayout.h"
#include "ShaderPass.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "PrimitiveTopology.h"

namespace Dash
{
	class FRootSignature;
	
	class FPipelineStateObject;
	class FGraphicsPSO;
	class FComputePSO;

	using FPipelineStateObjectRef = std::shared_ptr<FPipelineStateObject>;
	using FGraphicsPSORef = std::shared_ptr<FGraphicsPSO>;
	using FComputePSORef = std::shared_ptr<FComputePSO>;

	class FPipelineStateObject
	{
	public:
		FPipelineStateObject(const std::string& name);
		virtual ~FPipelineStateObject() {}

		static void DestroyAll();

		FRootSignatureRef GetRootSignature() const
		{
			ASSERT(mShaderPass != nullptr);
			return mShaderPass->GetRootSignature();
		}

		ID3D12PipelineState* GetPipelineState() const { return mPSO; }
		
		virtual void SetShader(FShaderResourceRef shader) = 0;
		void SetShaderPass(FShaderPassRef shaderPass) { mShaderPass = shaderPass; }
		virtual void Finalize() = 0;

		bool IsFinalized() const { return mIsFinalized; }

		FShaderPassRef GetShaderPass() const { return mShaderPass;}

	protected:
		void ApplyShaderPass();

	protected:
		std::string mName;

		bool mIsFinalized = false;

		CD3DX12_PIPELINE_STATE_STREAM2 mPipelineStateStream;

		ID3D12PipelineState* mPSO = nullptr;
		FShaderPassRef mShaderPass = nullptr;
	};

	class FGraphicsPSO : public FPipelineStateObject
	{
	public:
		FGraphicsPSO(const std::string& name);

		static FGraphicsPSORef MakeGraphicsPSO(const std::string& name);

		void SetBlendState(const FBlendState& blendDesc);
		void SetSamplerMask(UINT samperMask);
		void SetRasterizerState(const FRasterizerState& rasterDesc);
		void SetDepthStencilState(const FDepthStencilState& depthStencilDesc);
		void SetPrimitiveTopologyType(EPrimitiveTopology primitiveTopologyType);
		void SetDepthTargetFormat(EResourceFormat depthTargetFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
		void SetRenderTargetFormat(EResourceFormat renderTargetFormat, EResourceFormat depthTargetFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
		void SetRenderTargetFormats(UINT numRTVs, const EResourceFormat* renderTargetFormats, EResourceFormat depthTargetFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
		void SetInputLayout(const FInputAssemblerLayout& layout);
		void SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE indexBufferProps);

		void SetVertexShader(const void* binaryCode, size_t size) { mPipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetPixelShader(const void* binaryCode, size_t size) { mPipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetGeometryShader(const void* binaryCode, size_t size) { mPipelineStateStream.GS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetHullShader(const void* binaryCode, size_t size) { mPipelineStateStream.HS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetDomainShader(const void* binaryCode, size_t size) { mPipelineStateStream.DS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }

		void SetVertexShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPipelineStateStream.VS = shaderByteCode; }
		void SetPixelShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPipelineStateStream.PS = shaderByteCode; }
		void SetGeometryShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPipelineStateStream.GS = shaderByteCode; }
		void SetHullShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPipelineStateStream.HS = shaderByteCode; }
		void SetDomainShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPipelineStateStream.DS = shaderByteCode; }

		virtual void SetShader(FShaderResourceRef shader) override;

		virtual void Finalize() override;

		EPrimitiveTopology GetPrimitiveTopology() const { return mPrimitiveTopology; }

	private:
		FInputAssemblerLayout mInputLayout;
		EPrimitiveTopology mPrimitiveTopology;
	};

	class FComputePSO : public FPipelineStateObject
	{
	public:
		FComputePSO(const std::string& name);

		static FComputePSORef MakeComputePSO(const std::string& name);

		void SetComputeShader(const void* binaryCode, size_t size) { mPipelineStateStream.CS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetComputeShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPipelineStateStream.CS = shaderByteCode; }

		virtual void SetShader(FShaderResourceRef shader) override;

		virtual void Finalize() override;
	};
}