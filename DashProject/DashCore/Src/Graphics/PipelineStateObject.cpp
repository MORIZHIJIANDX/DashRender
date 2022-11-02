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

	FPipelineStateObject::FPipelineStateObject(const std::wstring& name)
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

	FGraphicsPSO::FGraphicsPSO(const std::wstring& name)
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

	void FGraphicsPSO::SetDepthTargetFormat(DXGI_FORMAT depthTargetFormat, UINT msaaCount /*= 1*/, UINT msaaQuality /*= 0*/)
	{
		mPSODesc.DSVFormat = depthTargetFormat;
		mPSODesc.SampleDesc.Count = msaaCount;
		mPSODesc.SampleDesc.Quality = msaaQuality;
	}

	void FGraphicsPSO::SetRenderTargetFormat(DXGI_FORMAT renderTargetFormat, DXGI_FORMAT depthTargetFormat, UINT msaaCount /*= 1*/, UINT msaaQuality /*= 0*/)
	{
		mPSODesc.NumRenderTargets = 1;
	}

	void FGraphicsPSO::SetRenderTargetFormats(UINT numRTVs, const DXGI_FORMAT* renderTargetFormats, DXGI_FORMAT depthTargetFormat, UINT msaaCount /*= 1*/, UINT msaaQuality /*= 0*/)
	{
		ASSERT_MSG(numRTVs == 0 || renderTargetFormats != nullptr, "Null format array conflicts with non-zero length");
		for (UINT i = 0; i < numRTVs; ++i)
		{
			ASSERT(renderTargetFormats[i] != DXGI_FORMAT_UNKNOWN);
			mPSODesc.RTVFormats[i] = renderTargetFormats[i];
		}
		for (UINT i = numRTVs; i < mPSODesc.NumRenderTargets; ++i)
		{
			mPSODesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
		}
		mPSODesc.NumRenderTargets = numRTVs;
		mPSODesc.DSVFormat = depthTargetFormat;
		mPSODesc.SampleDesc.Count = msaaCount;
		mPSODesc.SampleDesc.Quality = msaaQuality;
	}

	void FGraphicsPSO::SetInputLayout(UINT numElements, const D3D12_INPUT_ELEMENT_DESC* inputElementDescs)
	{
		mPSODesc.InputLayout.NumElements = numElements;

		if (numElements > 0)
		{
			D3D12_INPUT_ELEMENT_DESC* newElementsPtr = new D3D12_INPUT_ELEMENT_DESC[numElements * sizeof(D3D12_INPUT_ELEMENT_DESC)];
			std::memcpy(newElementsPtr, inputElementDescs, numElements * sizeof(D3D12_INPUT_ELEMENT_DESC));
			mInputLayout.reset((const D3D12_INPUT_ELEMENT_DESC*)newElementsPtr);
		}
		else
		{
			mInputLayout = nullptr;
		}
	}

	void FGraphicsPSO::SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE indexBufferProps)
	{
		mPSODesc.IBStripCutValue = indexBufferProps;
	}

	void FGraphicsPSO::Finalize()
	{
		ASSERT(mRootSignature->IsFinalized());
		mPSODesc.pRootSignature = mRootSignature->GetSignature();
		
		mPSODesc.InputLayout.pInputElementDescs = nullptr;
		size_t hashCode = HashState(&mPSODesc);
		hashCode = HashState(mInputLayout.get(), mPSODesc.InputLayout.NumElements, hashCode);
		mPSODesc.InputLayout.pInputElementDescs = mInputLayout.get();

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

	FComputePSO::FComputePSO(const std::wstring& name)
		: FPipelineStateObject(name)
		, mPSODesc{}
	{
		ZeroMemory(&mPSODesc, sizeof(mPSODesc));
		mPSODesc.NodeMask = 0;
	}

	void FComputePSO::Finalize()
	{
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