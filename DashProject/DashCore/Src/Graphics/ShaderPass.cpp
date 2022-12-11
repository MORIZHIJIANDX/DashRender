#include "PCH.h"
#include "ShaderPass.h"
#include "ShaderMap.h"

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

	void FShaderPass::Finalize()
	{
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

				//FShaderParameter& passParameter = parameterMap[key];

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
			mPassParameters.push_back(pair.second);

			LOG_INFO << " =============== ";

			LOG_INFO << "Resource Name : " << pair.second.Name;
			LOG_INFO << "Resource Type : " << pair.second.ResourceType;
			LOG_INFO << "Resource BindPoint : " << pair.second.BindPoint;
			LOG_INFO << "Resource Space : " << pair.second.RegisterSpace;
			LOG_INFO << "Resource Shader Stage : " << (UINT)pair.second.ShaderStage;
		}
	}
}