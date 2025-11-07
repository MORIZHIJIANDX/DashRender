#pragma once

#include "GraphicsDefines.h"
#include <wtypes.h>
#include <Unknwn.h>
#include "InputAssemblerLayout.h"
#include "dxc/inc/dxcapi.h"
#include "dxc/inc/d3d12shader.h"
#include "Utility/RefCounting.h"
#include "Utility/ThreadSafeCounter.h"

namespace Dash
{
	struct FShaderCreationInfo
	{
	public:
		FShaderCreationInfo()
			: Stage(EShaderStage::Vertex)
		{}

		FShaderCreationInfo(EShaderStage shaderStage, const std::string& fileName, const std::string& entryPoint, const std::vector<std::string>& defines = {})
			: FileName(fileName)
			, EntryPoint(entryPoint)
			, Defines(defines)
			, Stage(shaderStage)
		{
			Finalize();
		}

		void Finalize();
		size_t GetShaderHash() const { return ShaderHash; }
		std::string GetShaderTarget() const { return ShaderTarget; }
		std::string GetHashedFileName() const { return HashedFileName; }
		bool IsOutOfDate() const;

		std::string FileName;
		std::string EntryPoint;
		std::vector<std::string> Defines;
		EShaderStage Stage;

	protected:
		void ComputeShaderTargetFromEntryPoint();

	protected:
		std::string ShaderTarget;
		std::string HashedFileName;
		size_t ShaderHash = 0;
	};

	struct FDX12CompiledShader
	{
		TRefCountPtr<IDxcBlob> CompiledShaderBlob = nullptr;
		TRefCountPtr<IDxcBlob> ShaderRelectionBlob = nullptr;
		TRefCountPtr<ID3D12ShaderReflection> ShaderReflector = nullptr;
		std::map<std::string, std::string> BindlessResourceMap;

		bool IsValid() const
		{
			return CompiledShaderBlob != nullptr && ShaderReflector !=nullptr;
		}
	};

	struct FCompiledBinary
	{
		void* Data = nullptr;
		uint32 Size = 0;
	};

	struct FConstantBufferVariable
	{
		std::string VariableName;
		EShaderParameterType ParamterType = EShaderParameterType::Invalid;
		uint32 StartOffset;
		uint32 Size;
	};

	struct FShaderParameter
	{
		std::string Name;
		EShaderStage ShaderStage = EShaderStage::Vertex;
		D3D_SHADER_INPUT_TYPE ShaderInputType = D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER;
		EShaderResourceBindingType BindingType = EShaderResourceBindingType::Invalid;
		EShaderParameterType ParameterType = EShaderParameterType::Invalid;
		D3D_SRV_DIMENSION ResourceDimension = D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_BUFFER;
		uint32 BindPoint = 0;
		uint32 RegisterSpace = 0;
		uint32 Size = 0;
		uint32 BindCount = 0;
		uint32 Stride = 0;
		uint32 RootParameterIndex = 0;
		uint32 DescriptorOffset = 0;
		std::vector<FConstantBufferVariable> ConstantBufferVariables;
	};

	struct FShaderResource : public FRefCount
	{
	public:
		friend class FShaderMap;

		FCompiledBinary GetCompiledShader() const { return mShaderBinary; }
		std::string GetShaderFileName() const { return mCreationInfo.FileName; }
		std::string GetShaderEntryPoint() const { return mCreationInfo.EntryPoint; }
		std::string GetHashedFileName() const { return mCreationInfo.GetHashedFileName(); }
		EShaderStage GetShaderStage() const { return mCreationInfo.Stage; }
		size_t GetShaderHash() const { return mCreationInfo.GetShaderHash(); }
		const std::vector<FShaderParameter>& GetCBVParameters() const { return mCBVParameters; }
		const std::vector<FShaderParameter>& GetSRVParameters() const { return mSRVParameters; }
		const std::vector<FShaderParameter>& GetUAVParameters() const { return mUAVParameters; }
		const std::vector<FShaderParameter>& GetSamplerParameters() const { return mSamplerParameters; }
		const FInputAssemblerLayout& GetInputLayout() const { return mInputLayout; }

	protected:
		void Init(const FDX12CompiledShader& compiledShader, const FShaderCreationInfo& creationInfo);
		void ReflectShaderParameter(const TRefCountPtr<ID3D12ShaderReflection>& reflector);
		void ReflectInputLayout(const D3D12_SHADER_DESC& shaderDesc, const TRefCountPtr<ID3D12ShaderReflection>& reflector);
		void ReflectCBVParamter(FShaderParameter& parameter, const D3D12_SHADER_INPUT_BIND_DESC& resourceDesc, const TRefCountPtr<ID3D12ShaderReflection>& reflector);
		void ReflectSRVParamter(FShaderParameter& parameter, const D3D12_SHADER_INPUT_BIND_DESC& resourceDesc, const TRefCountPtr<ID3D12ShaderReflection>& reflector);
		void ReflectUAVParamter(FShaderParameter& parameter, const D3D12_SHADER_INPUT_BIND_DESC& resourceDesc, const TRefCountPtr<ID3D12ShaderReflection>& reflector);
		void ReflectSamplerParamter(FShaderParameter& parameter, const D3D12_SHADER_INPUT_BIND_DESC& resourceDesc, const TRefCountPtr<ID3D12ShaderReflection>& reflector);
		bool IsSystemGeneratedValues(const std::string& semantic) const;

		void SortParameters();
		void SortParameter(std::vector<FShaderParameter>& parameters);

		FShaderCreationInfo mCreationInfo;
		TRefCountPtr<IDxcBlob> mCompiledShaderBlob;
		FCompiledBinary mShaderBinary;
		
		std::vector<FShaderParameter> mCBVParameters;
		std::vector<FShaderParameter> mSRVParameters;
		std::vector<FShaderParameter> mUAVParameters;
		std::vector<FShaderParameter> mSamplerParameters;

		FInputAssemblerLayout mInputLayout;
	}; 
}