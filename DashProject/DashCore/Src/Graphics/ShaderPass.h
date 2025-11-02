#pragma once

#include "GraphicTypesFwd.h"
#include "Utility/ThreadSafeCounter.h"
#include "ShaderResource.h"
#include "RootSignature.h"
#include "SamplerDesc.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "InputAssemblerLayout.h"

namespace Dash
{
	class FShaderPass : public FRefCount
	{
	public:
		static FShaderPassRef MakeGraphicShaderPass(const std::string& passName, const std::vector<FShaderCreationInfo>& creationInfos,
			const FBlendState& blendState, const FRasterizerState& rasterizerState, const FDepthStencilState& depthStencilState);

		static FShaderPassRef MakeComputeShaderPass(const std::string& passName, const FShaderCreationInfo& creationInfo);

		EShaderPassType GetPassType() const { return mPassType; }
		const std::string& GetPassName() const { return mPassName; }
		FRootSignature* GetRootSignature() const { return mRootSignature; }
		const std::map<EShaderStage, FShaderResourceRef>& GetShaders() const { return mShaders; }
		size_t GetShadersHash() const { return ShadersHash; }

		int32 FindCBVParameterByName(const std::string& parameterName, EShaderStage stage) const;
		int32 FindSRVParameterByName(const std::string& parameterName, EShaderStage stage) const;
		int32 FindUAVParameterByName(const std::string& parameterName, EShaderStage stage) const;
		int32 FindSamplerParameterByName(const std::string& parameterName, EShaderStage stage) const;

		size_t GetCBVParameterNum(EShaderStage stage) const { return mCBVParameters.size(); }
		size_t GetSRVParameterNum(EShaderStage stage) const { return mSRVParameters.size(); }
		size_t GetUAVParameterNum(EShaderStage stage) const { return mUAVParameters.size(); }
		size_t GetSamplerParameterNum(EShaderStage stage) const { return mSamplerParameters.size(); }

		const std::vector<FShaderParameter>& GetCBVParameters(EShaderStage stage) const { return mCBVParameters; }
		const std::vector<FShaderParameter>& GetSRVParameters(EShaderStage stage) const { return mSRVParameters; }
		const std::vector<FShaderParameter>& GetUAVParameters(EShaderStage stage) const { return mUAVParameters; }
		const std::vector<FShaderParameter>& GetSamplerParameters(EShaderStage stage) const { return mSamplerParameters; }

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

		void CreateRootSignature(const FQuantizedBoundShaderState& quantizedBoundShaderState);
		void InitShaderRootParamters(EShaderStage stage, FBoundShaderState& boundShaderState, uint32 currentParameterIndex);
		D3D12_SHADER_VISIBILITY GetShaderVisibility(EShaderStage stage);
		std::vector<FSamplerDesc> CreateStaticSamplers();
		int32 FindParameterByName(const std::vector<FShaderParameter>& parameterArray, const std::string& parameterName) const;
		std::vector<std::string> GetParameterNames(const std::vector<FShaderParameter>& parameterArray) const;
		void InitDescriptorRanges(FBoundShaderState& boundShaderState, std::vector<FShaderParameter>& parameters, UINT& rootParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, EShaderStage stage);
		 
	private:
		std::map<EShaderStage, FShaderResourceRef> mShaders;

		std::vector<FConstantBufferVariable> mConstantVariables[GShaderStageCount];
		std::vector<FConstantBufferVariable> mBindlessSRVVariables[GShaderStageCount];
		std::vector<FConstantBufferVariable> mBindlessUAVVariables[GShaderStageCount];
		std::vector<FConstantBufferVariable> mBindlessSamplerVariables[GShaderStageCount];

		std::vector<FShaderParameter> mCBVParameters[GShaderStageCount];
		std::vector<FShaderParameter> mSRVParameters[GShaderStageCount];
		std::vector<FShaderParameter> mUAVParameters[GShaderStageCount];
		std::vector<FShaderParameter> mSamplerParameters[GShaderStageCount];

		FBlendState mBlendState{};
		FRasterizerState mRasterizerState{};
		FDepthStencilState mDepthStencilState{true, false};

		EShaderPassType mPassType;
		FRootSignature* mRootSignature;
		std::string mPassName;
		size_t ShadersHash;
	};
}