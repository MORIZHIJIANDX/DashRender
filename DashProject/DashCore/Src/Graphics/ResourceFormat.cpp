#include "PCH.h"
#include "ResourceFormat.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"
#include "Utility/Visitor.h"
#include "RenderDevice.h"

namespace Dash
{
	DXGI_FORMAT D3DBaseFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_TYPELESS;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_TYPELESS;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_TYPELESS;

			// 32-bit Z w/ Stencil
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_R32G8X24_TYPELESS;

			// No Stencil
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_TYPELESS;

			// 24-bit Z
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_R24G8_TYPELESS;

			// 16-bit Z w/o Stencil
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
			return DXGI_FORMAT_R16_TYPELESS;

		default:
			return format;
		}
	}

	DXGI_FORMAT D3DUnorderedAccessViewFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM;

		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_UNORM;

		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;

#ifdef DASH_DEBUG
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_D16_UNORM:
			ASSERT_MSG(false, "Requested a UAV Format for a depth stencil Format.");
#endif
		default:
			return format;
		}
	}

	DXGI_FORMAT D3DDepthStencilViewFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
			// 32-bit Z w/ Stencil
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

			// No Stencil
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_D32_FLOAT;

			// 24-bit Z
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;

			// 16-bit Z w/o Stencil
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
			return DXGI_FORMAT_D16_UNORM;

		default:
			return format;
		}
	}

	DXGI_FORMAT D3DDepthFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
			// 32-bit Z w/ Stencil
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

			// No Stencil
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;

			// 24-bit Z
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

			// 16-bit Z w/o Stencil
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
			return DXGI_FORMAT_R16_UNORM;

		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	DXGI_FORMAT D3DStencilFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
			// 32-bit Z w/ Stencil
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;

			// 24-bit Z
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_X24_TYPELESS_G8_UINT;

		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	size_t BytesPerPixel(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 128;

		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			return 96;

		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		case DXGI_FORMAT_Y416:
		case DXGI_FORMAT_Y210:
		case DXGI_FORMAT_Y216:
			return 64;

		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		case DXGI_FORMAT_AYUV:
		case DXGI_FORMAT_Y410:
		case DXGI_FORMAT_YUY2:
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
		case DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
		case DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
		case DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
#endif
			return 32;

		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
		case DXGI_FORMAT_V408:
#endif
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
		case DXGI_FORMAT_D16_UNORM_S8_UINT:
		case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
#endif
			return 24;

		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
		case DXGI_FORMAT_A8P8:
		case DXGI_FORMAT_B4G4R4A4_UNORM:
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
		case DXGI_FORMAT_P208:
		case DXGI_FORMAT_V208:
#endif
			return 16;

		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_420_OPAQUE:
		case DXGI_FORMAT_NV11:
			return 12;

		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
		case DXGI_FORMAT_AI44:
		case DXGI_FORMAT_IA44:
		case DXGI_FORMAT_P8:
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
		case DXGI_FORMAT_R4G4_UNORM:
#endif
			return 8;

		case DXGI_FORMAT_R1_UNORM:
			return 1;

		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			return 4;

		case DXGI_FORMAT_UNKNOWN:
		case DXGI_FORMAT_FORCE_UINT:
		default:
			return 0;
		}
	}

	size_t BytesPerPixel(EResourceFormat format)
	{
		return BytesPerPixel(D3DFormat(format));
	}

	DXGI_FORMAT D3DFormat(EResourceFormat format)
	{
		switch (format)
		{
		case EResourceFormat::R8_Unsigned_Norm:      return DXGI_FORMAT_R8_UNORM;
		case EResourceFormat::RG8_Usigned_Norm:      return DXGI_FORMAT_R8G8_UNORM;
		case EResourceFormat::RGBA8_Unsigned_Norm:   return DXGI_FORMAT_R8G8B8A8_UNORM;
		case EResourceFormat::RGBA16_Unsigned_Norm:  return DXGI_FORMAT_R16G16B16A16_UNORM;

		case EResourceFormat::RGBA8_Unsigned_Norm_Srgb:   return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case EResourceFormat::BGRA8_Unsigned_Norm_Srgb:   return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

		case EResourceFormat::BGRA8_Unsigned_Norm: return DXGI_FORMAT_B8G8R8A8_UNORM;

		case EResourceFormat::R8_Signed:         return DXGI_FORMAT_R8_SINT;
		case EResourceFormat::RG8_Signed:        return DXGI_FORMAT_R8G8_SINT;
		case EResourceFormat::RGBA8_Signed:      return DXGI_FORMAT_R8G8B8A8_SINT;

		case EResourceFormat::R8_Unsigned:       return DXGI_FORMAT_R8_UINT;
		case EResourceFormat::RG8_Unsigned:      return DXGI_FORMAT_R8G8_UINT;
		case EResourceFormat::RGBA8_Unsigned:    return DXGI_FORMAT_R8G8B8A8_UINT;

		case EResourceFormat::R16_Float:         return DXGI_FORMAT_R16_FLOAT;
		case EResourceFormat::RG16_Float:        return DXGI_FORMAT_R16G16_FLOAT;
		case EResourceFormat::RGBA16_Float:      return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case EResourceFormat::R16_Signed:        return DXGI_FORMAT_R16_SINT;
		case EResourceFormat::RG16_Signed:       return DXGI_FORMAT_R16G16_SINT;
		case EResourceFormat::RGBA16_Signed:     return DXGI_FORMAT_R16G16B16A16_SINT;

		case EResourceFormat::R16_Unsigned:      return DXGI_FORMAT_R16_UINT;
		case EResourceFormat::RG16_Unsigned:     return DXGI_FORMAT_R16G16_UINT;
		case EResourceFormat::RGBA16_Unsigned:   return DXGI_FORMAT_R16G16B16A16_UINT;

		case EResourceFormat::R32_Float:         return DXGI_FORMAT_R32_FLOAT;
		case EResourceFormat::RG32_Float:        return DXGI_FORMAT_R32G32_FLOAT;
		case EResourceFormat::RGB32_Float:       return DXGI_FORMAT_R32G32B32_FLOAT;
		case EResourceFormat::RGBA32_Float:      return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case EResourceFormat::R32_Signed:        return DXGI_FORMAT_R32_SINT;
		case EResourceFormat::RG32_Signed:       return DXGI_FORMAT_R32G32_SINT;
		case EResourceFormat::RGB32_Signed:      return DXGI_FORMAT_R32G32B32_SINT;
		case EResourceFormat::RGBA32_Signed:     return DXGI_FORMAT_R32G32B32A32_SINT;

		case EResourceFormat::R32_Unsigned:      return DXGI_FORMAT_R32_UINT;
		case EResourceFormat::RG32_Unsigned:     return DXGI_FORMAT_R32G32_UINT;
		case EResourceFormat::RGB32_Unsigned:    return DXGI_FORMAT_R32G32B32_UINT;
		case EResourceFormat::RGBA32_Unsigned:   return DXGI_FORMAT_R32G32B32A32_UINT;

		case EResourceFormat::RGB10A2_Unorm:     return DXGI_FORMAT_R10G10B10A2_UNORM;
		case EResourceFormat::R11G11B10_Float:   return DXGI_FORMAT_R11G11B10_FLOAT;

		case EResourceFormat::BC1_Unsigned_Norm: return DXGI_FORMAT_BC1_UNORM;
		case EResourceFormat::BC2_Unsigned_Norm: return DXGI_FORMAT_BC2_UNORM;
		case EResourceFormat::BC3_Unsigned_Norm: return DXGI_FORMAT_BC3_UNORM;
		case EResourceFormat::BC4_Unsigned_Norm: return DXGI_FORMAT_BC4_UNORM;
		case EResourceFormat::BC5_Unsigned_Norm: return DXGI_FORMAT_BC5_UNORM;
		case EResourceFormat::BC5_Signed_Norm:   return DXGI_FORMAT_BC5_SNORM;
		case EResourceFormat::BC7_Unsigned_Norm: return DXGI_FORMAT_BC7_UNORM;

		case EResourceFormat::BC1_Unsigned_Norm_Srgb: return DXGI_FORMAT_BC1_UNORM_SRGB;
		case EResourceFormat::BC2_Unsigned_Norm_Srgb: return DXGI_FORMAT_BC2_UNORM_SRGB;
		case EResourceFormat::BC3_Unsigned_Norm_Srgb: return DXGI_FORMAT_BC3_UNORM_SRGB;
		case EResourceFormat::BC7_Unsigned_Norm_Srgb: return DXGI_FORMAT_BC7_UNORM_SRGB;

		case EResourceFormat::R8_Typeless:       return DXGI_FORMAT_R8_TYPELESS;
		case EResourceFormat::RG8_Typeless:      return DXGI_FORMAT_R8G8_TYPELESS;
		case EResourceFormat::RGBA8_Typeless:    return DXGI_FORMAT_R8G8B8A8_TYPELESS;
		case EResourceFormat::R16_Typeless:      return DXGI_FORMAT_R16_TYPELESS;
		case EResourceFormat::RG16_Typeless:     return DXGI_FORMAT_R16G16_TYPELESS;
		case EResourceFormat::RGBA16_Typeless:   return DXGI_FORMAT_R16G16B16A16_TYPELESS;
		case EResourceFormat::R24G8_Typeless:    return DXGI_FORMAT_R24G8_TYPELESS;
		case EResourceFormat::R32G8X24_Typeless: return DXGI_FORMAT_R32G8X24_TYPELESS;
		case EResourceFormat::R32_Typeless:      return DXGI_FORMAT_R32_TYPELESS;
		case EResourceFormat::RG32_Typeless:     return DXGI_FORMAT_R32G32_TYPELESS;
		case EResourceFormat::RGB32_Typeless:    return DXGI_FORMAT_R32G32B32_TYPELESS;
		case EResourceFormat::RGBA32_Typeless:   return DXGI_FORMAT_R32G32B32A32_TYPELESS;

		case EResourceFormat::Depth16_Float:				   return DXGI_FORMAT_R16_FLOAT;
		case EResourceFormat::Depth24_Float_Stencil8_Unsigned: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case EResourceFormat::Depth32_Float:                   return DXGI_FORMAT_D32_FLOAT;
		case EResourceFormat::Depth32_Float_Stencil8_Unsigned: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

		case EResourceFormat::Unknown:            return DXGI_FORMAT_UNKNOWN;

		default: ASSERT_MSG(false, "Should never be hit"); return DXGI_FORMAT_UNKNOWN;
		}
	}

	DXGI_COLOR_SPACE_TYPE D3DColorSpace(EColorSpace space)
	{
		switch (space)
		{
		case EColorSpace::Rec709: return DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
		case EColorSpace::Rec2020: return DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
		default: ASSERT_MSG(false, "Should never be hit"); return {};
		}
	}

	EResourceFormat BaseFormat(EResourceFormat format)
	{
		switch (format)
		{
		case EResourceFormat::R8_Unsigned_Norm:
		case EResourceFormat::R8_Unsigned:
			return EResourceFormat::R8_Typeless;

		case EResourceFormat::RG8_Signed:
		case EResourceFormat::RG8_Unsigned:
			return EResourceFormat::RG8_Typeless;

		case EResourceFormat::RGBA8_Signed:
		case EResourceFormat::RGBA8_Unsigned:
		case EResourceFormat::RGBA8_Unsigned_Norm:
		case EResourceFormat::RGBA8_Unsigned_Norm_Srgb:
			return EResourceFormat::RGBA8_Typeless;

		case EResourceFormat::BGRA8_Unsigned_Norm:
		case EResourceFormat::BGRA8_Unsigned_Norm_Srgb:
			return EResourceFormat::BGRA8_Typeless;

		case EResourceFormat::R16_Float:
		case EResourceFormat::R16_Signed:
		case EResourceFormat::R16_Unsigned:
		case EResourceFormat::Depth16_Float:
			return EResourceFormat::R16_Typeless;

		case EResourceFormat::RG16_Float:
		case EResourceFormat::RG16_Signed:
		case EResourceFormat::RG16_Unsigned:
			return EResourceFormat::RG16_Typeless;

		case EResourceFormat::RGBA16_Float:
		case EResourceFormat::RGBA16_Signed:
		case EResourceFormat::RGBA16_Unsigned:
			return EResourceFormat::RGBA16_Typeless;

		case EResourceFormat::R32_Float:
		case EResourceFormat::R32_Signed:
		case EResourceFormat::R32_Unsigned:
			return EResourceFormat::R32_Typeless;

		case EResourceFormat::Depth24_Float_Stencil8_Unsigned:
			return EResourceFormat::R24G8_Typeless;

		case EResourceFormat::Depth32_Float_Stencil8_Unsigned:
			return EResourceFormat::R32G8X24_Typeless;

		case EResourceFormat::RG32_Float:
		case EResourceFormat::RG32_Signed:
		case EResourceFormat::RG32_Unsigned:
			return EResourceFormat::RG32_Typeless;

		case EResourceFormat::RGB32_Float:
		case EResourceFormat::RGB32_Signed:
		case EResourceFormat::RGB32_Unsigned:
			return EResourceFormat::RGB32_Typeless;

		case EResourceFormat::RGBA32_Float:
		case EResourceFormat::RGBA32_Signed:
		case EResourceFormat::RGBA32_Unsigned:
			return EResourceFormat::RGBA32_Typeless;

		default:
			return format;
		}
	}

	bool IsDepthStencilFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_D16_UNORM:

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
		case DXGI_FORMAT_D16_UNORM_S8_UINT:
		case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
#endif
			return true;

		default:
			return false;
		}
	}

	bool IsDepthStencilFormat(EResourceFormat format)
	{
		switch (format)
		{
		case EResourceFormat::Depth16_Float:
		case EResourceFormat::Depth24_Float_Stencil8_Unsigned: 
		case EResourceFormat::Depth32_Float:        
		case EResourceFormat::Depth32_Float_Stencil8_Unsigned:
			 return true;

		default:
			return false;
		}
	}

	bool IsColorFormat(EResourceFormat format)
	{
		switch (format)
		{
		case EResourceFormat::R8_Unsigned_Norm:      
		case EResourceFormat::RG8_Usigned_Norm:      
		case EResourceFormat::RGBA8_Unsigned_Norm:   
		case EResourceFormat::RGBA16_Unsigned_Norm:  

		case EResourceFormat::RGBA8_Unsigned_Norm_Srgb: 
		case EResourceFormat::BGRA8_Unsigned_Norm_Srgb:  

		case EResourceFormat::BGRA8_Unsigned_Norm: 

		case EResourceFormat::R8_Signed:         
		case EResourceFormat::RG8_Signed:       
		case EResourceFormat::RGBA8_Signed:     

		case EResourceFormat::R8_Unsigned:       
		case EResourceFormat::RG8_Unsigned:    
		case EResourceFormat::RGBA8_Unsigned:    

		case EResourceFormat::R16_Float:        
		case EResourceFormat::RG16_Float:        
		case EResourceFormat::RGBA16_Float:      

		case EResourceFormat::R16_Signed:       
		case EResourceFormat::RG16_Signed:      
		case EResourceFormat::RGBA16_Signed:    

		case EResourceFormat::R16_Unsigned:      
		case EResourceFormat::RG16_Unsigned:    
		case EResourceFormat::RGBA16_Unsigned:   

		case EResourceFormat::R32_Float:        
		case EResourceFormat::RG32_Float:       
		case EResourceFormat::RGB32_Float:       
		case EResourceFormat::RGBA32_Float:      

		case EResourceFormat::R32_Signed:        
		case EResourceFormat::RG32_Signed:     
		case EResourceFormat::RGB32_Signed:      
		case EResourceFormat::RGBA32_Signed:    

		case EResourceFormat::R32_Unsigned:      
		case EResourceFormat::RG32_Unsigned:     
		case EResourceFormat::RGB32_Unsigned:    
		case EResourceFormat::RGBA32_Unsigned:   

		case EResourceFormat::RGB10A2_Unorm:    
		case EResourceFormat::R11G11B10_Float:   

		case EResourceFormat::BC1_Unsigned_Norm: 
		case EResourceFormat::BC2_Unsigned_Norm: 
		case EResourceFormat::BC3_Unsigned_Norm: 
		case EResourceFormat::BC4_Unsigned_Norm: 
		case EResourceFormat::BC5_Unsigned_Norm: 
		case EResourceFormat::BC5_Signed_Norm:   
		case EResourceFormat::BC7_Unsigned_Norm: 

		case EResourceFormat::BC1_Unsigned_Norm_Srgb: 
		case EResourceFormat::BC2_Unsigned_Norm_Srgb: 
		case EResourceFormat::BC3_Unsigned_Norm_Srgb: 
		case EResourceFormat::BC7_Unsigned_Norm_Srgb: 
			return true;

		default: 
			return false;
		}
	}

	bool IsCompressedFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return true;

		default:
			return false;
		}
	}

	bool IsCompressedFormat(EResourceFormat format)
	{
		switch (format)
		{
		case EResourceFormat::BC1_Unsigned_Norm: 
		case EResourceFormat::BC2_Unsigned_Norm: 
		case EResourceFormat::BC3_Unsigned_Norm: 
		case EResourceFormat::BC4_Unsigned_Norm: 
		case EResourceFormat::BC5_Unsigned_Norm: 
		case EResourceFormat::BC5_Signed_Norm:   
		case EResourceFormat::BC7_Unsigned_Norm: 

		case EResourceFormat::BC1_Unsigned_Norm_Srgb:
		case EResourceFormat::BC2_Unsigned_Norm_Srgb:
		case EResourceFormat::BC3_Unsigned_Norm_Srgb:
		case EResourceFormat::BC7_Unsigned_Norm_Srgb:
			 return false;

		default:
		     return true;
		}
	}

	DXGI_FORMAT EnsureNotTypelessFormat(DXGI_FORMAT format)
	{
		// Assumes UNORM or FLOAT; doesn't use UINT or SINT
		switch (format)
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case DXGI_FORMAT_R32G32B32_TYPELESS:    return DXGI_FORMAT_R32G32B32_FLOAT;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case DXGI_FORMAT_R32G32_TYPELESS:       return DXGI_FORMAT_R32G32_FLOAT;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:  return DXGI_FORMAT_R10G10B10A2_UNORM;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:     return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_R16G16_TYPELESS:       return DXGI_FORMAT_R16G16_UNORM;
		case DXGI_FORMAT_R32_TYPELESS:          return DXGI_FORMAT_R32_FLOAT;
		case DXGI_FORMAT_R8G8_TYPELESS:         return DXGI_FORMAT_R8G8_UNORM;
		case DXGI_FORMAT_R16_TYPELESS:          return DXGI_FORMAT_R16_UNORM;
		case DXGI_FORMAT_R8_TYPELESS:           return DXGI_FORMAT_R8_UNORM;
		case DXGI_FORMAT_BC1_TYPELESS:          return DXGI_FORMAT_BC1_UNORM;
		case DXGI_FORMAT_BC2_TYPELESS:          return DXGI_FORMAT_BC2_UNORM;
		case DXGI_FORMAT_BC3_TYPELESS:          return DXGI_FORMAT_BC3_UNORM;
		case DXGI_FORMAT_BC4_TYPELESS:          return DXGI_FORMAT_BC4_UNORM;
		case DXGI_FORMAT_BC5_TYPELESS:          return DXGI_FORMAT_BC5_UNORM;
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:     return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:     return DXGI_FORMAT_B8G8R8X8_UNORM;
		case DXGI_FORMAT_BC7_TYPELESS:          return DXGI_FORMAT_BC7_UNORM;
		default:                                return format;
		}
	}

	EResourceFormat EnsureNotTypelessFormat(EResourceFormat format)
	{
		switch (format)
		{
		case EResourceFormat::R8_Typeless:		return EResourceFormat::R8_Unsigned_Norm;
		case EResourceFormat::RG8_Typeless:		return EResourceFormat::RGBA8_Unsigned_Norm;
		case EResourceFormat::RGBA8_Typeless:	return EResourceFormat::RGBA8_Typeless;
		case EResourceFormat::BGRA8_Typeless:	return EResourceFormat::RGBA8_Unsigned_Norm;
		case EResourceFormat::R16_Typeless:		return EResourceFormat::R16_Float;
		case EResourceFormat::RG16_Typeless:	return EResourceFormat::RG16_Float;
		case EResourceFormat::RGBA16_Typeless:	return EResourceFormat::RGBA16_Float;
		case EResourceFormat::R32_Typeless:		return EResourceFormat::R32_Float;
		case EResourceFormat::RG32_Typeless:	return EResourceFormat::RG32_Float;
		case EResourceFormat::RGB32_Typeless:	return EResourceFormat::RGB32_Float;
		case EResourceFormat::RGBA32_Typeless:	return EResourceFormat::RGBA32_Float;
		case EResourceFormat::R24G8_Typeless:	return EResourceFormat::Depth24_Float_Stencil8_Unsigned;
		case EResourceFormat::R32G8X24_Typeless:	return EResourceFormat::Depth32_Float_Stencil8_Unsigned;
		default:
			return format;
		}
	}

	bool CheckFormatSupport(D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport, D3D12_FORMAT_SUPPORT1 supportToCheck)
	{
		return (formatSupport.Support1 & supportToCheck) != 0;
	}

	bool CheckFormatSupport(D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport, D3D12_FORMAT_SUPPORT2 supportToCheck)
	{
		return (formatSupport.Support2 & supportToCheck) != 0;
	}

	EFormatSupport CheckFormatSupport(DXGI_FORMAT format)
	{
		EFormatSupport formatSupport = EFormatSupport::Invalid;
		if (FGraphicsCore::Device)
		{
			D3D12_FEATURE_DATA_FORMAT_SUPPORT d3dFormatSupport;
			d3dFormatSupport.Format = format;
			DX_CALL(FGraphicsCore::Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &d3dFormatSupport, sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)));

			if (CheckFormatSupport(d3dFormatSupport, D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE))
			{
				formatSupport |= EFormatSupport::ShaderResourceView;
			}

			if (CheckFormatSupport(d3dFormatSupport, D3D12_FORMAT_SUPPORT1_RENDER_TARGET))
			{
				formatSupport |= EFormatSupport::RenderTargetView;
			}

			if (CheckFormatSupport(d3dFormatSupport, D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL))
			{
				formatSupport |= EFormatSupport::DepthStencilView;
			}

			if (CheckFormatSupport(d3dFormatSupport, D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) && 
				CheckFormatSupport(d3dFormatSupport, D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
				CheckFormatSupport(d3dFormatSupport, D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE))
			{
				formatSupport |= EFormatSupport::UnorderAccessView;
			}

			if (CheckFormatSupport(d3dFormatSupport, D3D12_FORMAT_SUPPORT1_BUFFER))
			{
				formatSupport |= EFormatSupport::Buffer;
			}
		}

		return formatSupport;
	}

	EFormatSupport CheckFormatSupport(EResourceFormat format)
	{
		return CheckFormatSupport(D3DFormat(format));
	}

	std::pair<DXGI_FORMAT, std::optional<DXGI_FORMAT>> D3DDepthStecilShaderAccessFormats(EResourceFormat format)
	{
		switch (format)
		{
		case EResourceFormat::Depth16_Float:				   return { DXGI_FORMAT_R16_FLOAT, std::nullopt };
		case EResourceFormat::Depth24_Float_Stencil8_Unsigned: return { DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_X24_TYPELESS_G8_UINT };
		case EResourceFormat::Depth32_Float:                   return { DXGI_FORMAT_R32_FLOAT, std::nullopt };
		case EResourceFormat::Depth32_Float_Stencil8_Unsigned: return { DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS, DXGI_FORMAT_D32_FLOAT_S8X24_UINT };
		default: ASSERT_MSG(false, "Should never be hit"); return {};
		}
	}

	Dash::EResourceFormat ResourceFormatFromD3DFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8_TYPELESS:				return EResourceFormat::R8_Typeless;
		case DXGI_FORMAT_R8G8_TYPELESS:				return EResourceFormat::RG8_Typeless;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:			return EResourceFormat::RGBA8_Typeless;
		case DXGI_FORMAT_R16_TYPELESS:				return EResourceFormat::R16_Typeless;
		case DXGI_FORMAT_R16G16_TYPELESS:			return EResourceFormat::RG16_Typeless;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:		return EResourceFormat::RGBA16_Typeless;
		case DXGI_FORMAT_R32_TYPELESS:				return EResourceFormat::R32_Typeless;
		case DXGI_FORMAT_R32G32_TYPELESS:			return EResourceFormat::RG32_Typeless;
		case DXGI_FORMAT_R32G32B32_TYPELESS:		return EResourceFormat::RGB32_Typeless;
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:		return EResourceFormat::RGBA32_Typeless;

		case DXGI_FORMAT_B8G8R8A8_UNORM:			return EResourceFormat::BGRA8_Unsigned_Norm;

		case DXGI_FORMAT_R8_UNORM:					return EResourceFormat::R8_Unsigned_Norm;
		case DXGI_FORMAT_R8G8_UNORM:				return EResourceFormat::RG8_Usigned_Norm;
		case DXGI_FORMAT_R8G8B8A8_UNORM:			return EResourceFormat::RGBA8_Unsigned_Norm;
		case DXGI_FORMAT_R16G16B16A16_UNORM:		return EResourceFormat::RGBA16_Unsigned_Norm;

		case DXGI_FORMAT_R8_SINT:					return EResourceFormat::R8_Signed;
		case DXGI_FORMAT_R8G8_SINT:					return EResourceFormat::RG8_Signed;
		case DXGI_FORMAT_R8G8B8A8_SINT:				return EResourceFormat::RGBA8_Signed;

		case DXGI_FORMAT_R8_UINT:					return EResourceFormat::R8_Unsigned;
		case DXGI_FORMAT_R8G8_UINT:					return EResourceFormat::RG8_Unsigned;
		case DXGI_FORMAT_R8G8B8A8_UINT:				return EResourceFormat::RGBA8_Unsigned;

		case DXGI_FORMAT_R16_FLOAT:					return EResourceFormat::R16_Float;
		case DXGI_FORMAT_R16G16_FLOAT:				return EResourceFormat::RG16_Float;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:		return EResourceFormat::RGBA16_Float;

		case DXGI_FORMAT_R16_SINT:					return EResourceFormat::R16_Signed;
		case DXGI_FORMAT_R16G16_SINT:				return EResourceFormat::RG16_Signed;
		case DXGI_FORMAT_R16G16B16A16_SINT:			return EResourceFormat::RGBA16_Signed;

		case DXGI_FORMAT_R16_UINT:					return EResourceFormat::R16_Unsigned;
		case DXGI_FORMAT_R16G16_UINT:				return EResourceFormat::RG16_Unsigned;
		case DXGI_FORMAT_R16G16B16A16_UINT:			return EResourceFormat::RGBA16_Unsigned;

		case DXGI_FORMAT_R32_FLOAT:					return EResourceFormat::R32_Float;
		case DXGI_FORMAT_R32G32_FLOAT:				return EResourceFormat::RG32_Float;
		case DXGI_FORMAT_R32G32B32_FLOAT:			return EResourceFormat::RGB32_Float;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:		return EResourceFormat::RGBA32_Float;

		case DXGI_FORMAT_R32_SINT:					return EResourceFormat::R32_Signed;
		case DXGI_FORMAT_R32G32_SINT:				return EResourceFormat::RG32_Signed;
		case DXGI_FORMAT_R32G32B32_SINT:			return EResourceFormat::RGB32_Signed;
		case DXGI_FORMAT_R32G32B32A32_SINT:			return EResourceFormat::RGBA32_Signed;

		case DXGI_FORMAT_R32_UINT:					return EResourceFormat::R32_Unsigned;
		case DXGI_FORMAT_R32G32_UINT:				return EResourceFormat::RG32_Unsigned;
		case DXGI_FORMAT_R32G32B32_UINT:			return EResourceFormat::RGB32_Unsigned;
		case DXGI_FORMAT_R32G32B32A32_UINT:			return EResourceFormat::RGBA32_Unsigned;

		case DXGI_FORMAT_D24_UNORM_S8_UINT:			return EResourceFormat::Depth24_Float_Stencil8_Unsigned;
		case DXGI_FORMAT_D32_FLOAT:					return EResourceFormat::Depth32_Float;

		case DXGI_FORMAT_R10G10B10A2_UNORM:			return EResourceFormat::RGB10A2_Unorm;
		case DXGI_FORMAT_R11G11B10_FLOAT:			return EResourceFormat::R11G11B10_Float;

			// Compressed formats
		case DXGI_FORMAT_BC1_UNORM:					return EResourceFormat::BC1_Unsigned_Norm;
		case DXGI_FORMAT_BC2_UNORM:					return EResourceFormat::BC2_Unsigned_Norm;
		case DXGI_FORMAT_BC3_UNORM:					return EResourceFormat::BC3_Unsigned_Norm;
		case DXGI_FORMAT_BC4_UNORM:					return EResourceFormat::BC4_Unsigned_Norm;
		case DXGI_FORMAT_BC5_UNORM:					return EResourceFormat::BC5_Unsigned_Norm;
		case DXGI_FORMAT_BC5_SNORM:					return EResourceFormat::BC5_Signed_Norm;
		case DXGI_FORMAT_BC7_UNORM:					return EResourceFormat::BC7_Unsigned_Norm;

		case DXGI_FORMAT_UNKNOWN:					return EResourceFormat::Unknown;

		default:
			ASSERT_MSG(false, "Unsupported D3D format");
			return EResourceFormat::Unknown;
		}
	}

	EColorSpace ColorSpaceFromD3DSpace(DXGI_COLOR_SPACE_TYPE space)
	{
		switch (space)
		{
		case DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709: return EColorSpace::Rec709;
		case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020: return EColorSpace::Rec2020;
		default:
			ASSERT_MSG(false, "Unsupported Color Space");
			return EColorSpace::Rec709;
		}
	}

	FResourceMagnitude::FResourceMagnitude(uint32_t width, uint32_t height, uint32_t depth)
		: Width(width)
		, Height(height)
		, Depth(depth)
	{}

	FResourceMagnitude::FResourceMagnitude(uint32_t width, uint32_t height)
		: FResourceMagnitude(width, height, 1)
	{}

	FResourceMagnitude::FResourceMagnitude(uint32_t width)
		: FResourceMagnitude(width, 1, 1)
	{}

	uint32_t FResourceMagnitude::LargestMagnitude() const
	{
		return FMath::Max(Width, Height, Depth);
	}

	bool FResourceMagnitude::operator==(const FResourceMagnitude& rhs)
	{
		return Width == rhs.Width && Height == rhs.Height && Depth == rhs.Depth;
	}

	bool FResourceMagnitude::operator!=(const FResourceMagnitude& rhs)
	{
		return !(*this == rhs);
	}

	FResourceMagnitude FResourceMagnitude::XMultiplied(float m) const
	{
		return FResourceMagnitude(static_cast<uint32_t>(Width * m));
	}

	FResourceMagnitude FResourceMagnitude::XYMultiplied(float m) const
	{
		return FResourceMagnitude(static_cast<uint32_t>(Width * m), static_cast<uint32_t>(Height * m));
	}

	FResourceMagnitude FResourceMagnitude::XYZMultiplied(float m) const
	{	
		return FResourceMagnitude(static_cast<uint32_t>(Width * m), static_cast<uint32_t>(Height * m), static_cast<uint32_t>(Depth * m));
	}
}


