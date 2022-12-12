#pragma once
#include "ShaderResource.h"
#include "RootSignature.h"

namespace Dash
{
	enum class EShaderPassType
	{
		Raster,
		Compute
	};

	class FShaderPass
	{
	public:
		void SetShader(EShaderStage stage, const FShaderCreationInfo& creationInfo);
		void SetPassName(const std::string& name) { mPassName = name; }
		void Finalize(const std::string& passName);

		EShaderPassType GetPassType() const { return mPassType; }
		const FRootSignature& GetRootSignature() const { return mRootSignature; }
		const std::map<EShaderStage, FShaderResource*>& GetShaders() const { return mShaders; }
		const std::string& GetPassName() const { return mPassName; }
		std::optional<FShaderParameter> FindCBVParameterByName(const std::string& parameterName) const;
		std::optional<FShaderParameter> FindSRVParameterByName(const std::string& parameterName) const;
		std::optional<FShaderParameter> FindUAVParameterByName(const std::string& parameterName) const;
		std::optional<FShaderParameter> FindSamplerParameterByName(const std::string& parameterName) const;		
		bool IsValid() const;

	protected:
		void CreateRootSignature();
		D3D12_SHADER_VISIBILITY GetShaderVisibility(EShaderStage stage);
		std::vector<D3D12_SAMPLER_DESC> CreateStaticSamplers();
		std::optional<FShaderParameter> FindParameterByName(const std::vector<FShaderParameter>& parameterArray, const std::string& parameterName) const;

	private:
		std::map<EShaderStage, FShaderResource*> mShaders;

		std::vector<FShaderParameter> mCBVParameters;
		std::vector<FShaderParameter> mSRVParameters;
		std::vector<FShaderParameter> mUAVParameters;
		std::vector<FShaderParameter> mSamplerParameters;

		EShaderPassType mPassType;
		FRootSignature mRootSignature;
		std::string mPassName;
	};
}