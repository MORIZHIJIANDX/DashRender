#pragma once

#include "d3dx12.h"

namespace Dash
{
	class FRootSignature;

	class FPipelineStateObject
	{
	public:
		FPipelineStateObject(const std::wstring& name);
		virtual ~FPipelineStateObject() {}

		static void DestroyAll();

		void SetRootSignature(const FRootSignature& rootSignature);

		const FRootSignature& GetRootSignature() const
		{
			ASSERT(mRootSignature != nullptr);
			return *mRootSignature;
		}

		ID3D12PipelineState* GetPipelineState() const { return mPSO; }

		virtual void Finalize() = 0;

		bool IsFinalized() const { return mIsFinalized; }

	protected:
		std::wstring mName;

		bool mIsFinalized = false;

		const FRootSignature* mRootSignature = nullptr;

		ID3D12PipelineState* mPSO = nullptr;
	};

	class FGraphicsPSO : public FPipelineStateObject
	{
	public:
		FGraphicsPSO(const std::wstring& name);

		void SetBlendState(const D3D12_BLEND_DESC& blendDesc);
		void SetSamplerMask(UINT samperMask);
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterDesc);
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);
		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType);
		void SetDepthTargetFormat(DXGI_FORMAT depthTargetFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
		void SetRenderTargetFormat(DXGI_FORMAT renderTargetFormat, DXGI_FORMAT depthTargetFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
		void SetRenderTargetFormats(UINT numRTVs, const DXGI_FORMAT* renderTargetFormats, DXGI_FORMAT depthTargetFormat, UINT msaaCount = 1, UINT msaaQuality = 0);
		void SetInputLayout(UINT numElements, const D3D12_INPUT_ELEMENT_DESC* inputElementDescs);
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

		virtual void Finalize() override;

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC mPSODesc;
		std::shared_ptr<const D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	};

	class FComputePSO : public FPipelineStateObject
	{
	public:
		FComputePSO(const std::wstring& name);

		void SetComputeShader(const void* binaryCode, size_t size) { mPSODesc.CS = CD3DX12_SHADER_BYTECODE(binaryCode, size); }
		void SetComputeShader(const D3D12_SHADER_BYTECODE& shaderByteCode) { mPSODesc.CS = shaderByteCode; }

		virtual void Finalize() override;

	private:
		D3D12_COMPUTE_PIPELINE_STATE_DESC mPSODesc;
	};


}