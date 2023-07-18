#include "PCH.h"
#include "PipelineStateObject.h"
#include "RootSignature.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"
#include "Utility/Hash.h"
#include "RenderDevice.h"

namespace Dash
{
	using namespace Microsoft::WRL;

	static std::map<size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> GraphicsPipelineStateHashMap;
	static std::map<size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> ComputePipelineStateHashMap;

	FPipelineStateObject::FPipelineStateObject(const std::string& name)
		: mName(name)
	{}

	void FPipelineStateObject::DestroyAll()
	{
		GraphicsPipelineStateHashMap.clear();
		ComputePipelineStateHashMap.clear();
	}

	void FPipelineStateObject::ApplyShaderPass()
	{
		if (mShaderPass)
		{

			const std::map<EShaderStage, FShaderResourceRef>& shaders = mShaderPass->GetShaders();
			for (auto& pair : shaders)
			{
				if (pair.second != nullptr)
				{
					SetShader(pair.second);
				}
			}
		}
	}

	FGraphicsPSO::FGraphicsPSO(const std::string& name)
		: FPipelineStateObject(name)
		, mPSODesc{}
	{
		ZeroMemory(&mPSODesc, sizeof(mPSODesc));
		mPSODesc.NodeMask = 0;
		mPSODesc.SampleMask = 0xFFFFFFFFu;
		mPSODesc.SampleDesc.Count = 1;
		mPSODesc.InputLayout.NumElements = 0;
	}

	FGraphicsPSORef FGraphicsPSO::MakeGraphicsPSO(const std::string& name)
	{
		return std::make_shared<FGraphicsPSO>(name);
	}

	void FGraphicsPSO::SetBlendState(const FBlendState& blendDesc)
	{
		mPSODesc.BlendState = blendDesc.D3DBlendState();
	}

	void FGraphicsPSO::SetSamplerMask(UINT samperMask)
	{
		mPSODesc.SampleMask = samperMask;
	}

	void FGraphicsPSO::SetRasterizerState(const FRasterizerState& rasterDesc)
	{
		mPSODesc.RasterizerState = rasterDesc.D3DRasterizerState();
	}

	void FGraphicsPSO::SetDepthStencilState(const FDepthStencilState& depthStencilDesc)
	{
		mPSODesc.DepthStencilState = depthStencilDesc.D3DDepthStencilState();
	}

	void FGraphicsPSO::SetPrimitiveTopologyType(EPrimitiveTopology primitiveTopology)
	{	
		mPrimitiveTopology = primitiveTopology;
		mPSODesc.PrimitiveTopologyType = D3DPrimitiveTopologyType(primitiveTopology);
	}

	void FGraphicsPSO::SetDepthTargetFormat(EResourceFormat depthTargetFormat, UINT msaaCount /*= 1*/, UINT msaaQuality /*= 0*/)
	{
		SetRenderTargetFormats(0, nullptr, depthTargetFormat, msaaCount, msaaQuality);
	}

	void FGraphicsPSO::SetRenderTargetFormat(EResourceFormat renderTargetFormat, EResourceFormat depthTargetFormat, UINT msaaCount /*= 1*/, UINT msaaQuality /*= 0*/)
	{
		SetRenderTargetFormats(1, &renderTargetFormat, depthTargetFormat, msaaCount, msaaQuality);
	}

	void FGraphicsPSO::SetRenderTargetFormats(UINT numRTVs, const EResourceFormat* renderTargetFormats, EResourceFormat depthTargetFormat, UINT msaaCount /*= 1*/, UINT msaaQuality /*= 0*/)
	{
		ASSERT_MSG(numRTVs == 0 || renderTargetFormats != nullptr, "Null format array conflicts with non-zero length");
		for (UINT i = 0; i < numRTVs; ++i)
		{
			DXGI_FORMAT format = D3DFormat(renderTargetFormats[i]);
			ASSERT(format != DXGI_FORMAT_UNKNOWN);
			mPSODesc.RTVFormats[i] = format;
		}
		for (UINT i = numRTVs; i < mPSODesc.NumRenderTargets; ++i)
		{
			mPSODesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
		}
		mPSODesc.NumRenderTargets = numRTVs;
		mPSODesc.DSVFormat = D3DFormat(depthTargetFormat);
		mPSODesc.SampleDesc.Count = msaaCount;
		mPSODesc.SampleDesc.Quality = msaaQuality;
	}

	void FGraphicsPSO::SetInputLayout(const FInputAssemblerLayout& layout)
	{
		mInputLayout = layout;
		mPSODesc.InputLayout = mInputLayout.D3DLayout();
	}

	void FGraphicsPSO::SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE indexBufferProps)
	{
		mPSODesc.IBStripCutValue = indexBufferProps;
	}

	void FGraphicsPSO::SetShader(FShaderResourceRef shader)
	{	
		ASSERT(shader && shader->GetShaderStage() != EShaderStage::Compute);

		const void* shaderData = shader->GetCompiledShader().Data;
		uint32_t shaderSize = shader->GetCompiledShader().Size;

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

	void FGraphicsPSO::Finalize()
	{
		if (mIsFinalized)
		{
			return;
		}

		ApplyShaderPass();

		if (mShaderPass)
		{
			SetBlendState(mShaderPass->GetBlendState());
			SetRasterizerState(mShaderPass->GetRasterizerState());
			SetDepthStencilState(mShaderPass->GetDepthStencilState());
			SetInputLayout(mShaderPass->GetInputLayout());
		}

		ASSERT(mShaderPass != nullptr);
		ASSERT(mShaderPass->GetRootSignature()->IsFinalized());

		mPSODesc.pRootSignature = mShaderPass->GetRootSignature()->GetSignature();

		size_t hashCode = HashState(&mPSODesc);
		hashCode = HashState(mPSODesc.InputLayout.pInputElementDescs, mPSODesc.InputLayout.NumElements, hashCode);

		static std::mutex posMutex;
		ComPtr<ID3D12PipelineState>* psoRef = nullptr;
		bool firstTimeCompile = false;
		{
			std::lock_guard<std::mutex> lock(posMutex);

			auto iter = GraphicsPipelineStateHashMap.find(hashCode);
			if (iter == GraphicsPipelineStateHashMap.end())
			{
				psoRef = &GraphicsPipelineStateHashMap[hashCode];
				firstTimeCompile = true;
			}
			else
			{
				psoRef = &iter->second;
			}
		}

		if (firstTimeCompile == true)
		{
			ComPtr<ID3D12PipelineState> newPSO;
			DX_CALL(FGraphicsCore::Device->CreateGraphicsPipelineState(&mPSODesc, newPSO));
			SetD3D12DebugName(newPSO.Get(), mName.c_str());
			GraphicsPipelineStateHashMap[hashCode] = newPSO;
			mPSO = newPSO.Get();
		}
		else
		{
			while (*psoRef == nullptr)
			{
				std::this_thread::yield();
			}

			mPSO = psoRef->Get();
		}

		mIsFinalized = true;
	}

	FComputePSO::FComputePSO(const std::string& name)
		: FPipelineStateObject(name)
		, mPSODesc{}
	{
		ZeroMemory(&mPSODesc, sizeof(mPSODesc));
		mPSODesc.NodeMask = 0;
	}

	FComputePSORef FComputePSO::MakeComputePSO(const std::string& name)
	{
		return std::make_shared<FComputePSO>(name);
	}

	void FComputePSO::SetShader(FShaderResourceRef shader)
	{
		ASSERT(shader && shader->GetShaderStage() == EShaderStage::Compute);

		const void* shaderData = shader->GetCompiledShader().Data;
		uint32_t shaderSize = shader->GetCompiledShader().Size;

		SetComputeShader(shaderData, shaderSize);
	}

	void FComputePSO::Finalize()
	{
		if (mIsFinalized)
		{
			return;
		}

		ApplyShaderPass();

		ASSERT(mShaderPass != nullptr);
		ASSERT(mShaderPass->GetRootSignature()->IsFinalized());

		mPSODesc.pRootSignature = mShaderPass->GetRootSignature()->GetSignature();

		size_t hashCode = HashState(&mPSODesc);

		static std::mutex posMutex;
		ComPtr<ID3D12PipelineState>* psoRef = nullptr;
		bool firstTimeCompile = false;
		{
			std::lock_guard<std::mutex> lock(posMutex);

			auto iter = ComputePipelineStateHashMap.find(hashCode);
			if (iter == ComputePipelineStateHashMap.end())
			{
				psoRef = &ComputePipelineStateHashMap[hashCode];
				firstTimeCompile = true;
			}
			else
			{
				psoRef = &iter->second;
			}
		}

		if (firstTimeCompile == true)
		{
			ComPtr<ID3D12PipelineState> newPSO;
			DX_CALL(FGraphicsCore::Device->CreateComputePipelineState(&mPSODesc, newPSO));
			SetD3D12DebugName(newPSO.Get(), mName.c_str());
			ComputePipelineStateHashMap[hashCode] = newPSO;
		}
		else
		{
			while (*psoRef == nullptr)
			{
				std::this_thread::yield();
			}

			mPSO = psoRef->Get();
		}

		mIsFinalized = true;
	}
}