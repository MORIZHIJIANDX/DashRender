#pragma once

#include "Utility/BitwiseEnum.h"
#include <wtypes.h>
#include <Unknwn.h>
#include "InputAssemblerLayout.h"
#include "dxc/inc/dxcapi.h"
#include "dxc/inc/d3d12shader.h"

namespace Dash
{
	#define SHADER_BLOB_FILE_EXTENSION ".cso"
	#define REFLECTION_BLOB_FILE_EXTENSION ".refect"
	#define PDB_BLOB_FILE_EXTENSION ".pdb"

	enum class EShaderStage : uint16_t
	{
		Vertex = 1 << 1,
		Hull = 1 << 2,
		Domain = 1 << 3,
		Geometry = 1 << 4,
		Pixel = 1 << 5,
		Compute = 1 << 6,
	};
	ENABLE_BITMASK_OPERATORS(EShaderStage);

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
		uint32_t Size = 0;
	};

	struct FConstantBufferVariable
	{
		std::string VariableName;
		UINT StartOffset;
		UINT Size;
	};

	struct FShaderParameter
	{
		std::string Name;
		EShaderStage ShaderStage = EShaderStage::Vertex;
		D3D_SHADER_INPUT_TYPE ResourceType = D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER;
		D3D_SRV_DIMENSION ResourceDimension = D3D_SRV_DIMENSION::D3D_SRV_DIMENSION_BUFFER;
		UINT BindPoint = 0;
		UINT RegisterSpace = 0;
		UINT Size = 0;
		UINT BindCount = 0;
		UINT RootParameterIndex = 0;
		UINT DescriptorOffset = 0;
		std::vector<FConstantBufferVariable> ConstantBufferVariables;
	};

	struct FShaderResource
	{
	public:
		friend class FShaderMap;

		FCompiledBinary GetCompiledShader() const { return mShaderBinary; }
		std::string GetShaderFileName() const { return mCreationInfo.FileName; }
		std::string GetShaderEntryPoint() const { return mCreationInfo.EntryPoint; }
		std::string GetHashedFileName() const { return mCreationInfo.GetHashedFileName(); }
		EShaderStage GetShaderStage() const { return mCreationInfo.Stage; }
		size_t GetShaderHash() const { return mCreationInfo.GetShaderHash(); }
		const std::vector<FShaderParameter>& GetShaderParameters() const { return mParameters; }
		const FInputAssemblerLayout& GetInputLayout() const { return mInputLayout; }

	protected:
		void Init(Microsoft::WRL::ComPtr<IDxcBlob> compiledShaderBlob, Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflector, const FShaderCreationInfo& creationInfo);
		void ReflectShaderParameter(Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflector);
		bool IsSystemGeneratedValues(const std::string& semantic) const;

		FShaderCreationInfo mCreationInfo;
		Microsoft::WRL::ComPtr<IDxcBlob> mCompiledShaderBlob;
		FCompiledBinary mShaderBinary;
		std::vector<FShaderParameter> mParameters;
		FInputAssemblerLayout mInputLayout;
	}; 

	using FShaderResourceRef = std::shared_ptr<FShaderResource>;
}