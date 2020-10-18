#pragma once

//#include "../Consolid/Consolid.h"
#include "../Utility/Assert.h"

namespace Dash
{
	enum class EDASH_FORMAT : uint16_t
	{
		UnKwon = 0,
		R8_UNORM,
		A8_UNORM,
		R16_UNORM,
		R16_UINT,
		R32_UINT,
		R16_FLOAT,
		R32_FLOAT,
		R32G32_FLOAT,
		B5G6R5_UNORM,
		R32G32B32_FLOAT,
		B5G5R5A1_UNORM,
		R8G8B8A8_UINT,
		R8G8B8A8_UNORM,
		B8G8R8A8_UNORM,
		B8G8R8X8_UNORM,
		R10G10B10A2_UNORM,
		R10G10B10_XR_BIAS_A2_UNORM,
		R16G16B16A16_FLOAT,
		R16G16B16A16_UNORM,
		R32G32B32A32_FLOAT,
	};

	FORCEINLINE std::size_t GetByteSizeForFormat(EDASH_FORMAT format)
	{
		switch (format)
		{
		case EDASH_FORMAT::R16_UINT:
			return 2;
			break;
		case EDASH_FORMAT::R8G8B8A8_UINT:
		case EDASH_FORMAT::R8G8B8A8_UNORM:
		case EDASH_FORMAT::R32_UINT:
		case EDASH_FORMAT::R32_FLOAT:
			return 4;
			break;
		case EDASH_FORMAT::R32G32_FLOAT:
			return 8;
			break;
		case EDASH_FORMAT::R32G32B32_FLOAT:
			return 12;
			break;
		case EDASH_FORMAT::R32G32B32A32_FLOAT:
			return 16;
			break;
		default:
			ASSERT(false);
			break;
		}

		return 0;
	}
}