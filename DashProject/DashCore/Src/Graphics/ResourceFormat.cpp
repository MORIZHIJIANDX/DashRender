#include "PCH.h"
#include "ResourceFormat.h"

namespace Dash
{
	DXGI_FORMAT D3DFormat(ETypelessColorFormat type)
	{
		switch (type)
		{
		case ETypelessColorFormat::R8:                return DXGI_FORMAT_R8_TYPELESS;
		case ETypelessColorFormat::RG8:               return DXGI_FORMAT_R8G8_TYPELESS;
		case ETypelessColorFormat::RGBA8:             return DXGI_FORMAT_R8G8B8A8_TYPELESS;
		case ETypelessColorFormat::R16:               return DXGI_FORMAT_R16_TYPELESS;
		case ETypelessColorFormat::RG16:              return DXGI_FORMAT_R16G16_TYPELESS;
		case ETypelessColorFormat::RGBA16:            return DXGI_FORMAT_R16G16B16A16_TYPELESS;
		case ETypelessColorFormat::R32:               return DXGI_FORMAT_R32_TYPELESS;
		case ETypelessColorFormat::RG32:              return DXGI_FORMAT_R32G32_TYPELESS;
		case ETypelessColorFormat::RGB32:             return DXGI_FORMAT_R32G32B32_TYPELESS;
		case ETypelessColorFormat::RGBA32:            return DXGI_FORMAT_R32G32B32A32_TYPELESS;
		default: ASSERT_MSG(false, "Should never be hit"); return DXGI_FORMAT_UNKNOWN;
		}
	}

	DXGI_FORMAT D3DFormat(EColorFormat type)
	{
		switch (type)
		{
		case EColorFormat::R8_Unsigned_Norm:      return DXGI_FORMAT_R8_UNORM;
		case EColorFormat::RG8_Usigned_Norm:     return DXGI_FORMAT_R8G8_UNORM;
		case EColorFormat::RGBA8_Unsigned_Norm:   return DXGI_FORMAT_R8G8B8A8_UNORM;
		case EColorFormat::RGBA16_Unsigned_Norm: return DXGI_FORMAT_R16G16B16A16_UNORM;

		case EColorFormat::BGRA8_Unsigned_Norm: return DXGI_FORMAT_B8G8R8A8_UNORM;

		case EColorFormat::R8_Signed:         return DXGI_FORMAT_R8_SINT;
		case EColorFormat::RG8_Signed:        return DXGI_FORMAT_R8G8_SINT;
		case EColorFormat::RGBA8_Signed:      return DXGI_FORMAT_R8G8B8A8_SINT;

		case EColorFormat::R8_Unsigned:       return DXGI_FORMAT_R8_UINT;
		case EColorFormat::RG8_Unsigned:      return DXGI_FORMAT_R8G8_UINT;
		case EColorFormat::RGBA8_Unsigned:    return DXGI_FORMAT_R8G8B8A8_UINT;

		case EColorFormat::R16_Float:         return DXGI_FORMAT_R16_FLOAT;
		case EColorFormat::RG16_Float:        return DXGI_FORMAT_R16G16_FLOAT;
		case EColorFormat::RGBA16_Float:      return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case EColorFormat::R16_Signed:        return DXGI_FORMAT_R16_SINT;
		case EColorFormat::RG16_Signed:       return DXGI_FORMAT_R16G16_SINT;
		case EColorFormat::RGBA16_Signed:     return DXGI_FORMAT_R16G16B16A16_SINT;

		case EColorFormat::R16_Unsigned:      return DXGI_FORMAT_R16_UINT;
		case EColorFormat::RG16_Unsigned:     return DXGI_FORMAT_R16G16_UINT;
		case EColorFormat::RGBA16_Unsigned:   return DXGI_FORMAT_R16G16B16A16_UINT;

		case EColorFormat::R32_Float:         return DXGI_FORMAT_R32_FLOAT;
		case EColorFormat::RG32_Float:        return DXGI_FORMAT_R32G32_FLOAT;
		case EColorFormat::RGB32_Float:       return DXGI_FORMAT_R32G32B32_FLOAT;
		case EColorFormat::RGBA32_Float:      return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case EColorFormat::R32_Signed:        return DXGI_FORMAT_R32_SINT;
		case EColorFormat::RG32_Signed:       return DXGI_FORMAT_R32G32_SINT;
		case EColorFormat::RGB32_Signed:      return DXGI_FORMAT_R32G32B32_SINT;
		case EColorFormat::RGBA32_Signed:     return DXGI_FORMAT_R32G32B32A32_SINT;

		case EColorFormat::R32_Unsigned:      return DXGI_FORMAT_R32_UINT;
		case EColorFormat::RG32_Unsigned:     return DXGI_FORMAT_R32G32_UINT;
		case EColorFormat::RGB32_Unsigned:    return DXGI_FORMAT_R32G32B32_UINT;
		case EColorFormat::RGBA32_Unsigned:   return DXGI_FORMAT_R32G32B32A32_UINT;

		case EColorFormat::RGB10A2_Unorm:     return DXGI_FORMAT_R10G10B10A2_UNORM;
		case EColorFormat::R11G11B10_Float:   return DXGI_FORMAT_R11G11B10_FLOAT;

		case EColorFormat::BC1_Unsigned_Norm: return DXGI_FORMAT_BC1_UNORM;
		case EColorFormat::BC2_Unsigned_Norm: return DXGI_FORMAT_BC2_UNORM;
		case EColorFormat::BC3_Unsigned_Norm: return DXGI_FORMAT_BC3_UNORM;
		case EColorFormat::BC4_Unsigned_Norm: return DXGI_FORMAT_BC4_UNORM;
		case EColorFormat::BC5_Unsigned_Norm: return DXGI_FORMAT_BC5_UNORM;
		case EColorFormat::BC5_Signed_Norm:   return DXGI_FORMAT_BC5_SNORM;
		case EColorFormat::BC7_Unsigned_Norm: return DXGI_FORMAT_BC7_UNORM;

		default: ASSERT_MSG(false, "Should never be hit"); return DXGI_FORMAT_UNKNOWN;
		}
	}

	DXGI_FORMAT D3DFormat(EDepthStencilFormat type)
	{
		switch (type)
		{
		case EDepthStencilFormat::Depth24_Float_Stencil8_Unsigned: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case EDepthStencilFormat::Depth32_Float:                   return DXGI_FORMAT_D32_FLOAT;
		default: ASSERT_MSG(false, "Should never be hit"); return DXGI_FORMAT_UNKNOWN;
		}
	}

	DXGI_FORMAT D3DFormat(FFormatVariant type)
	{
		DXGI_FORMAT d3dFormat{};
		std::visit([&d3dFormat](auto&& concreteFormat) {d3dFormat = D3DFormat(concreteFormat); }, type);
		return d3dFormat;
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

	std::pair<DXGI_FORMAT, std::optional<DXGI_FORMAT>> D3DDepthStecilShaderAccessFormats(EDepthStencilFormat type)
	{
		switch (type)
		{
		case EDepthStencilFormat::Depth24_Float_Stencil8_Unsigned: return { DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_X24_TYPELESS_G8_UINT };
		case EDepthStencilFormat::Depth32_Float:                   return { DXGI_FORMAT_R32_FLOAT, std::nullopt };
		default: ASSERT_MSG(false, "Should never be hit"); return {};
		}
	}

	FFormatVariant FormatFromD3DFormat(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8_TYPELESS: return ETypelessColorFormat::R8;
		case DXGI_FORMAT_R8G8_TYPELESS: return ETypelessColorFormat::RG8;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS: return ETypelessColorFormat::RGBA8;
		case DXGI_FORMAT_R16_TYPELESS: return ETypelessColorFormat::R16;
		case DXGI_FORMAT_R16G16_TYPELESS: return ETypelessColorFormat::RG16;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS: return ETypelessColorFormat::RGBA16;
		case DXGI_FORMAT_R32_TYPELESS: return ETypelessColorFormat::R32;
		case DXGI_FORMAT_R32G32_TYPELESS: return ETypelessColorFormat::RG32;
		case DXGI_FORMAT_R32G32B32_TYPELESS: return ETypelessColorFormat::RGB32;
		case DXGI_FORMAT_R32G32B32A32_TYPELESS: return ETypelessColorFormat::RGBA32;

		case DXGI_FORMAT_B8G8R8A8_UNORM: return EColorFormat::BGRA8_Unsigned_Norm;

		case DXGI_FORMAT_R8_UNORM: return EColorFormat::R8_Unsigned_Norm;
		case DXGI_FORMAT_R8G8_UNORM: return EColorFormat::RG8_Usigned_Norm;
		case DXGI_FORMAT_R8G8B8A8_UNORM: return EColorFormat::RGBA8_Unsigned_Norm;
		case DXGI_FORMAT_R16G16B16A16_UNORM: return EColorFormat::RGBA16_Unsigned_Norm;

		case DXGI_FORMAT_R8_SINT: return EColorFormat::R8_Signed;
		case DXGI_FORMAT_R8G8_SINT: return EColorFormat::RG8_Signed;
		case DXGI_FORMAT_R8G8B8A8_SINT: return EColorFormat::RGBA8_Signed;

		case DXGI_FORMAT_R8_UINT: return EColorFormat::R8_Unsigned;
		case DXGI_FORMAT_R8G8_UINT: return EColorFormat::RG8_Unsigned;
		case DXGI_FORMAT_R8G8B8A8_UINT: return EColorFormat::RGBA8_Unsigned;

		case DXGI_FORMAT_R16_FLOAT: return EColorFormat::R16_Float;
		case DXGI_FORMAT_R16G16_FLOAT: return EColorFormat::RG16_Float;
		case DXGI_FORMAT_R16G16B16A16_FLOAT: return EColorFormat::RGBA16_Float;

		case DXGI_FORMAT_R16_SINT: return EColorFormat::R16_Signed;
		case DXGI_FORMAT_R16G16_SINT: return EColorFormat::RG16_Signed;
		case DXGI_FORMAT_R16G16B16A16_SINT: return EColorFormat::RGBA16_Signed;

		case DXGI_FORMAT_R16_UINT: return EColorFormat::R16_Unsigned;
		case DXGI_FORMAT_R16G16_UINT: return EColorFormat::RG16_Unsigned;
		case DXGI_FORMAT_R16G16B16A16_UINT: return EColorFormat::RGBA16_Unsigned;

		case DXGI_FORMAT_R32_FLOAT: return EColorFormat::R32_Float;
		case DXGI_FORMAT_R32G32_FLOAT: return EColorFormat::RG32_Float;
		case DXGI_FORMAT_R32G32B32_FLOAT: return EColorFormat::RGB32_Float;
		case DXGI_FORMAT_R32G32B32A32_FLOAT: return EColorFormat::RGBA32_Float;

		case DXGI_FORMAT_R32_SINT: return EColorFormat::R32_Signed;
		case DXGI_FORMAT_R32G32_SINT: return EColorFormat::RG32_Signed;
		case DXGI_FORMAT_R32G32B32_SINT: return EColorFormat::RGB32_Signed;
		case DXGI_FORMAT_R32G32B32A32_SINT: return EColorFormat::RGBA32_Signed;

		case DXGI_FORMAT_R32_UINT: return EColorFormat::R32_Unsigned;
		case DXGI_FORMAT_R32G32_UINT: return EColorFormat::RG32_Unsigned;
		case DXGI_FORMAT_R32G32B32_UINT: return EColorFormat::RGB32_Unsigned;
		case DXGI_FORMAT_R32G32B32A32_UINT: return EColorFormat::RGBA32_Unsigned;

		case DXGI_FORMAT_D24_UNORM_S8_UINT: return EDepthStencilFormat::Depth24_Float_Stencil8_Unsigned;
		case DXGI_FORMAT_D32_FLOAT: return EDepthStencilFormat::Depth32_Float;

		case DXGI_FORMAT_R10G10B10A2_UNORM: return EColorFormat::RGB10A2_Unorm;
		case DXGI_FORMAT_R11G11B10_FLOAT: return EColorFormat::R11G11B10_Float;

			// Compressed formats
		case DXGI_FORMAT_BC1_UNORM: return EColorFormat::BC1_Unsigned_Norm;
		case DXGI_FORMAT_BC2_UNORM: return EColorFormat::BC2_Unsigned_Norm;
		case DXGI_FORMAT_BC3_UNORM: return EColorFormat::BC3_Unsigned_Norm;
		case DXGI_FORMAT_BC4_UNORM: return EColorFormat::BC4_Unsigned_Norm;
		case DXGI_FORMAT_BC5_UNORM: return EColorFormat::BC5_Unsigned_Norm;
		case DXGI_FORMAT_BC5_SNORM: return EColorFormat::BC5_Signed_Norm;
		case DXGI_FORMAT_BC7_UNORM: return EColorFormat::BC7_Unsigned_Norm;

		default:
			ASSERT_MSG(false, "Unsupported D3D format");
			return ETypelessColorFormat::R8;
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
}


