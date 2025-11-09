#pragma once

namespace Dash
{
	#define SHADER_BLOB_FILE_EXTENSION ".cso"
	#define REFLECTION_BLOB_FILE_EXTENSION ".refect"
	#define PDB_BLOB_FILE_EXTENSION ".pdb"
	#define SHADER_PREPROCESS_FILE_EXTENSION ".preprocess"

	static constexpr const char* GBindlessCBufferName = TEXT("BindlessCBuffer");
	static constexpr const char* GBindlessSRVPrefix = TEXT("BindlessSRV_");
	static constexpr const char* GBindlessUAVPrefix = TEXT("BindlessUAV_");
	static constexpr const char* GBindlessSamplerPrefix = TEXT("BindlessSampler_");

	enum class EShaderStage : uint16
	{
		Vertex = 0,
		Hull = 1,
		Domain = 2,
		Geometry = 3,
		Pixel = 4,
		Compute = 5,
		Count = 6,
	};

	constexpr uint32 GShaderStageCount = static_cast<uint32>(EShaderStage::Count);

	const char* ShaderStageToString(EShaderStage stage);

	enum class EShaderResourceBindingType : uint8
	{
		Invalid,

		SamplerState,

		Texture1D,
		Texture1DArray,
		Texture2D,
		Texture2DArray,
		Texture2DMS,
		Texture3D,
		Texture3DArray,
		TextureCube,
		TextureCubeArray,
		TextureMetadata,

		Buffer,
		StructuredBuffer,
		ByteAddressBuffer,
		RaytracingAccelerationStructure,

		RWTexture1D,
		RWTexture1DArray,
		RWTexture2D,
		RWTexture2DArray,
		RWTexture3D,
		RWTexture3DArray,
		RWTextureCube,
		RWTextureCubeArray,
		RWTextureMetadata,

		RWBuffer,
		RWStructuredBuffer,
		RWByteAddressBuffer,

		RasterizerOrderedTexture2D,

		ResourceCollection,

		MAX
	};

	EShaderResourceBindingType D3DBindDescToShaderCodeResourceBinding(D3D12_SHADER_INPUT_BIND_DESC binding);
	EShaderResourceBindingType ShaderCodeResourceBindingFromString(const std::string& hlslType);

	enum class EShaderParameterType : uint8
	{
		Invalid,

		LooseData, // 普通的 (非 Bindless) UniformBuffer 成员
		UniformBuffer,
		Sampler,
		SRV,
		UAV,

		BindlessSampler,
		BindlessSRV,
		BindlessUAV,

		Num
	};

	enum class EShaderPassType
	{
		Raster,
		Compute
	};
}