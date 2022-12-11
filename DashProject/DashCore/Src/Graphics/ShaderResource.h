#pragma once

#include "Utility/FileUtility.h"
#include "Utility/BitwiseEnum.h"
#include <wtypes.h>
#include <Unknwn.h>
#include <wrl.h>
#include "ThirdParty/dxc/inc/dxcapi.h"
#include "ThirdParty/dxc/inc/d3d12shader.h"

namespace Dash
{
	#define SHADER_BLOB_FILE_EXTENSION ".cso"
	#define REFLECTION_BLOB_FILE_EXTENSION ".refect"

	enum class EShaderStage : uint16_t
	{
		Vertex = 0,
		Hull = 1, 
		Domain = 2,
		Geometry = 3,
		Pixel = 4,
		Compute = 5,
	};
	ENABLE_BITMASK_OPERATORS(EShaderStage);

	struct FShaderCreationInfo
	{
	public:
		FShaderCreationInfo()
			: Stage(EShaderStage::Vertex)
		{}

		FShaderCreationInfo(const std::string& fileName, const std::string& entryPoint, const std::vector<std::string>& defines = {})
			: FileName(fileName)
			, EntryPoint(entryPoint)
			, Defines(defines)
			, Stage(EShaderStage::Vertex)
		{}

		void Finalize();
		size_t GetShaderHash() const { return ShaderHash; }
		std::string GetShaderTarget() const { return ShaderTarget; }
		std::string GetHashedFileName() const { return HashedFileName; }
		bool IsOutOfDate() const;

		std::string FileName;
		std::string EntryPoint;
		std::vector<std::string> Defines;
		EShaderStage Stage{EShaderStage::Vertex};

	protected:
		void ComputeShaderTargetFromEntryPoint();

	protected:
		std::string ShaderTarget;
		std::string HashedFileName;
		size_t ShaderHash = 0;
	};

	struct FDX12CompiledShader
	{
		Microsoft::WRL::ComPtr<IDxcBlob> CompiledShaderBlob = nullptr;
		Microsoft::WRL::ComPtr<IDxcBlob> ShaderRelectionBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3D12ShaderReflection> ShaderReflector = nullptr;

		bool IsValid() const
		{
			return CompiledShaderBlob != nullptr && ShaderReflector !=nullptr;
		}
	};

	struct FCompiledBinary
	{
		void* Data = nullptr;
		uint64_t Size = 0;
	};

	struct FShaderParameter
	{
		std::string Name;
		EShaderStage ShaderStage;
		D3D_SHADER_INPUT_TYPE ResourceType;
		UINT BindPoint = 0;
		UINT RegisterSpace = 0;
		UINT Size = 0;
	};

	struct FShaderResource
	{
	public:
		friend class FShaderMap;

		FCompiledBinary GetCompiledShader() const { return mShaderBinary; }
		std::string GetShaderFileName() const { return mCreationInfo.FileName; }
		std::string GetShaderEntryPoint() const { return mCreationInfo.EntryPoint; }
		EShaderStage GetShaderStage() const { return mCreationInfo.Stage; }
		size_t GetShaderHash() const { return mCreationInfo.GetShaderHash(); }
		const std::vector<FShaderParameter>& GetShaderParameters() const { return mParameters; }

	protected:
		void Init(Microsoft::WRL::ComPtr<IDxcBlob> compiledShaderBlob, Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflector, const FShaderCreationInfo& creationInfo);
		void ReflectShaderParameter(Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflector);

		FShaderCreationInfo mCreationInfo;
		Microsoft::WRL::ComPtr<IDxcBlob> mCompiledShaderBlob;
		FCompiledBinary mShaderBinary;
		std::vector<FShaderParameter> mParameters;
	}; 
}