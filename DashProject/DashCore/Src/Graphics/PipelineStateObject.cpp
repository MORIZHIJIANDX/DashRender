#include "PCH.h"
#include "PipelineStateObject.h"
#include "RootSignature.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"
#include "Utility/Hash.h"

namespace Dash
{
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

	void FPipelineStateObject::SetRootSignature(const FRootSignature& rootSignature)
	{
		mRootSignature = &rootSignature;
	}

	void FPipelineStateObject::ApplyShaderPass()
	{
		if (mShaderPass)
		{
			if (!mShaderPass->IsValid())
			{
				mShaderPass->Finalize(mShaderPass->GetPassName());
			}

			const std::map<EShaderStage, FShaderResource*>& shaders = mShaderPass->GetShaders();
			for (auto& pair : shaders)
			{
				if (pair.second != nullptr)
				{
					SetShader(*pair.second);
				}
			}

			SetRootSignature(mShaderPass->GetRootSignature());
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

	void FGraphicsPSO::SetBlendState(const D3D12_BLEND_DESC& blendDesc)
	{
		mPSODesc.BlendState = blendDesc;
	}

	void FGraphicsPSO::SetSamplerMask(UINT samperMask)
	{
		mPSODesc.SampleMask = samperMask;
	}

	void FGraphicsPSO::SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterDesc)
	{
		mPSODesc.RasterizerState = rasterDesc;
	}

	void FGraphicsPSO::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc)
	{
		mPSODesc.DepthStencilState = depthStencilDesc;
	}

	void FGraphicsPSO::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType)
	{
		mPSODesc.PrimitiveTopologyType = primitiveTopologyType;
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

	void FGraphicsPSO::SetShader(const FShaderResource& shader)
	{	
		ASSERT(shader.GetShaderStage() != EShaderStage::Compute);

		const void* shaderData = shader.GetCompiledShader().Data;
		uint64_t shaderSize = shader.GetCompiledShader().Size;

		switch (shader.GetShaderStage())
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
		ApplyShaderPass();

		ASSERT(mRootSignature->IsFinalized());

		mPSODesc.pRootSignature = mRootSignature->GetSignature();
		
		size_t hashCode = HashState(&mPSODesc);
		hashCode = HashState(mPSODesc.InputLayout.pInputElementDescs, mPSODesc.InputLayout.NumElements, hashCode);

		static std::mutex posMutex;
		ID3D12PipelineState** psoRef = nullptr;
		bool firstTimeCompile = false;
		{
			std::lock_guard<std::mutex> lock(posMutex);

			auto iter = GraphicsPipelineStateHashMap.find(hashCode);
			if (iter == GraphicsPipelineStateHashMap.end())
			{
				psoRef = GraphicsPipelineStateHashMap[hashCode].GetAddressOf();
				firstTimeCompile = true;
			}
			else
			{
				psoRef = iter->second.GetAddressOf();
			}
		}

		if (firstTimeCompile == true)
		{
			DX_CALL(FGraphicsCore::Device->CreateGraphicsPipelineState(&mPSODesc, IID_PPV_ARGS(&mPSO)));
			SetD3D12DebugName(mPSO, mName.c_str());
			GraphicsPipelineStateHashMap[hashCode].Attach(mPSO);
		}
		else
		{
			while (*psoRef == nullptr)
			{
				std::this_thread::yield();
			}

			mPSO = *psoRef;
		}
	}

	FComputePSO::FComputePSO(const std::string& name)
		: FPipelineStateObject(name)
		, mPSODesc{}
	{
		ZeroMemory(&mPSODesc, sizeof(mPSODesc));
		mPSODesc.NodeMask = 0;
	}

	void FComputePSO::SetShader(const FShaderResource& shader)
	{
		ASSERT(shader.GetShaderStage() == EShaderStage::Compute);

		const void* shaderData = shader.GetCompiledShader().Data;
		uint64_t shaderSize = shader.GetCompiledShader().Size;

		SetComputeShader(shaderData, shaderSize);
	}

	void FComputePSO::Finalize()
	{
		ApplyShaderPass();

		ASSERT(mRootSignature->IsFinalized());

		mPSODesc.pRootSignature = mRootSignature->GetSignature();

		size_t hashCode = HashState(&mPSODesc);

		static std::mutex posMutex;
		ID3D12PipelineState** psoRef = nullptr;
		bool firstTimeCompile = false;
		{
			std::lock_guard<std::mutex> lock(posMutex);

			auto iter = ComputePipelineStateHashMap.find(hashCode);
			if (iter == ComputePipelineStateHashMap.end())
			{
				psoRef = ComputePipelineStateHashMap[hashCode].GetAddressOf();
				firstTimeCompile = true;
			}
			else
			{
				psoRef = iter->second.GetAddressOf();
			}
		}

		if (firstTimeCompile == true)
		{
			DX_CALL(FGraphicsCore::Device->CreateComputePipelineState(&mPSODesc, IID_PPV_ARGS(&mPSO)));
			SetD3D12DebugName(mPSO, mName.c_str());
			ComputePipelineStateHashMap[hashCode].Attach(mPSO);
		}
		else
		{
			while (*psoRef == nullptr)
			{
				std::this_thread::yield();
			}

			mPSO = *psoRef;
		}
	}

}