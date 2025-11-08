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
	struct FShaderVariable
	{
		std::string Name;
		uint32 BaseIndex = 0;	// BaseIndex In CVB, SRV, UAV, Sampler ParamterArray
		uint32 Size = 0;
		uint32 StartOffset = 0;
		uint32 Stride = 0;
		uint32 RootParameterIndex = 0;
		uint32 DescriptorOffset = 0;
		EShaderParameterType ParamterType = EShaderParameterType::Invalid;
		EShaderResourceBindingType BindingType = EShaderResourceBindingType::Invalid;
	};

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

		const std::map<std::string, FShaderVariable>& GetShaderVariableMap(EShaderStage stage) { return mShaderVariableMaps[static_cast<uint32>(stage)]; }

		FShaderVariable FindShaderVariable(const std::string& parameterName, EShaderStage stage);
		FShaderVariable FindShaderVariable(EShaderParameterType type, uint32 baseIndex, EShaderStage stage);

		uint32 GetCBVParameterNum(EShaderStage stage) const { return mNumCBVParameters[static_cast<uint32>(stage)]; }
		uint32 GetSRVParameterNum(EShaderStage stage) const { return mNumSRVParameters[static_cast<uint32>(stage)]; }
		uint32 GetUAVParameterNum(EShaderStage stage) const { return mNumUAVParameters[static_cast<uint32>(stage)]; }
		uint32 GetSamplerParameterNum(EShaderStage stage) const { return mNumSamplerParameters[static_cast<uint32>(stage)]; }

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

		void Finalize();

		void CreateRootSignature(const FQuantizedBoundShaderState& quantizedBoundShaderState, std::vector<FShaderParameter>* inCBVParameters,
			std::vector<FShaderParameter>* inSRVParameters, std::vector<FShaderParameter>* inUAVParameters, std::vector<FShaderParameter>* inSamplerParameters, bool containBindlessParameter);
		void InitShaderRootParamters(EShaderStage stage, FBoundShaderState& boundShaderState, uint32& currentParameterIndex, std::vector<FShaderParameter>& inCBVParameters,
			std::vector<FShaderParameter>& inSRVParameters, std::vector<FShaderParameter>& inUAVParameters, std::vector<FShaderParameter>& inSamplerParameters);
		D3D12_SHADER_VISIBILITY GetShaderVisibility(EShaderStage stage);
		std::vector<FSamplerDesc> CreateStaticSamplers();
		int32 FindParameterByName(const std::vector<FShaderParameter>& parameterArray, const std::string& parameterName) const;
		std::vector<std::string> GetParameterNames(const std::vector<FShaderParameter>& parameterArray) const;
		void InitDescriptorRanges(FBoundShaderState& boundShaderState, std::vector<FShaderParameter>& parameters, uint32& rootParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, 
			D3D12_DESCRIPTOR_RANGE_FLAGS rangeFlags, EShaderStage stage);

		void AddVariables(std::map<std::string, FShaderVariable>& variableMaps, const std::vector<FShaderParameter>& inParameters, const std::map<std::string, EShaderResourceBindingType>& bindlessPrameterMap);
		 
	private:
		std::map<EShaderStage, FShaderResourceRef> mShaders;

		std::map<std::string, FShaderVariable> mShaderVariableMaps[GShaderStageCount];

		uint32 mNumCBVParameters[GShaderStageCount];
		uint32 mNumSRVParameters[GShaderStageCount];
		uint32 mNumUAVParameters[GShaderStageCount];
		uint32 mNumSamplerParameters[GShaderStageCount];

		FBlendState mBlendState{};
		FRasterizerState mRasterizerState{};
		FDepthStencilState mDepthStencilState{true, false};

		EShaderPassType mPassType;
		FRootSignature* mRootSignature;
		std::string mPassName;
		size_t ShadersHash;
	};
}