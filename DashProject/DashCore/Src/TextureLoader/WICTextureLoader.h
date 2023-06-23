#pragma once

#include <string>
#include <vector>
#include "Graphics/ResourceDescription.h"

namespace Dash
{
	enum class EWIC_LOADER_FLAGS : uint32_t
	{
		WIC_LOADER_DEFAULT = 0,
		WIC_LOADER_FORCE_SRGB = 0x1,
		WIC_LOADER_IGNORE_SRGB = 0x2,
		WIC_LOADER_SRGB_DEFAULT = 0x4,
		WIC_LOADER_MIP_AUTOGEN = 0x8,
		WIC_LOADER_MIP_RESERVE = 0x10,
		WIC_LOADER_FIT_POW2 = 0x20,
		WIC_LOADER_MAKE_SQUARE = 0x40,
		WIC_LOADER_FORCE_RGBA32 = 0x80,
	};

	ENABLE_BITMASK_OPERATORS(EWIC_LOADER_FLAGS);


	bool __cdecl LoadWICTextureFromFile(
		const std::string& fileName,
		EWIC_LOADER_FLAGS loadFlags,
		FTextureBufferDescription& textureDescription,
		D3D12_SUBRESOURCE_DATA& subResource,
		std::vector<uint8_t>& decodedData);
}