#pragma once

#include "d3dx12.h"
#include "InputAssemblerLayout.h"
#include "ShaderPass.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"

namespace Dash
{
	class FRootSignature;

	class FPipelineStateObject
	{
	public:
		FPipelineStateObject(const std::string& name);
		virtual ~FPipelineStateObject() {}

		static void DestroyAll();

		void SetRootSignature(const FRootSignature& rootSignature);

		const FRootSignature& GetRootSignature() const
		{
			ASSERT(mRootSignature != nullptr);
			return *mRootSignature;
		}

		ID3D12PipelineState* GetPipelineState() const { return mPSO; }
		
		virtual void SetShader(const FShaderResource& shader) = 0;
		void SetShaderPass(const FShaderPass& shaderPass) { mShaderPass = const_cast<FShaderPass*>(&shaderPass); }
		virtual void Finalize() = 0;

		bool IsFinalized() const { return mIsFinalized; }

		const FShaderPass* GetShaderPass() const { return mShaderPass;}

	protected:
		void ApplyShaderPass();

	protected:
		std::string mName;

		bool mIsFinalized = false;

		const FRootSignature* mRootSignature = nullptr;

		ID3D12PipelineState* mPSO = nullptr;
		FShaderPass* mShaderPass = nullptr;
	};

	class FGraphicsPSO : public FPipelineStateObject
	{
	public:
		FGraphicsPSO(const std::string& name);

		void SetBlendState(const FBlendState& blendDesc);
		void SetSamplerMask(UINT samperMask);
		void SetRasterizerState(const FRasterizerState& rasterDesc);
		void SetDepthStencilState(const FDepthStencilState& depthStencilDesc);
		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType);
		void SetDepthTargetFormat(EResourceFormat depthTargetFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
		void SetRenderTargetFormat(EResourceFormat renderTargetFormat, EResourceFormat depthTargetFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
		void SetRenderTargetFormats(UINT numRTVs, const EResourceFormat* renderTargetFormats, EResourceFormat depthTargetFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
		void SetInputLayout(const FInputAssemblerLayout& layout);
		void SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE indexBufferProps);

		void SetVertexShader(const void* binaryCode, size_t size) { mPSODesc.VS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetPixelShader(const void* binaryCode, size_t size) { mPSODesc.PS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetGeometryShader(const void* binaryCode, size_t size) { mPSODesc.GS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetHullShader(const void* binaryCode, size_t size) { mPSODesc.HS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetDomainShader(const void* binaryCode, size_t size) { mPSODesc.DS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }	

		void SetVertexShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPSODesc.VS = shaderByteCode; }
		void SetPixelShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPSODesc.PS = shaderByteCode; }
		void SetGeometryShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPSODesc.GS = shaderByteCode; }
		void SetHullShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPSODesc.HS = shaderByteCode; }
		void SetDomainShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPSODesc.DS = shaderByteCode; }

		virtual void SetShader(const FShaderResource& shader) override;

		virtual void Finalize() override;

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC mPSODesc{};
		FInputAssemblerLayout mInputLayout;
	};

	class FComputePSO : public FPipelineStateObject
	{
	public:
		FComputePSO(const std::string& name);

		void SetComputeShader(const void* binaryCode, size_t size) { mPSODesc.CS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetComputeShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPSODesc.CS = shaderByteCode; }

		virtual void SetShader(const FShaderResource& shader) override;

		virtual void Finalize() override;

	private:
		D3D12_COMPUTE_PIPELINE_STATE_DESC mPSODesc{};
	};


}