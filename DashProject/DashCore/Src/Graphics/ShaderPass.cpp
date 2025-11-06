#include "PCH.h"
#include "ShaderPass.h"
#include "ShaderMap.h"
#include "Utility/StringUtility.h"
#include "GraphicsCore.h"

namespace Dash
{
	FShaderPassRef FShaderPass::MakeGraphicShaderPass(const std::string& passName, const std::vector<FShaderCreationInfo>& creationInfos,
		const FBlendState& blendState, const FRasterizerState& rasterizerState, const FDepthStencilState& depthStencilState)
	{
		FShaderPassRef newPass = MakeRefCounted<FShaderPass>();
		newPass->SetPassName(passName);
		newPass->SetShaders(creationInfos);
		newPass->SetBlendState(blendState);
		newPass->SetRasterizerState(rasterizerState);
		newPass->SetDepthStencilState(depthStencilState);

		newPass->Finalize();

		return newPass;
	}

	FShaderPassRef Dash::FShaderPass::MakeComputeShaderPass(const std::string& passName, const FShaderCreationInfo& creationInfo)
	{
		FShaderPassRef newPass = MakeRefCounted<FShaderPass>();
		newPass->SetPassName(passName);
		newPass->SetShaders({ creationInfo });

		newPass->Finalize();

		return newPass;
	}

	FShaderVariable FShaderPass::FindShaderVariable(const std::string& parameterName, EShaderStage stage)
	{
		uint32 shaderStageIndex = static_cast<uint32>(stage);
		auto iter = mShaderVariableMaps[shaderStageIndex].find(parameterName);
		if (iter != mShaderVariableMaps[shaderStageIndex].end())
		{
			return iter->second;
		}
		else
		{
			return FShaderVariable{};
		}
	}

	FShaderVariable FShaderPass::FindShaderVariable(EShaderParameterType type, uint32 baseIndex, EShaderStage stage)
	{
		uint32 shaderStageIndex = static_cast<uint32>(stage);
		for (const auto& pair : mShaderVariableMaps[shaderStageIndex]) {
			if (pair.second.BaseIndex == baseIndex && pair.second.ParamterType == type)
			{
				return pair.second;
			}
		}
	
		return FShaderVariable{};
	}

	const FInputAssemblerLayout& FShaderPass::GetInputLayout() const
	{
		ASSERT(mShaders.contains(EShaderStage::Vertex));
		return mShaders.at(EShaderStage::Vertex)->GetInputLayout();
	}

	void FShaderPass::SetShaders(const std::vector<FShaderCreationInfo>& creationInfos)
	{
		std::set<EShaderStage> validShaderStages;
		for (int32 i = 0; i < creationInfos.size(); i++)
		{
			validShaderStages.insert(creationInfos[i].Stage);
		}
		
		ASSERT_MSG(validShaderStages.size() == creationInfos.size(), "Set duplicates shader stages.");

		for (int32 i = 0; i < creationInfos.size(); i++)
		{
			EShaderStage stage = creationInfos[i].Stage;
			mShaders[stage] = FShaderMap::LoadShader(creationInfos[i]);
			ASSERT(mShaders[stage] != nullptr);
		}
	}

	struct FParameterKey
	{
		D3D_SHADER_INPUT_TYPE ShaderInputType;
		UINT BindPoint = 0;
		UINT RegisterSpace = 0;
		D3D_SRV_DIMENSION ResourceDimension;

		bool operator<(const FParameterKey& src)const
		{
			if (ShaderInputType == src.ShaderInputType)
			{
				if (BindPoint == src.BindPoint)
				{
					if (RegisterSpace == src.RegisterSpace)
					{
						return ResourceDimension < src.ResourceDimension;
					}

					return RegisterSpace < src.RegisterSpace;
				}
				else
				{
					return BindPoint < src.BindPoint;
				}
			}
			
			return ShaderInputType < src.ShaderInputType;
		}
	};

	void FShaderPass::Finalize()
	{
		bool createStaticSamplers = false;
		mPassType = mShaders.contains(EShaderStage::Compute) ? EShaderPassType::Compute : EShaderPassType::Raster;
		
		std::vector<FShaderParameter> cbvParameters[GShaderStageCount];
		std::vector<FShaderParameter> srvParameters[GShaderStageCount];
		std::vector<FShaderParameter> uavParameters[GShaderStageCount];
		std::vector<FShaderParameter> samplerParameters[GShaderStageCount];

		FQuantizedBoundShaderState quantizedBoundShaderState;
		quantizedBoundShaderState.RootSignatureType = mPassType;

		std::vector<FShaderResourceRef> SortedShaders;
		for (auto& pair : mShaders)
		{
			EShaderStage shaderStage = pair.first;
			FShaderResourceRef shaderRef = pair.second;

			uint32 shaderStageIndex = static_cast<uint32>(shaderStage);

			cbvParameters[shaderStageIndex] = shaderRef->GetCBVParameters();
			srvParameters[shaderStageIndex] = shaderRef->GetSRVParameters();
			uavParameters[shaderStageIndex] = shaderRef->GetUAVParameters();

			for (auto& samplerParameter : shaderRef->GetSamplerParameters())
			{
				if (FStringUtility::Contains(samplerParameter.Name, "_Static"))
				{
					createStaticSamplers = true;
				}
				else
				{
					samplerParameters[shaderStageIndex].push_back(samplerParameter);
				}
			}

			mNumCBVParameters[shaderStageIndex] = static_cast<uint32>(cbvParameters[shaderStageIndex].size());
			mNumSRVParameters[shaderStageIndex] = static_cast<uint32>(srvParameters[shaderStageIndex].size());
			mNumUAVParameters[shaderStageIndex] = static_cast<uint32>(uavParameters[shaderStageIndex].size());
			mNumSamplerParameters[shaderStageIndex] = static_cast<uint32>(samplerParameters[shaderStageIndex].size());

			quantizedBoundShaderState.RegisterCounts[shaderStageIndex].ConstantBufferCount = mNumCBVParameters[shaderStageIndex];
			quantizedBoundShaderState.RegisterCounts[shaderStageIndex].ShaderResourceCount = mNumSRVParameters[shaderStageIndex];
			quantizedBoundShaderState.RegisterCounts[shaderStageIndex].UnorderedAccessCount = mNumUAVParameters[shaderStageIndex];
			quantizedBoundShaderState.RegisterCounts[shaderStageIndex].SamplerCount = mNumSamplerParameters[shaderStageIndex];

			quantizedBoundShaderState.NumRootParameters += mNumCBVParameters[shaderStageIndex];
			quantizedBoundShaderState.NumRootParameters += srvParameters[shaderStageIndex].empty() ? 0 : 1;
			quantizedBoundShaderState.NumRootParameters += uavParameters[shaderStageIndex].empty() ? 0 : 1;
			quantizedBoundShaderState.NumRootParameters += samplerParameters[shaderStageIndex].empty() ? 0 : 1;

			SortedShaders.push_back(shaderRef);

			DASH_LOG(LogTemp, Info, "ShaderType : {}", ShaderStageToString(shaderStage));
			DASH_LOG(LogTemp, Info, "Num Constant Buffer Parameters : {}", cbvParameters[shaderStageIndex].size());
			DASH_LOG(LogTemp, Info, "Num Shader Resource View Parameters : {}", srvParameters[shaderStageIndex].size());
			DASH_LOG(LogTemp, Info, "Num Unordered Access View Parameters : {}", uavParameters[shaderStageIndex].size());
			DASH_LOG(LogTemp, Info, "Num Sampler Parameters : {}", samplerParameters[shaderStageIndex].size());
		}

		quantizedBoundShaderState.NumStaticSamplers = createStaticSamplers ? 8 : 0;

		std::sort(SortedShaders.begin(), SortedShaders.end(), [](const FShaderResourceRef& a, const FShaderResourceRef& b){
			return a->GetShaderHash() < b->GetShaderHash();
		});

		std::string HashedShaderFileName;
		for (size_t Index = 0; Index < SortedShaders.size(); Index++)
		{
			HashedShaderFileName += SortedShaders[Index]->GetHashedFileName();
		}
		ShadersHash = std::hash<std::string>{}(HashedShaderFileName);

		CreateRootSignature(quantizedBoundShaderState, cbvParameters, srvParameters, uavParameters, samplerParameters);

		for (auto& pair : mShaders)
		{
			EShaderStage shaderStage = pair.first;
			FShaderResourceRef shaderRef = pair.second;

			uint32 shaderStageIndex = static_cast<uint32>(shaderStage);

			AddVariables(mShaderVariableMaps[shaderStageIndex], cbvParameters[shaderStageIndex]);
			AddVariables(mShaderVariableMaps[shaderStageIndex], srvParameters[shaderStageIndex]);
			AddVariables(mShaderVariableMaps[shaderStageIndex], uavParameters[shaderStageIndex]);
			AddVariables(mShaderVariableMaps[shaderStageIndex], samplerParameters[shaderStageIndex]);
		}
	}

	void FShaderPass::CreateRootSignature(const FQuantizedBoundShaderState& quantizedBoundShaderState, std::vector<FShaderParameter>* inCBVParameters,
		std::vector<FShaderParameter>* inSRVParameters, std::vector<FShaderParameter>* inUAVParameters, std::vector<FShaderParameter>* inSamplerParameters)
	{
		FBoundShaderState boundShaderState(quantizedBoundShaderState);
		
		uint32 rootParameterIndex = 0;

		for (auto& pair : mShaders)
		{
			EShaderStage shaderStage = pair.first;
			FShaderResourceRef shaderRef = pair.second;

			uint32 shaderStageIndex = static_cast<uint32>(shaderStage);

			InitShaderRootParamters(shaderStage, boundShaderState, rootParameterIndex, inCBVParameters[shaderStageIndex], inSRVParameters[shaderStageIndex],
				inUAVParameters[shaderStageIndex], inSamplerParameters[shaderStageIndex]);
		}
		 
		if (quantizedBoundShaderState.NumStaticSamplers > 0)
		{
			std::vector<FSamplerDesc> staticSamplers = CreateStaticSamplers();

			for (UINT index = 0; index < staticSamplers.size(); index++)
			{
				boundShaderState.InitStaticSampler(index, staticSamplers[index], D3D12_SHADER_VISIBILITY_ALL);
			}
		}
		
		boundShaderState.Finalize(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		mRootSignature = FGraphicsCore::RootSignatureManager->GetRootSignature(boundShaderState, mPassName + "_RootSignature");
	}

	void FShaderPass::InitShaderRootParamters(EShaderStage stage, FBoundShaderState& boundShaderState, uint32& currentParameterIndex, std::vector<FShaderParameter>& inCBVParameters,
		std::vector<FShaderParameter>& inSRVParameters, std::vector<FShaderParameter>& inUAVParameters, std::vector<FShaderParameter>& inSamplerParameters)
	{
		uint32 shaderStageIndex = static_cast<uint32>(stage);
		
		for (uint32 cbvIndex = 0; cbvIndex < inCBVParameters.size(); cbvIndex++)
		{
			FShaderParameter& constantBufferParameter = inCBVParameters[cbvIndex];

			constantBufferParameter.RootParameterIndex = currentParameterIndex;
			boundShaderState[currentParameterIndex].InitAsRootConstantBufferView(constantBufferParameter.BindPoint, GetShaderVisibility(stage), constantBufferParameter.RegisterSpace);
			currentParameterIndex++;

			DASH_LOG(LogTemp, Warning, "Shader Stage {} InitRootCBV RootParamterIndex {} Pramter Name {} BindPoint {}", shaderStageIndex, currentParameterIndex,
				constantBufferParameter.Name, constantBufferParameter.BindPoint);
		}

		if (!inSRVParameters.empty())
		{
			InitDescriptorRanges(boundShaderState, inSRVParameters, currentParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV, stage);
		}
		
		if (!inUAVParameters.empty())
		{
			InitDescriptorRanges(boundShaderState, inUAVParameters, currentParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV, stage);
		}

		if (!inSamplerParameters.empty())
		{
			InitDescriptorRanges(boundShaderState, inSamplerParameters, currentParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, stage);
		}
	}

	D3D12_SHADER_VISIBILITY FShaderPass::GetShaderVisibility(EShaderStage stage)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
		case EShaderStage::Hull:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_HULL;
		case EShaderStage::Domain:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_DOMAIN;
		case EShaderStage::Geometry:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_GEOMETRY;
		case EShaderStage::Pixel:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
		case EShaderStage::Compute:		
		default:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
		}
	}

	std::vector<FSamplerDesc> FShaderPass::CreateStaticSamplers()
	{
		// Applications usually only need a handful of samplers.  So just define them all up front
		// and keep them available as part of the root signature. 	
		FSamplerDesc pointWrap{ ESamplerFilter::Point, ESamplerAddressMode::Wrap, ESamplerComparisonFunc::Never };
		FSamplerDesc pointClamp{ ESamplerFilter::Point, ESamplerAddressMode::Clamp, ESamplerComparisonFunc::Never };
		FSamplerDesc linearWrap{ ESamplerFilter::Linear, ESamplerAddressMode::Wrap, ESamplerComparisonFunc::Never };
		FSamplerDesc linearClamp{ ESamplerFilter::Linear, ESamplerAddressMode::Clamp, ESamplerComparisonFunc::Never };
		FSamplerDesc anisotropicWrap{ ESamplerFilter::Anisotropic, ESamplerAddressMode::Wrap, ESamplerComparisonFunc::Never, FLinearColor::Black, 0.0f, 8 };
		FSamplerDesc anisotropicClamp{ ESamplerFilter::Anisotropic, ESamplerAddressMode::Clamp, ESamplerComparisonFunc::Never, FLinearColor::Black, 0.0f, 8 };                          
		FSamplerDesc shadow{ ESamplerFilter::Point, ESamplerAddressMode::Border, ESamplerComparisonFunc::LessEqual};
		FSamplerDesc depthMap{ ESamplerFilter::Linear, ESamplerAddressMode::Border, ESamplerComparisonFunc::LessEqual };

		std::vector<FSamplerDesc> StaticSamplers;
		StaticSamplers.push_back(pointWrap);
		StaticSamplers.push_back(pointClamp);
		StaticSamplers.push_back(linearWrap);
		StaticSamplers.push_back(linearClamp);
		StaticSamplers.push_back(anisotropicWrap);
		StaticSamplers.push_back(anisotropicClamp);
		StaticSamplers.push_back(shadow);
		StaticSamplers.push_back(depthMap);

		return StaticSamplers;
	}

	int32 FShaderPass::FindParameterByName(const std::vector<FShaderParameter>& parameterArray, const std::string& parameterName) const
	{
		for (int32 i = 0; i < parameterArray.size(); i++)
		{
			if (parameterArray[i].Name == parameterName)
			{
				return i;
			}
		}

		return INDEX_NONE;
	}

	std::vector<std::string> FShaderPass::GetParameterNames(const std::vector<FShaderParameter>& parameterArray) const
	{
		std::vector<std::string> names;
		for (size_t i = 0; i < parameterArray.size(); i++)
		{
			names.push_back(parameterArray[i].Name);
		}
		return names;
	}

	void FShaderPass::InitDescriptorRanges(FBoundShaderState& boundShaderState, std::vector<FShaderParameter>& parameters, UINT& rootParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE rangeType, EShaderStage stage)
	{
		uint32 parametersCount = static_cast<uint32>(parameters.size());
		if (parametersCount > 0)
		{
			for (uint32 parameterIndex = 0; parameterIndex < parametersCount; parameterIndex++)
			{
				parameters[parameterIndex].RootParameterIndex = rootParameterIndex;
				parameters[parameterIndex].DescriptorOffset = parameterIndex;	//offset in parameter

				ASSERT(parameters[parameterIndex].RegisterSpace == 0);
			}
			boundShaderState[rootParameterIndex].InitAsDescriptorTable(1, GetShaderVisibility(stage));
			// 不同 Shader 间的 register 相互独立，都是从 Bind Point 0 开始
			boundShaderState[rootParameterIndex].SetTableRange(0, rangeType, 0, parametersCount, 0);
			rootParameterIndex++;
		}
	}

	void FShaderPass::AddVariables(std::map<std::string, FShaderVariable>& variableMaps, const std::vector<FShaderParameter>& inParameters)
	{
		for (uint32 parameterIndex = 0; parameterIndex < inParameters.size(); parameterIndex++)
		{
			const FShaderParameter& parameter = inParameters[parameterIndex];

			std::string shaderVariableName = parameter.Name;

			FShaderVariable shaderVariable;
			shaderVariable.Name = parameter.Name;
			shaderVariable.BaseIndex = parameterIndex;
			shaderVariable.Size = parameter.Size;
			shaderVariable.StartOffset = 0;
			shaderVariable.Stride = parameter.Stride;
			shaderVariable.RootParameterIndex = parameter.RootParameterIndex;
			shaderVariable.DescriptorOffset = parameter.DescriptorOffset;
			shaderVariable.ParamterType = parameter.ParameterType;

			variableMaps.emplace(shaderVariableName, shaderVariable);

			if (parameter.ParameterType == EShaderParameterType::UniformBuffer)
			{
				for (uint32 uniformVariableIndex = 0; uniformVariableIndex < parameter.ConstantBufferVariables.size(); uniformVariableIndex++)
				{
					const FConstantBufferVariable& uniformVariable = parameter.ConstantBufferVariables[parameterIndex];

					shaderVariableName = uniformVariable.VariableName;
					shaderVariable.Name = uniformVariable.VariableName;
					shaderVariable.Size = uniformVariable.Size;
					shaderVariable.StartOffset = uniformVariable.StartOffset;
					shaderVariable.ParamterType = uniformVariable.ParamterType;

					variableMaps.emplace(shaderVariableName, shaderVariable);
				}
			}
		}
	}
}