#include "PCH.h"
#include "GraphicsDefines.h"

namespace Dash
{
	const char* ShaderStageToString(EShaderStage stage)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:   return "Vertex";
		case EShaderStage::Hull:     return "Hull";
		case EShaderStage::Domain:   return "Domain";
		case EShaderStage::Geometry: return "Geometry";
		case EShaderStage::Pixel:    return "Pixel";
		case EShaderStage::Compute:  return "Compute";
		default:                     return "Unknown";
		}
	}

	EShaderResourceBindingType D3DBindDescToShaderCodeResourceBinding(D3D12_SHADER_INPUT_BIND_DESC binding)
	{
		switch (binding.Type)
		{
		case D3D_SIT_SAMPLER:
			return EShaderResourceBindingType::SamplerState;
		case D3D_SIT_TBUFFER:
		case D3D_SIT_CBUFFER:
			return EShaderResourceBindingType::Buffer;
		case D3D_SIT_TEXTURE:
			switch (binding.Dimension)
			{
			case D3D_SRV_DIMENSION_BUFFER:			return EShaderResourceBindingType::Buffer;
			case D3D_SRV_DIMENSION_TEXTURE2D:		return EShaderResourceBindingType::Texture2D;
			case D3D_SRV_DIMENSION_TEXTURE2DARRAY:	return EShaderResourceBindingType::Texture2DArray;
			case D3D_SRV_DIMENSION_TEXTURE2DMS:		return EShaderResourceBindingType::Texture2DMS;
			case D3D_SRV_DIMENSION_TEXTURE3D:		return EShaderResourceBindingType::Texture3D;
			case D3D_SRV_DIMENSION_TEXTURECUBE:		return EShaderResourceBindingType::TextureCube;
			default:
				return EShaderResourceBindingType::Invalid;
			}
		case D3D_SIT_UAV_RWTYPED:
			switch (binding.Dimension)
			{
			case D3D_SRV_DIMENSION_BUFFER:			return EShaderResourceBindingType::RWBuffer;
			case D3D_SRV_DIMENSION_TEXTURE2D:		return EShaderResourceBindingType::RWTexture2D;
			case D3D_SRV_DIMENSION_TEXTURE2DARRAY:	return EShaderResourceBindingType::RWTexture2DArray;
			case D3D_SRV_DIMENSION_TEXTURE3D:		return EShaderResourceBindingType::RWTexture3D;
			case D3D_SRV_DIMENSION_TEXTURECUBE:		return EShaderResourceBindingType::RWTextureCube;
			default:
				return EShaderResourceBindingType::Invalid;
			}
		case D3D_SIT_STRUCTURED:		return EShaderResourceBindingType::StructuredBuffer;
		case D3D_SIT_UAV_RWSTRUCTURED:	return EShaderResourceBindingType::RWStructuredBuffer;
		case D3D_SIT_BYTEADDRESS:		return EShaderResourceBindingType::ByteAddressBuffer;
		case D3D_SIT_UAV_RWBYTEADDRESS:	return EShaderResourceBindingType::RWByteAddressBuffer;
		default:
			return EShaderResourceBindingType::Invalid;
		}
	}

	// 元素类型
	enum class EShaderResourceElementType {
		Unknown,
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
		Uint,
		Uint2,
		Uint3,
		Uint4,
		Half,
		Half2,
		Half3,
		Half4,
		Double,
		Bool,
		Struct  // 用于 StructuredBuffer<CustomStruct>
	};

	// 静态成员定义
	const std::unordered_map<std::string, EShaderResourceBindingType> GResourceTypeMap = {
		{"Texture1D", EShaderResourceBindingType::Texture1D},
		{"Texture1DArray", EShaderResourceBindingType::Texture1DArray},
		{"Texture2D", EShaderResourceBindingType::Texture2D},
		{"Texture2DArray", EShaderResourceBindingType::Texture2DArray},
		{"Texture2DMS", EShaderResourceBindingType::Texture2DMS},
		{"Texture3D", EShaderResourceBindingType::Texture3D},
		{"TextureCube", EShaderResourceBindingType::TextureCube},
		{"TextureCubeArray", EShaderResourceBindingType::TextureCubeArray},
		{"Buffer", EShaderResourceBindingType::Buffer},
		{"StructuredBuffer", EShaderResourceBindingType::StructuredBuffer},
		{"ByteAddressBuffer", EShaderResourceBindingType::ByteAddressBuffer},
		{"RWTexture1D", EShaderResourceBindingType::RWTexture1D},
		{"RWTexture1DArray", EShaderResourceBindingType::RWTexture1DArray},
		{"RWTexture2D", EShaderResourceBindingType::RWTexture2D},
		{"RWTexture2DArray", EShaderResourceBindingType::RWTexture2DArray},
		{"RWTexture3D", EShaderResourceBindingType::RWTexture3D},
		{"RWBuffer", EShaderResourceBindingType::RWBuffer},
		{"RWStructuredBuffer", EShaderResourceBindingType::RWStructuredBuffer},
		{"RWByteAddressBuffer", EShaderResourceBindingType::RWByteAddressBuffer},
		//{"AppendStructuredBuffer", EShaderResourceBindingType::AppendStructuredBuffer},
		//{"ConsumeStructuredBuffer", EShaderResourceBindingType::ConsumeStructuredBuffer},
		{"SamplerState", EShaderResourceBindingType::SamplerState},
		//{"SamplerComparisonState", EShaderResourceBindingType::SamplerComparisonState},
		//{"ConstantBuffer", EShaderResourceBindingType::ConstantBuffer},
		{"RaytracingAccelerationStructure", EShaderResourceBindingType::RaytracingAccelerationStructure}
	};

	const std::unordered_map<std::string, EShaderResourceElementType> GElementTypeMap = {
		{"float", EShaderResourceElementType::Float},
		{"float1", EShaderResourceElementType::Float},
		{"float2", EShaderResourceElementType::Float2},
		{"float3", EShaderResourceElementType::Float3},
		{"float4", EShaderResourceElementType::Float4},
		{"int", EShaderResourceElementType::Int},
		{"int1", EShaderResourceElementType::Int},
		{"int2", EShaderResourceElementType::Int2},
		{"int3", EShaderResourceElementType::Int3},
		{"int4", EShaderResourceElementType::Int4},
		{"uint", EShaderResourceElementType::Uint},
		{"uint1", EShaderResourceElementType::Uint},
		{"uint2", EShaderResourceElementType::Uint2},
		{"uint3", EShaderResourceElementType::Uint3},
		{"uint4", EShaderResourceElementType::Uint4},
		{"half", EShaderResourceElementType::Half},
		{"half1", EShaderResourceElementType::Half},
		{"half2", EShaderResourceElementType::Half2},
		{"half3", EShaderResourceElementType::Half3},
		{"half4", EShaderResourceElementType::Half4},
		{"double", EShaderResourceElementType::Double},
		{"bool", EShaderResourceElementType::Bool}
	};

	EShaderResourceBindingType ShaderCodeResourceBindingFromString(const std::string& hlslType)
	{
		const std::regex TemplateType(R"((\w+)\s*<\s*([^>]+)\s*>)");
		const std::regex ArrayType(R"((\w+(?:\s*<[^>]+>)?)\s*\[\s*(\d+)\s*\])");

		bool isArray = false;
		bool isReadWrite = false;
		int32 arraySize = 0;
		EShaderResourceBindingType type;
		EShaderResourceElementType elementType;

		std::string cleanType = hlslType;
		cleanType.erase(std::remove_if(cleanType.begin(), cleanType.end(),
			[](char c) { return std::isspace(c); }),
			cleanType.end());

		// 检查是否是数组
		std::smatch arrayMatch;
		if (std::regex_match(cleanType, arrayMatch, ArrayType)) {
			isArray = true;
			arraySize = std::stoi(arrayMatch[2]);
			cleanType = arrayMatch[1];
		}

		// 提取基础类型和模板参数
		std::string baseType;
		std::string templateParam;
		std::string elementTypeName;

		std::smatch templateMatch;
		if (std::regex_match(cleanType, templateMatch, TemplateType)) {
			baseType = templateMatch[1];
			templateParam = templateMatch[2];
		}
		else {
			baseType = cleanType;
		}

		// 检查是否是 RW 类型
		if (baseType.substr(0, 2) == "RW") {
			isReadWrite = true;
		}

		// 查找资源类型
		auto typeIt = GResourceTypeMap.find(baseType);
		if (typeIt != GResourceTypeMap.end()) {
			type = typeIt->second;
		}

		// 解析元素类型
		if (!templateParam.empty()) {
			auto elemIt = GElementTypeMap.find(templateParam);
			if (elemIt != GElementTypeMap.end()) {
				elementType = elemIt->second;
			}
			else {
				// 可能是自定义结构体
				elementType = EShaderResourceElementType::Struct;
				elementTypeName = templateParam;
			}
		}

		DASH_LOG(LogTemp, Info, "HLSL Type {}, baseType {}, templateParam {}, elementTypeName {}, Is Array {}", hlslType, baseType, templateParam, elementTypeName, isArray);

		return type;
	}
}

