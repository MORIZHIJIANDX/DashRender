#include "PCH.h"
#include "ShaderResource.h"
#include "Utility/StringUtility.h"
#include "Utility/FileUtility.h"
#include "ResourceFormat.h"

namespace Dash
{
	using namespace Microsoft::WRL;

	#define VERTEX_SHADER_PROFILE "vs"
	#define HULL_SHADER_PROFILE "hs"
	#define DOMAIN_SHADER_PROFILE "ds"
	#define GEOMETRY_SHADER_PROFILE "gs"
	#define PIXEL_SHADER_PROFILE "ps"
	#define COMPUTE_SHADER_PROFILE "cs"

	#define SHADER_BLOB_PATH "ShaderBlob"

	static constexpr const char* kBindlessSRVPrefix = TEXT("BindlessSRV_");
	static constexpr const char* kBindlessUAVPrefix = TEXT("BindlessUAV_");
	static constexpr const char* kBindlessSamplerPrefix = TEXT("BindlessSampler_");

	EShaderParameterType ParseAndRemoveBindlessParameterPrefix(std::string& memberName)
	{
		if (FStringUtility::ReplaceFirst(memberName, kBindlessSRVPrefix, ""))
		{
			return EShaderParameterType::BindlessSRV;
		}

		if (FStringUtility::ReplaceFirst(memberName, kBindlessUAVPrefix, ""))
		{
			return EShaderParameterType::BindlessUAV;
		}
		
		if (FStringUtility::ReplaceFirst(memberName, kBindlessSamplerPrefix, ""))
		{
			return EShaderParameterType::BindlessSampler;
		}

		return EShaderParameterType::LooseData;
	}

	void FShaderCreationInfo::Finalize()
	{
		std::string blobFileName = FFileUtility::CombinePath(FFileUtility::CombinePath(FFileUtility::GetParentPath(FileName), SHADER_BLOB_PATH), FFileUtility::GetFileName(FileName));
		std::string hasedName = FFileUtility::RemoveExtension(blobFileName) + "_" + EntryPoint;

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
		return !FFileUtility::IsPathExistent(hasedShaderName) || !FFileUtility::IsPathExistent(reflectionName) || FFileUtility::GetFileLastWriteTime(FileName) > FFileUtility::GetFileLastWriteTime(hasedShaderName);
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

	void FShaderResource::Init(const FDX12CompiledShader& compiledShader, const FShaderCreationInfo& creationInfo)
	{
		ASSERT(compiledShader.CompiledShaderBlob != nullptr);

		mCompiledShaderBlob = compiledShader.CompiledShaderBlob;
		mShaderBinary.Data = mCompiledShaderBlob->GetBufferPointer();
		mShaderBinary.Size = static_cast<uint32>(mCompiledShaderBlob->GetBufferSize());

		mCreationInfo = creationInfo;

		ReflectShaderParameter(compiledShader.ShaderReflector);
	}

	enum class EVSInputType
	{
		Float,
		Half,
		Short,
	};

	void FShaderResource::ReflectShaderParameter(const TRefCountPtr<ID3D12ShaderReflection>& reflector)
	{
		D3D12_SHADER_DESC shaderDesc;
		reflector->GetDesc(&shaderDesc);

		DASH_LOG(LogTemp, Info, "Num ConstantBuffers {} ", shaderDesc.ConstantBuffers);
		DASH_LOG(LogTemp, Info, "Num BoundResources {} ", shaderDesc.BoundResources);

		for (uint32 resourceIndex = 0; resourceIndex < shaderDesc.BoundResources; ++resourceIndex)
		{
			D3D12_SHADER_INPUT_BIND_DESC resourceDesc;
			reflector->GetResourceBindingDesc(resourceIndex, &resourceDesc);

			FShaderParameter resourceParameter;
			resourceParameter.Name = std::string(resourceDesc.Name);
			resourceParameter.ShaderStage = mCreationInfo.Stage;
			resourceParameter.BindPoint = resourceDesc.BindPoint;
			resourceParameter.RegisterSpace = resourceDesc.Space;
			resourceParameter.ShaderInputType = resourceDesc.Type;
			resourceParameter.BindingType = D3DBindDescToShaderCodeResourceBinding(resourceDesc);
			resourceParameter.ResourceDimension = resourceDesc.Dimension;
			resourceParameter.BindCount = resourceDesc.BindCount;

			if (resourceDesc.Type == D3D_SIT_CBUFFER || resourceDesc.Type == D3D_SIT_TBUFFER)
			{
				ASSERT_MSG(resourceDesc.Type != D3D_SIT_TBUFFER, "TBuffer Not Supported!");
				ReflectCBVParamter(resourceParameter, resourceDesc, reflector);
			}
			else if (resourceDesc.Type == D3D_SIT_TEXTURE || resourceDesc.Type == D3D_SIT_STRUCTURED || resourceDesc.Type == D3D_SIT_BYTEADDRESS || resourceDesc.Type == D3D_SIT_RTACCELERATIONSTRUCTURE)
			{
				ASSERT(resourceDesc.BindCount == 1);
				ReflectSRVParamter(resourceParameter, resourceDesc, reflector);
			}
			else if (resourceDesc.Type == D3D_SIT_UAV_RWTYPED || resourceDesc.Type == D3D_SIT_UAV_RWSTRUCTURED ||
				resourceDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS || resourceDesc.Type == D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER ||
				resourceDesc.Type == D3D_SIT_UAV_APPEND_STRUCTURED)
			{
				ASSERT(resourceDesc.BindCount == 1);
				ReflectUAVParamter(resourceParameter, resourceDesc, reflector);
			}
			else if (resourceDesc.Type == D3D_SIT_SAMPLER)
			{
				ReflectSamplerParamter(resourceParameter, resourceDesc, reflector);
			}
			else
			{
				ASSERT_MSG(false, "Invalid Shader Resource Type!");
			}
		}

		if (mCreationInfo.Stage == EShaderStage::Vertex)
		{
			ReflectInputLayout(shaderDesc, reflector);
		}
	}

	void FShaderResource::ReflectCBVParamter(FShaderParameter& parameter, const D3D12_SHADER_INPUT_BIND_DESC& resourceDesc, const TRefCountPtr<ID3D12ShaderReflection>& reflector)
	{
		ID3D12ShaderReflectionConstantBuffer* shaderReflectionConstantBuffer = reflector->GetConstantBufferByName(resourceDesc.Name);

		D3D12_SHADER_BUFFER_DESC bufferDesc;
		shaderReflectionConstantBuffer->GetDesc(&bufferDesc);

		for (uint32 variableIndex = 0; variableIndex < bufferDesc.Variables; variableIndex++)
		{
			ID3D12ShaderReflectionVariable* shaderReflectionVariable = shaderReflectionConstantBuffer->GetVariableByIndex(variableIndex);

			D3D12_SHADER_VARIABLE_DESC variableDesc;
			shaderReflectionVariable->GetDesc(&variableDesc);

			FConstantBufferVariable bufferVariable;
			bufferVariable.ParamterType = ParseAndRemoveBindlessParameterPrefix(bufferVariable.VariableName);
			bufferVariable.VariableName = variableDesc.Name;
			bufferVariable.Size = variableDesc.Size;
			bufferVariable.StartOffset = variableDesc.StartOffset;
			
			parameter.ConstantBufferVariables.push_back(bufferVariable);	
		}

		parameter.ParameterType = EShaderParameterType::UniformBuffer;
		parameter.Size = bufferDesc.Size;

		mCBVParameters.push_back(parameter);

		DASH_LOG(LogTemp, Info, "Found CBuffer {} BindPoint {} Register Space {}", parameter.Name, parameter.BindPoint, parameter.RegisterSpace);
	}

	void FShaderResource::ReflectSRVParamter(FShaderParameter& parameter, const D3D12_SHADER_INPUT_BIND_DESC& resourceDesc, const TRefCountPtr<ID3D12ShaderReflection>& reflector)
	{
		if (resourceDesc.Type == D3D_SIT_STRUCTURED)
		{
			parameter.Stride = resourceDesc.NumSamples;
		}
		parameter.ParameterType = EShaderParameterType::SRV;
		mSRVParameters.push_back(parameter);
	}

	void FShaderResource::ReflectUAVParamter(FShaderParameter& parameter, const D3D12_SHADER_INPUT_BIND_DESC& resourceDesc, const TRefCountPtr<ID3D12ShaderReflection>& reflector)
	{
		parameter.ParameterType = EShaderParameterType::UAV;
		mUAVParameters.push_back(parameter);
	}

	void FShaderResource::ReflectSamplerParamter(FShaderParameter& parameter, const D3D12_SHADER_INPUT_BIND_DESC& resourceDesc, const TRefCountPtr<ID3D12ShaderReflection>& reflector)
	{
		parameter.ParameterType = EShaderParameterType::Sampler;
		mSamplerParameters.push_back(parameter);
	}

	void FShaderResource::ReflectInputLayout(const D3D12_SHADER_DESC& shaderDesc, const TRefCountPtr<ID3D12ShaderReflection>& reflector)
	{
		uint32 currentInputSlot = 0;
		std::string prevSemanticName{};
		uint32 prevSemanticIndex = 0;

		int32 systemValueStart = INDEX_NONE;
		int32 noSystemValueEnd = INDEX_NONE;

		for (uint32 parameterIndex = 0; parameterIndex < shaderDesc.InputParameters; ++parameterIndex)
		{
			D3D12_SIGNATURE_PARAMETER_DESC inputSignatureParameterDesc;
			reflector->GetInputParameterDesc(parameterIndex, &inputSignatureParameterDesc);

			EVSInputType inputType = EVSInputType::Float;
			EResourceFormat parameterFormat = EResourceFormat::Unknown;

			std::string currentSemanticName{ inputSignatureParameterDesc.SemanticName };
			bool isIstanceSemantic = FStringUtility::Contains(FStringUtility::ToLower(currentSemanticName), "instance");
			bool isSystemGeneratedValue = IsSystemGeneratedValues(currentSemanticName);

			if (isSystemGeneratedValue)
			{
				if (systemValueStart == INDEX_NONE)
				{
					systemValueStart = parameterIndex;
				}
				continue;
			}
			else
			{
				noSystemValueEnd = parameterIndex;
			}

			if (FStringUtility::Contains(currentSemanticName, "Half"))
			{
				inputType = EVSInputType::Half;
			}
			else if (FStringUtility::Contains(currentSemanticName, "Short"))
			{
				inputType = EVSInputType::Short;
			}

			if (inputSignatureParameterDesc.Mask == 1)
			{
				if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::R32_Unsigned;
						break;
					case EVSInputType::Half:
						parameterFormat = EResourceFormat::R16_Unsigned;
						break;
					case EVSInputType::Short:
						parameterFormat = EResourceFormat::R8_Unsigned;
						break;
					default:
						break;
					}
				}
				else if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::R32_Signed;
						break;
					case EVSInputType::Half:
						parameterFormat = EResourceFormat::R16_Signed;
						break;
					case EVSInputType::Short:
						parameterFormat = EResourceFormat::R8_Signed;
						break;
					default:
						break;
					}
				}
				else if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::R32_Float;
						break;
					case EVSInputType::Half:
						parameterFormat = EResourceFormat::R16_Float;
						break;
					case EVSInputType::Short:
						parameterFormat = EResourceFormat::R8_Unsigned_Norm;
						break;
					default:
						break;
					}
				}
			}
			else if (inputSignatureParameterDesc.Mask <= 3)
			{
				if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::RG32_Unsigned;
						break;
					case EVSInputType::Half:
						parameterFormat = EResourceFormat::RG16_Unsigned;
						break;
					case EVSInputType::Short:
						parameterFormat = EResourceFormat::RG8_Unsigned;
						break;
					default:
						break;
					}
				}
				else if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::RG32_Signed;
						break;
					case EVSInputType::Half:
						parameterFormat = EResourceFormat::RG16_Signed;
						break;
					case EVSInputType::Short:
						parameterFormat = EResourceFormat::RG8_Signed;
						break;
					default:
						break;
					}
				}
				else if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::RG32_Float;
						break;
					case EVSInputType::Half:
						parameterFormat = EResourceFormat::RG16_Float;
						break;
					case EVSInputType::Short:
						parameterFormat = EResourceFormat::RG8_Usigned_Norm;
						break;
					default:
						break;
					}
				}
			}
			else if (inputSignatureParameterDesc.Mask <= 7)
			{
				if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::RGB32_Unsigned;
						break;
					case EVSInputType::Half:
					case EVSInputType::Short:
						ASSERT_MSG(false, "Not Supported Format!");
						break;
					default:
						break;
					}
				}
				else if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::RGB32_Signed;
						break;
					case EVSInputType::Half:
					case EVSInputType::Short:
						ASSERT_MSG(false, "Not Supported Format!");
						break;
					default:
						break;
					}
				}
				else if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::RGB32_Float;
						break;
					case EVSInputType::Half:
					case EVSInputType::Short:
						ASSERT_MSG(false, "Not Supported Format!");
						break;
					default:
						break;
					}
				}
			}
			else if (inputSignatureParameterDesc.Mask <= 15)
			{
				if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::RGBA32_Unsigned;
						break;
					case EVSInputType::Half:
						parameterFormat = EResourceFormat::RGBA16_Unsigned;
						break;
					case EVSInputType::Short:
						parameterFormat = EResourceFormat::RGBA8_Unsigned;
						break;
					default:
						break;
					}
				}
				else if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::RGBA32_Signed;
						break;
					case EVSInputType::Half:
						parameterFormat = EResourceFormat::RGBA16_Signed;
						break;
					case EVSInputType::Short:
						parameterFormat = EResourceFormat::RGBA8_Signed;
						break;
					default:
						break;
					}
				}
				else if (inputSignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					switch (inputType)
					{
					case EVSInputType::Float:
						parameterFormat = EResourceFormat::RGBA32_Float;
						break;
					case EVSInputType::Half:
						parameterFormat = EResourceFormat::RGBA16_Float;
						break;
					case EVSInputType::Short:
						parameterFormat = EResourceFormat::RGBA8_Unsigned_Norm;
						break;
					default:
						break;
					}
				}
			}

			if (!prevSemanticName.empty())
			{
				if (currentSemanticName != prevSemanticName && (prevSemanticIndex + 1) != inputSignatureParameterDesc.SemanticIndex)
				{
					++currentInputSlot;
				}
			}

			if (isIstanceSemantic)
			{
				mInputLayout.AddPerInstanceLayoutElement(inputSignatureParameterDesc.SemanticName, inputSignatureParameterDesc.SemanticIndex, parameterFormat, currentInputSlot);
			}
			else
			{
				mInputLayout.AddPerVertexLayoutElement(inputSignatureParameterDesc.SemanticName, inputSignatureParameterDesc.SemanticIndex, parameterFormat, currentInputSlot);
			}

			prevSemanticName = std::string(inputSignatureParameterDesc.SemanticName);
			prevSemanticIndex = inputSignatureParameterDesc.SemanticIndex;

			DASH_LOG(LogTemp, Info, " =================================================== ");
		}

		if ((systemValueStart != INDEX_NONE) && (noSystemValueEnd != INDEX_NONE) && (systemValueStart < noSystemValueEnd))
		{
			ASSERT_MSG(false, "System values should be placed after all non-system values.");
		}
	}

	bool FShaderResource::IsSystemGeneratedValues(const std::string& semantic) const
	{	
		std::string lowerSemantic = FStringUtility::ToLower(semantic);
		if (lowerSemantic == "sv_instanceid" || 
			lowerSemantic == "sv_vertexid" ||
			lowerSemantic == "sv_primitiveid")
		{
			return true;
		}

		return false;
	}

	void FShaderResource::SortParameters()
	{
	}

	void FShaderResource::SortParameter(std::vector<FShaderParameter>& parameters)
	{
		std::sort(parameters.begin(), parameters.end(), [](const FShaderParameter& paramA, const FShaderParameter& paramB)
			{
				if (paramA.RegisterSpace == paramB.RegisterSpace)
				{
					return paramA.BindPoint < paramB.BindPoint;
				}
				else
				{
					return paramA.RegisterSpace < paramB.RegisterSpace;
				}
			}
		);
	}
}