#pragma once
#include "Graphics/ShaderTechnique.h"
#include "Texture.h"
#include "AssetDefines.h"

namespace Dash
{
	class FMaterial
	{
	public:
		struct FShaderPassParameter
		{
			std::map<std::string, std::vector<uint8>> ConstantBufferMap;
			std::map<std::string, FTextureRef> TextureBufferMap;
			FShaderPassRef ShaderPass;
		};

	public:
		FMaterial(const std::string& name, const FShaderTechniqueRef& shaderTechnique);
		~FMaterial();

		bool SetTextureParameter(const std::string& parameterName, const FTextureRef& texture);
		bool SetFloatParameter(const std::string& parameterName, Scalar parameter);
		bool SetVector2Parameter(const std::string& parameterName, const FVector2f& parameter);
		bool SetVector3Parameter(const std::string& parameterName, const FVector3f& parameter);
		bool SetVector4Parameter(const std::string& parameterName, const FVector4f& parameter);
	
		const std::map<std::string, FShaderPassParameter>& GetShaderPassParameters() const { return mShaderPassParametersMap; }

		FShaderTechniqueRef GetShaderTechnique() const { return mShaderTechnique; }

	private:

		struct FConstantBufferVariableInfo
		{
			std::string BufferName;
			UINT StartOffset;
			UINT Size;
		};

		template<typename ParameterType>
		struct FConstantBufferParameterInfo
		{
			std::map<std::string, FConstantBufferVariableInfo> ConstantBufferVariableMap;	// < PassName, VariableInfo >
			ParameterType Parameter;
		};

		struct FTextureParameterInfo
		{
			std::vector<std::string> RelevantPasses;	// < PassName, VariableInfo >
			FTextureRef Parameter;
		};

		void InitData();

	private:
		std::string mName;
		FShaderTechniqueRef mShaderTechnique;

		std::map<std::string, FConstantBufferParameterInfo<Scalar>> mScalarParameterMap;
		std::map<std::string, FConstantBufferParameterInfo<FVector2f>> mVector2ParameterMap;
		std::map<std::string, FConstantBufferParameterInfo<FVector3f>> mVector3ParameterMap;
		std::map<std::string, FConstantBufferParameterInfo<FVector4f>> mVector4ParameterMap;
		std::map<std::string, FTextureParameterInfo> mTextureParameterMap;

		std::map<std::string, FShaderPassParameter> mShaderPassParametersMap;
	};
}