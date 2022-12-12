#include "PCH.h"
#include "ShaderPass.h"
#include "ShaderMap.h"
#include "Utility/StringUtility.h"

namespace Dash
{
	void FShaderPass::SetShader(EShaderStage stage, const FShaderCreationInfo& creationInfo)
	{
		mShaders[stage] = &FShaderMap::LoadShader(creationInfo);
		ASSERT(mShaders[stage] != nullptr);
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
				if (BindPoint == src.RegisterSpace)
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

	void FShaderPass::Finalize(const std::string& passName)
	{
		mPassName = passName;

		std::map<FParameterKey, FShaderParameter> parameterMap;
		
		for (auto& pair : mShaders)
		{
			EShaderStage stage = pair.first;
			FShaderResource* creationInfo = pair.second;

			const std::vector<FShaderParameter>& shaderParameters = creationInfo->GetShaderParameters();
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

		mPassType = mShaders.contains(EShaderStage::Compute) ? EShaderPassType::Compute : EShaderPassType::Raster;

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
				if (!FStringUtility::Contains(pair.second.Name, "StaticSampler"))
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

		CreateRootSignature();
	}

	std::optional<FShaderParameter> FShaderPass::FindCBVParameterByName(const std::string& parameterName) const
	{
		return FindParameterByName(mCBVParameters, parameterName);
	}

	std::optional<FShaderParameter> FShaderPass::FindSRVParameterByName(const std::string& parameterName) const
	{
		return FindParameterByName(mSRVParameters, parameterName);
	}

	std::optional<FShaderParameter> FShaderPass::FindUAVParameterByName(const std::string& parameterName) const
	{
		return FindParameterByName(mUAVParameters, parameterName);
	}

	std::optional<FShaderParameter> FShaderPass::FindSamplerParameterByName(const std::string& parameterName) const
	{
		return FindParameterByName(mSamplerParameters, parameterName);
	}

	bool FShaderPass::IsValid() const
	{
		return mRootSignature.GetSignature() != nullptr;
	}

	void FShaderPass::CreateRootSignature()
	{
		UINT numConstantParameters = static_cast<UINT>(mCBVParameters.size());
		UINT numSRVParameters = static_cast<UINT>(mSRVParameters.size());
		UINT numUAVParameters = static_cast<UINT>(mUAVParameters.size());
		UINT numDynamicSamplerParameters = static_cast<UINT>(mSamplerParameters.size());

		bool hasConstantParameters = numConstantParameters > 0;
		bool hasSRVParameters = numSRVParameters > 0;
		bool hasUAVParameters = numUAVParameters > 0;
		bool hasDynamicSamplerParameters = numDynamicSamplerParameters > 0;

		UINT numRootParameters = numConstantParameters + (hasSRVParameters ? UINT(1) : UINT(0)) + (hasUAVParameters ? UINT(1) : UINT(0)) + (hasDynamicSamplerParameters ? UINT(1) : UINT(0));
		mRootSignature.Reset(2, 8);
		
		uint32_t parameterIndex = 0;
		if (hasConstantParameters)
		{
			for (size_t i = 0; i < numConstantParameters; i++)
			{
				FShaderParameter& cbv = mCBVParameters[i];
				mRootSignature[parameterIndex].InitAsRootConstantBufferView(cbv.BindPoint, GetShaderVisibility(cbv.ShaderStage), cbv.RegisterSpace);
				++parameterIndex;
			}
		}

		if (hasSRVParameters)
		{
			std::sort(mSRVParameters.begin(), mSRVParameters.end(), [](const FShaderParameter& paramA, const FShaderParameter& paramB)
				{
					return paramA.BindPoint < paramB.BindPoint;
				}
			);

			UINT totalSRVCount = 0;
			UINT baseRegister = mSRVParameters[0].BindPoint;
			EShaderStage srvShaderStge = mSRVParameters[0].ShaderStage;
			for (size_t i = 0; i < numSRVParameters; i++)
			{
				FShaderParameter& srv = mSRVParameters[i];
				totalSRVCount += srv.BindCount;
				srvShaderStge |= srv.ShaderStage;
			}

			if (totalSRVCount > 0)
			{
				//only support register space 0
				mRootSignature[parameterIndex].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV, baseRegister, totalSRVCount, GetShaderVisibility(srvShaderStge));
				++parameterIndex;
			}
		}

		if (hasUAVParameters)
		{
			std::sort(mUAVParameters.begin(), mUAVParameters.end(), [](const FShaderParameter& paramA, const FShaderParameter& paramB)
				{
					return paramA.BindPoint < paramB.BindPoint;
				}
			);

			UINT totalUAVCount = 0;
			UINT baseRegister = mUAVParameters[0].BindPoint;
			EShaderStage uavShaderStge = mUAVParameters[0].ShaderStage;
			for (size_t i = 0; i < numUAVParameters; i++)
			{
				FShaderParameter& uav = mUAVParameters[i];
				totalUAVCount += uav.BindCount;
				uavShaderStge |= uav.ShaderStage;
			}

			if (totalUAVCount > 0)
			{
				//only support register space 0
				mRootSignature[parameterIndex].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV, baseRegister, totalUAVCount, GetShaderVisibility(uavShaderStge));
				++parameterIndex;
			}
		}

		if (hasDynamicSamplerParameters)
		{
			std::sort(mSamplerParameters.begin(), mSamplerParameters.end(), [](const FShaderParameter& paramA, const FShaderParameter& paramB)
				{
					return paramA.BindPoint < paramB.BindPoint;
				}
			);

			UINT totalSamplerCount = 0;
			UINT baseRegister = mSamplerParameters[0].BindPoint;
			EShaderStage samplerShaderStge = mSamplerParameters[0].ShaderStage;
			for (size_t i = 0; i < numDynamicSamplerParameters; i++)
			{
				FShaderParameter& uav = mSamplerParameters[i];
				totalSamplerCount += uav.BindCount;
				samplerShaderStge |= uav.ShaderStage;
			}

			if (totalSamplerCount > 0)
			{
				//only support register space 0
				mRootSignature[parameterIndex].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, baseRegister, totalSamplerCount, GetShaderVisibility(samplerShaderStge));
				++parameterIndex;
			}
		}
		 
		std::vector<D3D12_SAMPLER_DESC> staticSamplers = CreateStaticSamplers();

		for (UINT index = 0; index < staticSamplers.size(); index++)
		{
			mRootSignature.InitStaticSampler(index, staticSamplers[index], D3D12_SHADER_VISIBILITY_ALL);
		}
		
		mRootSignature.Finalize(mPassName + "_RootSignature", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
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

	std::vector<D3D12_SAMPLER_DESC> FShaderPass::CreateStaticSamplers()
	{
		// Applications usually only need a handful of samplers.  So just define them all up front
		// and keep them available as part of the root signature.  

		FLOAT borderColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

		D3D12_SAMPLER_DESC pointWrap{};
		pointWrap.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		pointWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		pointWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		pointWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		pointWrap.MipLODBias = 0.0f;
		pointWrap.MaxAnisotropy = 0;
		pointWrap.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		pointWrap.MinLOD = 0.0f;
		pointWrap.MaxLOD = D3D12_FLOAT32_MAX;

		D3D12_SAMPLER_DESC pointClamp{};
		pointClamp.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		pointClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		pointClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		pointClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		pointClamp.MipLODBias = 0.0f;
		pointClamp.MaxAnisotropy = 0;
		pointClamp.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		pointClamp.MinLOD = 0.0f;
		pointClamp.MaxLOD = D3D12_FLOAT32_MAX;

		D3D12_SAMPLER_DESC linearWrap{};
		linearWrap.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		linearWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		linearWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		linearWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		linearWrap.MipLODBias = 0.0f;
		linearWrap.MaxAnisotropy = 0;
		linearWrap.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		linearWrap.MinLOD = 0.0f;
		linearWrap.MaxLOD = D3D12_FLOAT32_MAX;

		D3D12_SAMPLER_DESC linearClamp{};
		linearClamp.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		linearClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		linearClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		linearClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		linearClamp.MipLODBias = 0.0f;
		linearClamp.MaxAnisotropy = 0;
		linearClamp.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		linearClamp.MinLOD = 0.0f;
		linearClamp.MaxLOD = D3D12_FLOAT32_MAX;

		D3D12_SAMPLER_DESC anisotropicWrap{};
		anisotropicWrap.Filter = D3D12_FILTER_ANISOTROPIC;
		anisotropicWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisotropicWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisotropicWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisotropicWrap.MipLODBias = 0.0f;
		anisotropicWrap.MaxAnisotropy = 8;
		anisotropicWrap.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		anisotropicWrap.MinLOD = 0.0f;
		anisotropicWrap.MaxLOD = D3D12_FLOAT32_MAX;

		D3D12_SAMPLER_DESC anisotropicClamp{};
		anisotropicClamp.Filter = D3D12_FILTER_ANISOTROPIC;
		anisotropicClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		anisotropicClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		anisotropicClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		anisotropicClamp.MipLODBias = 0.0f;
		anisotropicClamp.MaxAnisotropy = 8;
		anisotropicClamp.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		anisotropicClamp.MinLOD = 0.0f;
		anisotropicClamp.MaxLOD = D3D12_FLOAT32_MAX;                          

		D3D12_SAMPLER_DESC shadow{};
		shadow.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		shadow.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		shadow.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		shadow.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		shadow.MipLODBias = 0.0f;
		shadow.MaxAnisotropy = 16;
		shadow.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		shadow.MinLOD = 0.0f;
		shadow.MaxLOD = D3D12_FLOAT32_MAX;

		D3D12_SAMPLER_DESC depthMap{};
		depthMap.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		depthMap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		depthMap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		depthMap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		depthMap.MipLODBias = 0.0f;
		depthMap.MaxAnisotropy = 0;
		depthMap.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		depthMap.MinLOD = 0.0f;
		depthMap.MaxLOD = D3D12_FLOAT32_MAX;

		std::vector<D3D12_SAMPLER_DESC> StaticSamplers;
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

	std::optional<FShaderParameter> FShaderPass::FindParameterByName(const std::vector<FShaderParameter>& parameterArray, const std::string& parameterName) const
	{
		for (size_t i = 0; i < parameterArray.size(); i++)
		{
			if (parameterArray[i].Name == parameterName)
			{
				return parameterArray[i];
			}
		}

		return std::nullopt;
	}


}