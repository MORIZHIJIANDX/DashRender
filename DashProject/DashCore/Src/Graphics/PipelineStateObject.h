#pragma once

#include "GraphicTypesFwd.h"
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

	struct FGraphicsPipelineStateInitializer
	{
	public:
		FGraphicsPipelineStateInitializer()
			: PrimitiveTopology(EPrimitiveTopology::None)
			, HashCode(0)
			, PipelineStateStream()
		{
			PipelineStateStream.NodeMask = 0;
			PipelineStateStream.SampleMask = UINT_MAX;
			PipelineStateStream.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };
		}

		void SetBlendState(const FBlendState& blendDesc);
		void SetSamplerMask(uint32 samperMask);
		void SetRasterizerState(const FRasterizerState& rasterDesc);
		void SetDepthStencilState(const FDepthStencilState& depthStencilDesc);
		void SetPrimitiveTopologyType(EPrimitiveTopology primitiveTopologyType);
		void SetDepthTargetFormat(EResourceFormat depthTargetFormat, uint32 msaaCount = 1, uint32 msaaQuality = 0);
		void SetRenderTargetFormat(EResourceFormat renderTargetFormat, EResourceFormat depthTargetFormat, uint32 msaaCount = 1, uint32 msaaQuality = 0);
		void SetRenderTargetFormats(uint32 numRTVs, const EResourceFormat* renderTargetFormats, EResourceFormat depthTargetFormat, uint32 msaaCount = 1, uint32 msaaQuality = 0);
		void SetInputLayout(const FInputAssemblerLayout& layout);
		void SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE indexBufferProps);
		void SetShaderPass(const FShaderPassRef& shaderPass);

		void Finalize();

		FInputAssemblerLayout InputLayout;
		EPrimitiveTopology PrimitiveTopology;
		FShaderPassRef ShaderPass;
		size_t HashCode;
		CD3DX12_PIPELINE_STATE_STREAM2 PipelineStateStream;

	protected:
		void SetVertexShader(const void* binaryCode, size_t size) { PipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetPixelShader(const void* binaryCode, size_t size) { PipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetGeometryShader(const void* binaryCode, size_t size) { PipelineStateStream.GS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetHullShader(const void* binaryCode, size_t size) { PipelineStateStream.HS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetDomainShader(const void* binaryCode, size_t size) { PipelineStateStream.DS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }

		void SetShader(const FShaderResourceRef& shader);
	};

	struct FComputePipelineStateInitializer
	{
	public:
		FComputePipelineStateInitializer() 
			: HashCode(0)
			, PipelineStateStream()
		{}

		void SetShaderPass(const FShaderPassRef& shaderPass);

		void Finalize();

		FShaderPassRef ShaderPass;
		size_t HashCode;
		CD3DX12_PIPELINE_STATE_STREAM2 PipelineStateStream;

	protected:
		void SetComputeShader(const void* binaryCode, size_t size) { PipelineStateStream.CS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
	};

	class FPipelineStateObject
	{
	public:
		FPipelineStateObject(const std::string& name);
		virtual ~FPipelineStateObject() {}

		FRootSignature* GetRootSignature() const
		{
			ASSERT(mShaderPass != nullptr);
			return mShaderPass->GetRootSignature();
		}

		ID3D12PipelineState* GetPipelineState() const { return mPSO.GetReference(); }
		
		FShaderPassRef GetShaderPass() const { return mShaderPass;}

	protected:
		std::string mName;

		CD3DX12_PIPELINE_STATE_STREAM2 mPipelineStateStream;

		TRefCountPtr<ID3D12PipelineState> mPSO = nullptr;
		FShaderPassRef mShaderPass = nullptr;
	};

	class FGraphicsPSO : public FPipelineStateObject
	{
	public:
		FGraphicsPSO(const FGraphicsPipelineStateInitializer& initializer, const std::string& name);

		EPrimitiveTopology GetPrimitiveTopology() const { return mPrimitiveTopology; }

	private:
		void Init(const FGraphicsPipelineStateInitializer& initializer, const std::string& name);

	private:
		FInputAssemblerLayout mInputLayout;
		EPrimitiveTopology mPrimitiveTopology;
	};

	class FComputePSO : public FPipelineStateObject
	{
	public:
		FComputePSO(const FComputePipelineStateInitializer& initializer, const std::string& name);

	private:
		void Init(const FComputePipelineStateInitializer& initializer, const std::string& name);
	};

	class FPipelineStateCache
	{
	public:
		FGraphicsPSO* GetGraphicsPipelineState(const FGraphicsPipelineStateInitializer& initializer, const std::string& name);
		FComputePSO* GetComputePipelineState(const FComputePipelineStateInitializer& initializer, const std::string& name);

		void Destroy();

	private:
		std::mutex mGraphicsPipelineStateLock;
		std::mutex mComputePipelineStateLock;

		std::map<size_t, FGraphicsPSO*> mGraphicsPipelineStateHashMap;
		std::map<size_t, FComputePSO*> mComputePipelineStateHashMap;
	};
}