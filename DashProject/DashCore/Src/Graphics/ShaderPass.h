#pragma once
#include "ShaderResource.h"
#include "RootSignature.h"
#include "SamplerDesc.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "InputAssemblerLayout.h"

namespace Dash
{
	enum class EShaderPassType
	{
		Raster,
		Compute
	};

	class FShaderPass;
	using FShaderPassRef = std::shared_ptr<FShaderPass>;

	class FShaderPass
	{
	public:
		static FShaderPassRef MakeGraphicShaderPass(const std::string& passName, const std::vector<FShaderCreationInfo>& creationInfos,
			const FBlendState& blendState, const FRasterizerState& rasterizerState, const FDepthStencilState& depthStencilState);

		static FShaderPassRef MakeComputeShaderPass(const std::string& passName, const FShaderCreationInfo& creationInfo);

		EShaderPassType GetPassType() const { return mPassType; }
		const std::string& GetPassName() const { return mPassName; }
		FRootSignatureRef GetRootSignature() const { return mRootSignatureRef; }
		const std::map<EShaderStage, FShaderResourceRef>& GetShaders() const { return mShaders; }
		size_t GetShadersHash() const { return ShadersHash; }

		int32 FindCBVParameterByName(const std::string& parameterName) const;
		int32 FindSRVParameterByName(const std::string& parameterName) const;
		int32 FindUAVParameterByName(const std::string& parameterName) const;
		int32 FindSamplerParameterByName(const std::string& parameterName) const;

		size_t GetCBVParameterNum() const { return mCBVParameters.size(); }
		size_t GetSRVParameterNum() const { return mSRVParameters.size(); }
		size_t GetUAVParameterNum() const { return mUAVParameters.size(); }
		size_t GetSamplerParameterNum() const { return mSamplerParameters.size(); }

		const std::vector<FShaderParameter>& GetCBVParameters() const { return mCBVParameters; }
		const std::vector<FShaderParameter>& GetSRVParameters() const { return mSRVParameters; }
		const std::vector<FShaderParameter>& GetUAVParameters() const { return mUAVParameters; }
		const std::vector<FShaderParameter>& GetSamplerParameters() const { return mSamplerParameters; }

		const FBlendState& GetBlendState() const { return mBlendState; }
		const FRasterizerState& GetRasterizerState() const { return mRasterizerState; }
		const FDepthStencilState& GetDepthStencilState() const { return mDepthStencilState; }
		const FInputAssemblerLayout& GetInputLayout() const;

	protected:
		void SetShaders(const std::vector<FShaderCreationInfo>& creationInfos);
		void SetPassName(const std::string& name) { mPassName = name; }
		void SetBlendState(const FBlendState& blendState) { mBlendState = blendState; }
		void SetRasterizerState(const FRasterizerState& rasterizerState) { mRasterizerState = rasterizerState; }
		void SetDepthStencilState(const FDepthStencilState& depthStencilState) { mDepthStencilState = depthStencilState; }

		void Finalize(bool createStaticSamplers = true);

		void CreateRootSignature(bool createStaticSamplers);
		D3D12_SHADER_VISIBILITY GetShaderVisibility(EShaderStage stage);
		std::vector<FSamplerDesc> CreateStaticSamplers();
		int32 FindParameterByName(const std::vector<FShaderParameter>& parameterArray, const std::string& parameterName) const;
		std::vector<std::string> GetParameterNames(const std::vector<FShaderParameter>& parameterArray) const;
		void InitDescriptorRanges(std::vector<FShaderParameter>& parameters, UINT& rootParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE rangeType);

	private:
		std::map<EShaderStage, FShaderResourceRef> mShaders;

		std::vector<FShaderParameter> mCBVParameters;
		std::vector<FShaderParameter> mSRVParameters;
		std::vector<FShaderParameter> mUAVParameters;
		std::vector<FShaderParameter> mSamplerParameters;

		FBlendState mBlendState{};
		FRasterizerState mRasterizerState{};
		FDepthStencilState mDepthStencilState{true, false};

		EShaderPassType mPassType;
		FRootSignatureRef mRootSignatureRef;
		std::string mPassName;
		size_t ShadersHash;
	};
}