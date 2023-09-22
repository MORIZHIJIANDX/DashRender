#include "PCH.h"
#include "Material.h"
#include "Utility/StringUtility.h"
#include "AssetManager.h"

namespace Dash
{
	FMaterial::FMaterial(const std::string& name, FShaderTechniqueRef shaderTechnique)
		: mName(name)
		, mShaderTechnique(shaderTechnique)
	{
		const std::vector<FShaderPassRef>& shaderPass = shaderTechnique->GetPasses();
		for (auto& pass : shaderPass)
		{
			const std::string passName = pass->GetPassName();
			FShaderPassParameter& PassParameter = mShaderPassParametersMap[passName];
			PassParameter.ShaderPass = pass;

			const std::vector<FShaderParameter>& cbvParameters = pass->GetCBVParameters();
			for (auto& parameter : cbvParameters)
			{
				if (FStringUtility::Contains(parameter.Name, "FrameConstantBuffer"))
				{
					continue;
				}

				PassParameter.ConstantBufferMap[parameter.Name] = std::make_shared<std::vector<uint8_t>>(parameter.Size);

				for (size_t variableIndex = 0; variableIndex < parameter.ConstantBufferVariables.size(); variableIndex++)
				{
					const FConstantBufferVariable& constantVariable = parameter.ConstantBufferVariables[variableIndex];

					FConstantBufferVariableInfo variableInfo;
					variableInfo.BufferName = parameter.Name;
					variableInfo.Size = constantVariable.Size;
					variableInfo.StartOffset = constantVariable.StartOffset;

					if (constantVariable.Size == sizeof(Scalar))
					{
						mScalarParameterMap[constantVariable.VariableName].ConstantBufferVariableMap.emplace(passName, variableInfo);
					}
					else if (constantVariable.Size == sizeof(FVector2f))
					{
						mVector2ParameterMap[constantVariable.VariableName].ConstantBufferVariableMap.emplace(passName, variableInfo);
					}
					else if (constantVariable.Size == sizeof(FVector3f))
					{
						mVector3ParameterMap[constantVariable.VariableName].ConstantBufferVariableMap.emplace(passName, variableInfo);
					}
					else if (constantVariable.Size == sizeof(FVector4f))
					{
						mVector4ParameterMap[constantVariable.VariableName].ConstantBufferVariableMap.emplace(passName, variableInfo);
					}
					else
					{
						ASSERT_FAIL("Invalid Constant Variable Type!");
					}
				}
			}

			const std::vector<FShaderParameter>& srvParameters = pass->GetSRVParameters();
			for (auto& parameter : srvParameters)
			{
				PassParameter.TextureBufferMap.emplace(parameter.Name, nullptr);
				mTextureParameterMap[parameter.Name].RelevantPasses.push_back(passName);
			}
		}

		InitData();
	}

	FMaterial::~FMaterial()
	{
	}

	bool FMaterial::SetTextureParameter(const std::string& parameterName, const FTextureRef& texture)
	{
		if (mTextureParameterMap.contains(parameterName))
		{
			mTextureParameterMap[parameterName].Parameter = texture;

			for (auto& passName : mTextureParameterMap[parameterName].RelevantPasses)
			{
				mShaderPassParametersMap[passName].TextureBufferMap[parameterName] = texture;
			}

			return true;
		}

		return false;
	}

	bool FMaterial::SetFloatParameter(const std::string& parameterName, Scalar parameter)
	{
		if (mScalarParameterMap.contains(parameterName))
		{
			mScalarParameterMap[parameterName].Parameter = parameter;
			for (auto& variableInfo : mScalarParameterMap[parameterName].ConstantBufferVariableMap)
			{	
				Scalar* variable = reinterpret_cast<Scalar*>(mShaderPassParametersMap[variableInfo.first].ConstantBufferMap[variableInfo.second.BufferName]->data() + variableInfo.second.StartOffset);
				*variable = parameter;
			}

			return true;
		}

		return false;
	}

	bool FMaterial::SetVector2Parameter(const std::string& parameterName, const FVector2f& parameter)
	{
		if (mVector2ParameterMap.contains(parameterName))
		{
			mVector2ParameterMap[parameterName].Parameter = parameter;
			for (auto& variableInfo : mVector2ParameterMap[parameterName].ConstantBufferVariableMap)
			{
				FVector2f* variable = reinterpret_cast<FVector2f*>(mShaderPassParametersMap[variableInfo.first].ConstantBufferMap[variableInfo.second.BufferName]->data() + variableInfo.second.StartOffset);
				*variable = parameter;
			}

			return true;
		}

		return false;
	}

	bool FMaterial::SetVector3Parameter(const std::string& parameterName, const FVector3f& parameter)
	{
		if (mVector3ParameterMap.contains(parameterName))
		{
			mVector3ParameterMap[parameterName].Parameter = parameter;
			for (auto& variableInfo : mVector3ParameterMap[parameterName].ConstantBufferVariableMap)
			{
				FVector3f* variable = reinterpret_cast<FVector3f*>(mShaderPassParametersMap[variableInfo.first].ConstantBufferMap[variableInfo.second.BufferName]->data() + variableInfo.second.StartOffset);
				*variable = parameter;
			}

			return true;
		}

		return false;
	}

	bool FMaterial::SetVector4Parameter(const std::string& parameterName, const FVector4f& parameter)
	{
		if (mVector4ParameterMap.contains(parameterName))
		{
			mVector4ParameterMap[parameterName].Parameter = parameter;
			for (auto& variableInfo : mVector4ParameterMap[parameterName].ConstantBufferVariableMap)
			{
				FVector4f* variable = reinterpret_cast<FVector4f*>(mShaderPassParametersMap[variableInfo.first].ConstantBufferMap[variableInfo.second.BufferName]->data() + variableInfo.second.StartOffset);
				*variable = parameter;
			}

			return true;
		}

		return false;
	}

	void FMaterial::InitData()
	{
		for (auto& parameter : mScalarParameterMap)
		{
			parameter.second.Parameter = 0.0f;
		}

		for (auto& parameter : mVector2ParameterMap)
		{
			parameter.second.Parameter = FVector2f(0.0f, 0.0f);
		}

		for (auto& parameter : mVector3ParameterMap)
		{
			parameter.second.Parameter = FVector3f(0.0f, 0.0f, 0.0f);
		}

		for (auto& parameter : mVector4ParameterMap)
		{
			parameter.second.Parameter = FVector4f(0.0f, 0.0f, 0.0f, 0.0f);
		}

		for (auto& parameter : mTextureParameterMap)
		{
			parameter.second.Parameter = FAssetManager::Get().MakeTexture("Black"); 
		}

		for (auto& parameter : mShaderPassParametersMap)
		{
			for (auto& buffer : parameter.second.ConstantBufferMap)
			{
				std::memset(buffer.second->data(), 0, sizeof(buffer.second->size()));
			}
		}
	}
}