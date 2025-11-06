#include "PCH.h"
#include "PipelineStateObject.h"
#include "RootSignature.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"
#include "Utility/Hash.h"
#include "RenderDevice.h"

namespace Dash
{
	void FGraphicsPipelineStateInitializer::SetBlendState(const FBlendState& blendDesc)
	{
		PipelineStateStream.BlendState = blendDesc.D3DBlendState();
	}

	void FGraphicsPipelineStateInitializer::SetSamplerMask(uint32 samperMask)
	{
		PipelineStateStream.SampleMask = samperMask;
	}

	void FGraphicsPipelineStateInitializer::SetRasterizerState(const FRasterizerState& rasterDesc)
	{
		PipelineStateStream.RasterizerState = rasterDesc.D3DRasterizerState();
	}

	void FGraphicsPipelineStateInitializer::SetDepthStencilState(const FDepthStencilState& depthStencilDesc)
	{
		PipelineStateStream.DepthStencilState = depthStencilDesc.D3DDepthStencilState();
	}

	void FGraphicsPipelineStateInitializer::SetPrimitiveTopologyType(EPrimitiveTopology primitiveTopologyType)
	{
		PrimitiveTopology = primitiveTopologyType;
		PipelineStateStream.PrimitiveTopologyType = D3DPrimitiveTopologyType(primitiveTopologyType);
	}

	void FGraphicsPipelineStateInitializer::SetDepthTargetFormat(EResourceFormat depthTargetFormat, uint32 msaaCount, uint32 msaaQuality)
	{
		SetRenderTargetFormats(0, nullptr, depthTargetFormat, msaaCount, msaaQuality);
	}

	void FGraphicsPipelineStateInitializer::SetRenderTargetFormat(EResourceFormat renderTargetFormat, EResourceFormat depthTargetFormat, uint32 msaaCount, uint32 msaaQuality)
	{
		SetRenderTargetFormats(1, &renderTargetFormat, depthTargetFormat, msaaCount, msaaQuality);
	}

	void FGraphicsPipelineStateInitializer::SetRenderTargetFormats(uint32 numRTVs, const EResourceFormat* renderTargetFormats, EResourceFormat depthTargetFormat, uint32 msaaCount, uint32 msaaQuality)
	{
		ASSERT_MSG(numRTVs == 0 || renderTargetFormats != nullptr, "Null format array conflicts with non-zero length");

		D3D12_RT_FORMAT_ARRAY RTFormatArray = {};
		RTFormatArray.NumRenderTargets = numRTVs;

		for (uint32 i = 0; i < numRTVs; ++i)
		{
			DXGI_FORMAT format = D3DFormat(renderTargetFormats[i]);
			ASSERT(format != DXGI_FORMAT_UNKNOWN);
			RTFormatArray.RTFormats[i] = format;
		}
		for (uint32 i = numRTVs; i < (sizeof(RTFormatArray.RTFormats) / sizeof(RTFormatArray.RTFormats[0])); ++i)
		{
			RTFormatArray.RTFormats[i] = DXGI_FORMAT_UNKNOWN;
		}

		PipelineStateStream.RTVFormats = RTFormatArray;
		PipelineStateStream.DSVFormat = D3DFormat(depthTargetFormat);
		PipelineStateStream.SampleDesc = DXGI_SAMPLE_DESC{ msaaCount, msaaQuality };
	}

	void FGraphicsPipelineStateInitializer::SetInputLayout(const FInputAssemblerLayout& layout)
	{
		InputLayout = layout;
		PipelineStateStream.InputLayout = InputLayout.D3DLayout();
	}

	void FGraphicsPipelineStateInitializer::SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE indexBufferProps)
	{
		PipelineStateStream.IBStripCutValue = indexBufferProps;
	}

	void FGraphicsPipelineStateInitializer::SetShaderPass(const FShaderPassRef& shaderPass)
	{
		ASSERT(shaderPass != nullptr);

		ShaderPass = shaderPass;

		const std::map<EShaderStage, FShaderResourceRef>& shaders = shaderPass->GetShaders();
		for (auto& pair : shaders)
		{
			if (pair.second != nullptr)
			{
				SetShader(pair.second);
			}
		}

		PipelineStateStream.pRootSignature = shaderPass->GetRootSignature()->GetSignature();

		SetBlendState(shaderPass->GetBlendState());
		SetRasterizerState(shaderPass->GetRasterizerState());
		SetDepthStencilState(shaderPass->GetDepthStencilState());
		SetInputLayout(shaderPass->GetInputLayout());
	}

	void FGraphicsPipelineStateInitializer::SetShader(const FShaderResourceRef& shader)
	{
		ASSERT(shader && shader->GetShaderStage() != EShaderStage::Compute);

		const void* shaderData = shader->GetCompiledShader().Data;
		uint32 shaderSize = shader->GetCompiledShader().Size;

		switch (shader->GetShaderStage())
		{
		case EShaderStage::Vertex:
			SetVertexShader(shaderData, shaderSize);
			break;
		case EShaderStage::Hull:
			SetHullShader(shaderData, shaderSize);
			break;
		case EShaderStage::Domain:
			SetDomainShader(shaderData, shaderSize);
			break;
		case EShaderStage::Geometry:
			SetVertexShader(shaderData, shaderSize);
			break;
		case EShaderStage::Pixel:
			SetPixelShader(shaderData, shaderSize);
			break;
		default:
			break;
		}
	}

	void FGraphicsPipelineStateInitializer::Finalize()
	{
		HashCode = HashState(&PipelineStateStream, 1, ShaderPass->GetShadersHash());
		const D3D12_INPUT_LAYOUT_DESC& InputLayoutDesc = PipelineStateStream.InputLayout;
		HashCode = HashState(InputLayoutDesc.pInputElementDescs, InputLayoutDesc.NumElements, HashCode);
	}

	void FComputePipelineStateInitializer::SetShaderPass(const FShaderPassRef& shaderPass)
	{
		ASSERT(shaderPass != nullptr);
		ASSERT(shaderPass->GetShaders().contains(EShaderStage::Compute));

		ShaderPass = shaderPass;

		PipelineStateStream.pRootSignature = shaderPass->GetRootSignature()->GetSignature();

		const std::map<EShaderStage, FShaderResourceRef>& shaders = shaderPass->GetShaders();
		FShaderResourceRef computeShader = shaders.find(EShaderStage::Compute)->second;

		const void* shaderData = computeShader->GetCompiledShader().Data;
		uint32 shaderSize = computeShader->GetCompiledShader().Size;

		SetComputeShader(shaderData, shaderSize);
	}

	void FComputePipelineStateInitializer::Finalize()
	{
		HashCode = HashState(&PipelineStateStream, 1, ShaderPass->GetShadersHash());
	}

	FPipelineStateObject::FPipelineStateObject(const std::string& name)
		: mName(name)
		, mPipelineStateStream()
	{
		mPipelineStateStream.NodeMask = 0;
		mPipelineStateStream.SampleMask = 0xFFFFFFFFu;
		mPipelineStateStream.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };
	}

	FGraphicsPSO::FGraphicsPSO(const FGraphicsPipelineStateInitializer& initializer, const std::string& name)
		: FPipelineStateObject(name)
		, mInputLayout()
		, mPrimitiveTopology(initializer.PrimitiveTopology)
	{
		Init(initializer, name);
	}

	void FGraphicsPSO::Init(const FGraphicsPipelineStateInitializer& initializer, const std::string& name)
	{
		mInputLayout = initializer.InputLayout;
		mShaderPass = initializer.ShaderPass;
		mPipelineStateStream = initializer.PipelineStateStream;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(mPipelineStateStream), &mPipelineStateStream
		};

		DX_CALL(FGraphicsCore::Device->CreatePipelineState(&pipelineStateStreamDesc, mPSO));
		SetD3D12DebugName(mPSO.GetReference(), mName.c_str());
	}

	FComputePSO::FComputePSO(const FComputePipelineStateInitializer& initializer, const std::string& name)
		: FPipelineStateObject(name)
	{
		Init(initializer, name);
	}

	void FComputePSO::Init(const FComputePipelineStateInitializer& initializer, const std::string& name)
	{
		mShaderPass = initializer.ShaderPass;
		mPipelineStateStream = initializer.PipelineStateStream;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(mPipelineStateStream), &mPipelineStateStream
		};

		DX_CALL(FGraphicsCore::Device->CreatePipelineState(&pipelineStateStreamDesc, mPSO));
		SetD3D12DebugName(mPSO.GetReference(), mName.c_str());
	}

	FGraphicsPSO* FPipelineStateCache::GetGraphicsPipelineState(const FGraphicsPipelineStateInitializer& initializer, const std::string& name)
	{
		ASSERT(initializer.HashCode != 0);

		std::lock_guard<std::mutex> lock(mGraphicsPipelineStateLock);
		auto iter = mGraphicsPipelineStateHashMap.find(initializer.HashCode);
		if(iter == mGraphicsPipelineStateHashMap.end())
		{
			FGraphicsPSO* newGraphicsPSO = new FGraphicsPSO(initializer, name);
			mGraphicsPipelineStateHashMap.emplace(initializer.HashCode, newGraphicsPSO);
			return newGraphicsPSO;
		}

		ASSERT(iter->second);
		return iter->second;
	}

	FComputePSO* FPipelineStateCache::GetComputePipelineState(const FComputePipelineStateInitializer& initializer, const std::string& name)
	{
		ASSERT(initializer.HashCode != 0);

		std::lock_guard<std::mutex> lock(mComputePipelineStateLock);
		auto iter = mComputePipelineStateHashMap.find(initializer.HashCode);
		if (iter == mComputePipelineStateHashMap.end())
		{
			FComputePSO* newComputePSO = new FComputePSO(initializer, name);
			mComputePipelineStateHashMap.emplace(initializer.HashCode, newComputePSO);
			return newComputePSO;
		}

		ASSERT(iter->second);
		return iter->second;
	}

	void FPipelineStateCache::Destroy()
	{
		{
			std::lock_guard<std::mutex> lock(mGraphicsPipelineStateLock);
			for (auto& Pair : mGraphicsPipelineStateHashMap)
			{
				delete Pair.second;
			}

			mGraphicsPipelineStateHashMap.clear();
		}

		{
			std::lock_guard<std::mutex> lock(mComputePipelineStateLock);
			for (auto& Pair : mComputePipelineStateHashMap)
			{
				delete Pair.second;
			}

			mComputePipelineStateHashMap.clear();
		}
	}
}