#include "PCH.h"
#include "ShaderPass.h"
#include "ShaderMap.h"
#include "Utility/StringUtility.h"

namespace Dash
{
	FShaderPassRef FShaderPass::MakeShaderPass(const std::string& passName, const std::vector<FShaderCreationInfo>& creationInfos,
		const FBlendState& blendState, const FRasterizerState& rasterizerState, const FDepthStencilState& depthStencilState)
	{
		FShaderPassRef newPass = std::make_shared<FShaderPass>();
		newPass->SetPassName(passName);
		newPass->SetShaders(creationInfos);
		newPass->SetBlendState(blendState);
		newPass->SetRasterizerState(rasterizerState);
		newPass->SetDepthStencilState(depthStencilState);

		newPass->Finalize();

		return newPass;
	}

	const FInputAssemblerLayout& FShaderPass::GetInputLayout() const
	{
		ASSERT(mShaders.contains(EShaderStage::Vertex));
		return mShaders.at(EShaderStage::Vertex)->GetInputLayout();
	}

	void FShaderPass::SetShaders(const std::vector<FShaderCreationInfo>& creationInfos)
	{
		std::set<EShaderStage> validShaderStages;
		for (int32_t i = 0; i < creationInfos.size(); i++)
		{
			validShaderStages.insert(creationInfos[i].Stage);
		}
		
		ASSERT_MSG(validShaderStages.size() == creationInfos.size(), "Set duplicates shader stages.");

		for (int32_t i = 0; i < creationInfos.size(); i++)
		{
			EShaderStage stage = creationInfos[i].Stage;
			mShaders[stage] = FShaderMap::LoadShader(creationInfos[i]);
			ASSERT(mShaders[stage] != nullptr);
		}
	}

	struct FParameterKey
	{
		D3D_SHADER_INPUT_TYPE ResourceType;
		UINT BindPoint = 0;
		UINT RegisterSpace = 0;
		D3D_SRV_DIMENSION ResourceDimension;

		bool operator<(const FParameterKey& src)const
		{
			if (ResourceType == src.ResourceType)
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
			
			return ResourceType < src.ResourceType;
		}
	};

	void FShaderPass::Finalize(bool createStaticSamplers)
	{
		mPassType = mShaders.contains(EShaderStage::Compute) ? EShaderPassType::Compute : EShaderPassType::Raster;

		std::map<FParameterKey, FShaderParameter> parameterMap;
		
		for (auto& pair : mShaders)
		{
			EShaderStage stage = pair.first;
			FShaderResourceRef shaderRef = pair.second;

			const std::vector<FShaderParameter>& shaderParameters = shaderRef->GetShaderParameters();
			for (UINT parameterIndex = 0; parameterIndex < shaderParameters.size(); ++parameterIndex)
			{
				const FShaderParameter& shaderParameter = shaderParameters[parameterIndex];

				FParameterKey key{ shaderParameter.ResourceType, shaderParameter.BindPoint, shaderParameter.RegisterSpace };

				if (parameterMap.contains(key))
				{
					ASSERT(parameterMap[key].Name == shaderParameter.Name);
					ASSERT(parameterMap[key].Size == shaderParameter.Size);
					parameterMap[key].ShaderStage |= shaderParameter.ShaderStage;
				}
				else
				{
					parameterMap[key] = shaderParameter;
				}
			}
		}

		LOG_INFO << "Num Pass Parameters : " << parameterMap.size();

		for (auto& pair : parameterMap)
		{
			D3D_SHADER_INPUT_TYPE resourceType = pair.second.ResourceType;

			switch (resourceType)
			{
			case D3D_SHADER_INPUT_TYPE::D3D10_SIT_CBUFFER:
			{
				mCBVParameters.push_back(pair.second);
				LOG_INFO << "Add CBV Pass Parameter : " << pair.second.Name;
				break;
			}
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_TBUFFER:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_BYTEADDRESS:
			{
				mSRVParameters.push_back(pair.second);
				LOG_INFO << "Add SRV Pass Parameter : " << pair.second.Name;
				break;
			}
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWBYTEADDRESS:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_APPEND_STRUCTURED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_CONSUME_STRUCTURED:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_FEEDBACKTEXTURE:
			{
				mUAVParameters.push_back(pair.second);
				LOG_INFO << "Add UAV Pass Parameter : " << pair.second.Name;
				break;
			}
			case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:
			{
				if (!FStringUtility::Contains(pair.second.Name, "_Static"))
				{
					mSamplerParameters.push_back(pair.second);
					LOG_INFO << "Add Sampler Pass Parameter : " << pair.second.Name;
				}
				break;
			}
			default:
				ASSERT_MSG(false, "Invalid shader resource type!");
				break;
			}
		}

		CreateRootSignature(createStaticSamplers);
	}

	int32_t FShaderPass::FindCBVParameterByName(const std::string& parameterName) const
	{
		return FindParameterByName(mCBVParameters, parameterName);
	}

	int32_t FShaderPass::FindSRVParameterByName(const std::string& parameterName) const
	{
		return FindParameterByName(mSRVParameters, parameterName);
	}

	int32_t FShaderPass::FindUAVParameterByName(const std::string& parameterName) const
	{
		return FindParameterByName(mUAVParameters, parameterName);
	}

	int32_t FShaderPass::FindSamplerParameterByName(const std::string& parameterName) const
	{
		return FindParameterByName(mSamplerParameters, parameterName);
	}

	void FShaderPass::CreateRootSignature(bool createStaticSamplers)
	{
		mRootSignatureRef = FRootSignature::MakeRootSignature();

		UINT numConstantParameters = static_cast<UINT>(mCBVParameters.size());
		UINT numSRVParameters = static_cast<UINT>(mSRVParameters.size());
		UINT numUAVParameters = static_cast<UINT>(mUAVParameters.size());
		UINT numDynamicSamplerParameters = static_cast<UINT>(mSamplerParameters.size());

		bool hasConstantParameters = numConstantParameters > 0;
		bool hasSRVParameters = numSRVParameters > 0;
		bool hasUAVParameters = numUAVParameters > 0;
		bool hasDynamicSamplerParameters = numDynamicSamplerParameters > 0;

		UINT numRootParameters = numConstantParameters + (hasSRVParameters ? UINT(1) : UINT(0)) + (hasUAVParameters ? UINT(1) : UINT(0)) + (hasDynamicSamplerParameters ? UINT(1) : UINT(0));
		mRootSignatureRef->Reset(numRootParameters, createStaticSamplers ? 8 : 0);
		
		uint32_t rootParameterIndex = 0;
		if (hasConstantParameters)
		{
			std::sort(mCBVParameters.begin(), mCBVParameters.end(), [](const FShaderParameter& paramA, const FShaderParameter& paramB)
				{
					return paramA.BindPoint < paramB.BindPoint;
				}
			);

			for (size_t i = 0; i < numConstantParameters; i++)
			{
				FShaderParameter& cbv = mCBVParameters[i];
				cbv.RootParameterIndex = rootParameterIndex;
				(*mRootSignatureRef)[rootParameterIndex].InitAsRootConstantBufferView(cbv.BindPoint, GetShaderVisibility(cbv.ShaderStage), cbv.RegisterSpace);
				++rootParameterIndex;
			}
		}

		InitDescriptorRanges(mSRVParameters, rootParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
		InitDescriptorRanges(mUAVParameters, rootParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV);
		InitDescriptorRanges(mSamplerParameters, rootParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER);
		 
		if (createStaticSamplers)
		{
			std::vector<FSamplerDesc> staticSamplers = CreateStaticSamplers();

			for (UINT index = 0; index < staticSamplers.size(); index++)
			{
				mRootSignatureRef->InitStaticSampler(index, staticSamplers[index], D3D12_SHADER_VISIBILITY_ALL);
			}
		}
		
		mRootSignatureRef->Finalize(mPassName + "_RootSignature", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
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

	int32_t FShaderPass::FindParameterByName(const std::vector<FShaderParameter>& parameterArray, const std::string& parameterName) const
	{
		for (int32_t i = 0; i < parameterArray.size(); i++)
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

	void FShaderPass::InitDescriptorRanges(std::vector<FShaderParameter>& parameters, UINT& rootParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE rangeType)
	{
		size_t parametersCount = parameters.size();
		if (parametersCount > 0)
		{
			std::sort(parameters.begin(), parameters.end(), [](const FShaderParameter& paramA, const FShaderParameter& paramB)
				{
					return paramA.BindPoint < paramB.BindPoint;
				}
			);

			std::vector<std::pair<size_t, size_t>> ranges;

			EShaderStage shaderStages = parameters[0].ShaderStage;
			size_t startParameterIndex = 0;
			while (startParameterIndex < parametersCount)
			{
				for (size_t parameterIndex = startParameterIndex; parameterIndex < parametersCount; parameterIndex++)
				{
					if (parameterIndex + 1 < parametersCount)
					{
						if ((parameters[parameterIndex + 1].BindPoint != (parameters[parameterIndex].BindPoint + 1)) ||
							(parameters[parameterIndex + 1].RegisterSpace != parameters[parameterIndex].RegisterSpace))
						{
							ranges.push_back(std::pair(startParameterIndex, parameterIndex));
							startParameterIndex = parameterIndex + 1;
						}
					}
					else
					{
						ranges.push_back(std::pair(startParameterIndex, parameterIndex));
						startParameterIndex = parameterIndex + 1;
					}
					shaderStages |= parameters[parameterIndex].ShaderStage;
				}
			}

			UINT rangeCount = static_cast<UINT>(ranges.size());
			if (rangeCount > 0)
			{
				(*mRootSignatureRef)[rootParameterIndex].InitAsDescriptorTable(rangeCount, GetShaderVisibility(shaderStages));

				UINT parameterDescriptorOffset = 0;
				for (size_t rangeIndex = 0; rangeIndex < rangeCount; rangeIndex++)
				{
					size_t startShaderParameterIndex = ranges[rangeIndex].first;
					size_t endShaderParameterIndex = ranges[rangeIndex].second;
					UINT baseShaderRegister = parameters[startShaderParameterIndex].BindPoint;
					UINT registerSpace = parameters[startShaderParameterIndex].RegisterSpace;
					UINT descriptorCountInRange = 0;
					for (size_t parameterIndex = startShaderParameterIndex; parameterIndex <= endShaderParameterIndex; parameterIndex++)
					{
						parameters[parameterIndex].RootParameterIndex = rootParameterIndex;
						parameters[parameterIndex].DescriptorOffset = parameterDescriptorOffset;	//offset in parameter
						descriptorCountInRange += parameters[parameterIndex].BindCount;
						parameterDescriptorOffset += parameters[parameterIndex].BindCount;
					}
					(*mRootSignatureRef)[rootParameterIndex].SetTableRange(static_cast<UINT>(rangeIndex), rangeType, baseShaderRegister, descriptorCountInRange, registerSpace);
				}
				++rootParameterIndex;
			}
		}
	}
}