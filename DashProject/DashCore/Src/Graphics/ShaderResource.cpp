#include "PCH.h"
#include "ShaderResource.h"
#include "Utility/StringUtility.h"
#include "Utility/FileUtility.h"

namespace Dash
{
	using namespace Microsoft::WRL;

	#define VERTEX_SHADER_PROFILE "vs"
	#define HULL_SHADER_PROFILE "hs"
	#define DOMAIN_SHADER_PROFILE "ds"
	#define GEOMETRY_SHADER_PROFILE "gs"
	#define PIXEL_SHADER_PROFILE "ps"
	#define COMPUTE_SHADER_PROFILE "cs"


	void FShaderCreationInfo::Finalize()
	{
		std::string hasedName = FileUtility::RemoveExtension(FileName) + "_" + EntryPoint;

		std::sort(Defines.begin(), Defines.end());
		for (const std::string& define : Defines)
		{
			hasedName = hasedName + "_" + define;
		}

		ShaderHash = std::hash<std::string>{}(hasedName);
		HashedFileName = hasedName + "_" + FStringUtility::ToString(ShaderHash);

		ComputeShaderTargetFromEntryPoint();
	}

	bool FShaderCreationInfo::IsOutOfDate() const
	{
		std::string hasedShaderName = GetHashedFileName() + SHADER_BLOB_FILE_EXTENSION;
		std::string reflectionName = GetHashedFileName() + REFLECTION_BLOB_FILE_EXTENSION;
		return !FileUtility::IsPathExistent(hasedShaderName) || !FileUtility::IsPathExistent(reflectionName) || FileUtility::GetFileLastWriteTime(FileName) > FileUtility::GetFileLastWriteTime(hasedShaderName);
	}

	void FShaderCreationInfo::ComputeShaderTargetFromEntryPoint()
	{
		std::vector<std::string> splitStrs = FStringUtility::Split(EntryPoint, "_");

		ASSERT(splitStrs.size() == 2);

		std::string fileProfile = FStringUtility::ToLower(splitStrs[0]);

		if (fileProfile == VERTEX_SHADER_PROFILE)
		{
			Stage = EShaderStage::Vertex;
		}
		else if (fileProfile == HULL_SHADER_PROFILE)
		{
			Stage = EShaderStage::Hull;
		}
		else if (fileProfile == DOMAIN_SHADER_PROFILE)
		{
			Stage = EShaderStage::Domain;
		}
		else if (fileProfile == GEOMETRY_SHADER_PROFILE)
		{
			Stage = EShaderStage::Geometry;
		}
		else if (fileProfile == PIXEL_SHADER_PROFILE)
		{
			Stage = EShaderStage::Pixel;
		}
		else if(fileProfile == COMPUTE_SHADER_PROFILE)
		{
			Stage = EShaderStage::Compute;
		}
		else
		{
			ASSERT_MSG(false, "Invalid Shader Stage!");
		}

		static const std::string targetLevel{ "_6_4" };

		ShaderTarget = FStringUtility::ToLower(splitStrs[0]) + targetLevel;
	}

	void FShaderResource::Init(ComPtr<IDxcBlob> compiledShaderBlob, ComPtr<ID3D12ShaderReflection> reflector, const FShaderCreationInfo& creationInfo)
	{
		ASSERT(compiledShaderBlob != nullptr);

		mCompiledShaderBlob = compiledShaderBlob;
		mShaderBinary.Data = mCompiledShaderBlob->GetBufferPointer();
		mShaderBinary.Size = mCompiledShaderBlob->GetBufferSize();

		mCreationInfo = creationInfo;

		ReflectShaderParameter(reflector);
	}

	void FShaderResource::ReflectShaderParameter(ComPtr<ID3D12ShaderReflection> reflector)
	{
		D3D12_SHADER_DESC shaderDesc;
		reflector->GetDesc(&shaderDesc);

		LOG_INFO << "Num ConstantBuffers : " << shaderDesc.ConstantBuffers;
		LOG_INFO << "Num BoundResources : " << shaderDesc.BoundResources;

		LOG_INFO << " ---------------------------------------------------- ";


		std::map<std::string, UINT> bufferSizeMap;
		for (UINT constantBufferIndex = 0; constantBufferIndex < shaderDesc.ConstantBuffers; ++constantBufferIndex)
		{
			ID3D12ShaderReflectionConstantBuffer* shaderReflectionConstantBuffer = reflector->GetConstantBufferByIndex(constantBufferIndex);

			D3D12_SHADER_BUFFER_DESC bufferDesc;
			shaderReflectionConstantBuffer->GetDesc(&bufferDesc);

			bufferSizeMap.emplace(bufferDesc.Name, bufferDesc.Size);

			LOG_INFO << "Buffer Name : " << std::string(bufferDesc.Name);
			LOG_INFO << "Buffer Type : " << bufferDesc.Type;
			LOG_INFO << "Buffer Variables : " << bufferDesc.Variables;
			LOG_INFO << "Buffer Size : " << bufferDesc.Size;
			LOG_INFO << "Buffer uFlags : " << bufferDesc.uFlags;
		}

		LOG_INFO << " ---------------------------------------------------- ";

		for (UINT resourceIndex = 0; resourceIndex < shaderDesc.BoundResources; ++resourceIndex)
		{
			D3D12_SHADER_INPUT_BIND_DESC resourceDesc;
			reflector->GetResourceBindingDesc(resourceIndex, &resourceDesc);

			FShaderParameter resourceParameter;
			resourceParameter.Name = std::string(resourceDesc.Name);
			resourceParameter.ShaderStage = mCreationInfo.Stage;
			resourceParameter.BindPoint = resourceDesc.BindPoint;
			resourceParameter.RegisterSpace = resourceDesc.Space;
			resourceParameter.ResourceType = resourceDesc.Type;
			resourceParameter.ResourceDimension = resourceDesc.Dimension;
			resourceParameter.BindCount = resourceDesc.BindCount;

			if (bufferSizeMap.contains(resourceParameter.Name))
			{
				resourceParameter.Size = bufferSizeMap[resourceParameter.Name];
			}

			mParameters.push_back(resourceParameter);

			LOG_INFO << "Resource Name : " << std::string(resourceDesc.Name);
			LOG_INFO << "Resource Type : " << resourceDesc.Type;
			LOG_INFO << "Resource BindPoint : " << resourceDesc.BindPoint;
			LOG_INFO << "Resource BindCount : " << resourceDesc.BindCount;
			LOG_INFO << "Resource uFlags : " << resourceDesc.uFlags;
			LOG_INFO << "Resource ReturnType : " << resourceDesc.ReturnType;
			LOG_INFO << "Resource Dimension : " << resourceDesc.Dimension;
			LOG_INFO << "Resource NumSamples : " << resourceDesc.NumSamples;
			LOG_INFO << "Resource Space : " << resourceDesc.Space;
			LOG_INFO << "Resource uID : " << resourceDesc.uID;

			LOG_INFO << " ================================== ";
		}
	}
}