#pragma once
#include "ShaderResource.h"
#include "RootSignature.h"
#include "SamplerDesc.h"

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
		static FShaderPassRef MakeShaderPass();

		void SetShader(EShaderStage stage, const FShaderCreationInfo& creationInfo);
		void SetPassName(const std::string& name) { mPassName = name; }
		void Finalize(const std::string& passName, bool createStaticSamplers = true);

		EShaderPassType GetPassType() const { return mPassType; }
		const std::string& GetPassName() const { return mPassName; }
		FRootSignatureRef GetRootSignature() const { return mRootSignatureRef; }
		const std::map<EShaderStage, FShaderResourceRef>& GetShaders() const { return mShaders; }

		int32_t FindCBVParameterByName(const std::string& parameterName) const;
		int32_t FindSRVParameterByName(const std::string& parameterName) const;
		int32_t FindUAVParameterByName(const std::string& parameterName) const;
		int32_t FindSamplerParameterByName(const std::string& parameterName) const;

		size_t GetCBVParameterNum() const { return mCBVParameters.size(); }
		size_t GetSRVParameterNum() const { return mSRVParameters.size(); }
		size_t GetUAVParameterNum() const { return mUAVParameters.size(); }
		size_t GetSamplerParameterNum() const { return mSamplerParameters.size(); }

		const std::vector<FShaderParameter>& GetCBVParameters() const { return mCBVParameters; }
		const std::vector<FShaderParameter>& GetSRVParameters() const { return mSRVParameters; }
		const std::vector<FShaderParameter>& GetUAVParameters() const { return mUAVParameters; }
		const std::vector<FShaderParameter>& GetSamplerParameters() const { return mSamplerParameters; }

		bool IsFinalized() const;

	protected:
		void CreateRootSignature(bool createStaticSamplers);
		D3D12_SHADER_VISIBILITY GetShaderVisibility(EShaderStage stage);
		std::vector<FSamplerDesc> CreateStaticSamplers();
		int32_t FindParameterByName(const std::vector<FShaderParameter>& parameterArray, const std::string& parameterName) const;
		std::vector<std::string> GetParameterNames(const std::vector<FShaderParameter>& parameterArray) const;
		void InitDescriptorRanges(std::vector<FShaderParameter>& parameters, UINT& rootParameterIndex, D3D12_DESCRIPTOR_RANGE_TYPE rangeType);

	private:
		std::map<EShaderStage, FShaderResourceRef> mShaders;

		std::vector<FShaderParameter> mCBVParameters;
		std::vector<FShaderParameter> mSRVParameters;
		std::vector<FShaderParameter> mUAVParameters;
		std::vector<FShaderParameter> mSamplerParameters;

		EShaderPassType mPassType;
		FRootSignatureRef mRootSignatureRef;
		std::string mPassName;
	};
}