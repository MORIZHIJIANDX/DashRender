#pragma once

namespace Dash
{
	#define SHADER_BLOB_FILE_EXTENSION ".cso"
	#define REFLECTION_BLOB_FILE_EXTENSION ".refect"
	#define PDB_BLOB_FILE_EXTENSION ".pdb"

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

		// Texture1D: not used in the renderer.
		// Texture1DArray: not used in the renderer.
		Texture2D,
		Texture2DArray,
		Texture2DMS,
		Texture3D,
		// Texture3DArray: not used in the renderer.
		TextureCube,
		TextureCubeArray,
		TextureMetadata,

		Buffer,
		StructuredBuffer,
		ByteAddressBuffer,
		RaytracingAccelerationStructure,

		// RWTexture1D: not used in the renderer.
		// RWTexture1DArray: not used in the renderer.
		RWTexture2D,
		RWTexture2DArray,
		RWTexture3D,
		// RWTexture3DArray: not used in the renderer.
		RWTextureCube,
		// RWTextureCubeArray: not used in the renderer.
		RWTextureMetadata,

		RWBuffer,
		RWStructuredBuffer,
		RWByteAddressBuffer,

		RasterizerOrderedTexture2D,

		ResourceCollection,

		MAX
	};

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