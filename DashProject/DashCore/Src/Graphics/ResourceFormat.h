#pragma once

#include <dxgiformat.h>
#include <dxgicommon.h>
#include "ResourceState.h"

namespace Dash
{
	using FColorClearValue = FVector4f;

	struct FDepthStencilClearValue
	{
		float Depth;
		uint8_t Stencil;
	};

	using FClearValue = std::variant<FColorClearValue, FDepthStencilClearValue>;

	enum class ETypelessColorFormat : uint32_t
	{
		R8, RG8, RGBA8,
		R16, RG16, RGBA16,
		R32, RG32, RGB32, RGBA32
	};

	enum class EColorFormat : uint32_t
	{
		R8_Unsigned_Norm, RG8_Usigned_Norm, RGBA8_Unsigned_Norm, RGBA16_Unsigned_Norm,

		BGRA8_Unsigned_Norm,

		R8_Signed, RG8_Signed, RGBA8_Signed,
		R8_Unsigned, RG8_Unsigned, RGBA8_Unsigned,

		R16_Float, RG16_Float, RGBA16_Float,
		R16_Signed, RG16_Signed, RGBA16_Signed,
		R16_Unsigned, RG16_Unsigned, RGBA16_Unsigned,

		R32_Float, RG32_Float, RGB32_Float, RGBA32_Float,
		R32_Signed, RG32_Signed, RGB32_Signed, RGBA32_Signed,
		R32_Unsigned, RG32_Unsigned, RGB32_Unsigned, RGBA32_Unsigned,

		RGB10A2_Unorm, R11G11B10_Float,

		// Compressed formats
		BC1_Unsigned_Norm, BC2_Unsigned_Norm, BC3_Unsigned_Norm, BC4_Unsigned_Norm,
		BC5_Unsigned_Norm, BC5_Signed_Norm, BC7_Unsigned_Norm
	};

	enum class EResourceFormat : uint32_t
	{
		Unknown,

		// Color Formats
		R8_Unsigned_Norm, RG8_Usigned_Norm, RGBA8_Unsigned_Norm, RGBA16_Unsigned_Norm,

		BGRA8_Unsigned_Norm,

		R8_Signed, RG8_Signed, RGBA8_Signed,
		R8_Unsigned, RG8_Unsigned, RGBA8_Unsigned,

		R16_Float, RG16_Float, RGBA16_Float,
		R16_Signed, RG16_Signed, RGBA16_Signed,
		R16_Unsigned, RG16_Unsigned, RGBA16_Unsigned,

		R32_Float, RG32_Float, RGB32_Float, RGBA32_Float,
		R32_Signed, RG32_Signed, RGB32_Signed, RGBA32_Signed,
		R32_Unsigned, RG32_Unsigned, RGB32_Unsigned, RGBA32_Unsigned,

		RGB10A2_Unorm, R11G11B10_Float,

		// Compressed Color Formats
		BC1_Unsigned_Norm, BC2_Unsigned_Norm, BC3_Unsigned_Norm, BC4_Unsigned_Norm,
		BC5_Unsigned_Norm, BC5_Signed_Norm, BC7_Unsigned_Norm,

		// Typeless Color Format
		R8_Typeless, RG8_Typeless, RGBA8_Typeless,
		R16_Typeless, RG16_Typeless, RGBA16_Typeless,
		R32_Typeless, RG32_Typeless, RGB32_Typeless, RGBA32_Typeless,

		// Depth Stencil Format 
		Depth24_Float_Stencil8_Unsigned, Depth32_Float
	};

	enum class EDepthStencilFormat : uint32_t
	{
		Depth24_Float_Stencil8_Unsigned, Depth32_Float
	};

	enum class EColorSpace : uint8_t
	{
		Rec709, Rec2020
	};

	enum class ETextureDimension : uint8_t
	{
		Texture1D, Texture2D, Texture3D
	};

	enum class EFormatSupport : uint16_t
	{
		Invalid = 0,
		Buffer = 1 << 1,
		ShaderResourceView = 1 << 2,
		RenderTargetView = 1 << 3,
		UnorderAccessView = 1 << 4,
		DepthStencilView = 1 << 5
	};
	ENABLE_BITMASK_OPERATORS(EFormatSupport);

	using FFormatVariant = std::variant<ETypelessColorFormat, EColorFormat, EDepthStencilFormat>;

	DXGI_FORMAT D3DTypelessFormat(DXGI_FORMAT format);
	DXGI_FORMAT D3DUnorderedAccessViewFormat(DXGI_FORMAT format);
	DXGI_FORMAT D3DDepthStencilViewFormat(DXGI_FORMAT format);
	DXGI_FORMAT D3DDepthFormat(DXGI_FORMAT format);
	DXGI_FORMAT D3DStencilFormat(DXGI_FORMAT format);

	DXGI_FORMAT D3DFormat(ETypelessColorFormat type);
	DXGI_FORMAT D3DFormat(EColorFormat type);
	DXGI_FORMAT D3DFormat(EDepthStencilFormat type);
	DXGI_FORMAT D3DFormat(FFormatVariant type);

	DXGI_FORMAT D3DFormat(EResourceFormat format);
	DXGI_COLOR_SPACE_TYPE D3DColorSpace(EColorSpace space);

	bool IsDepthStencilFormat(DXGI_FORMAT format);
	bool IsCompressedFormat(DXGI_FORMAT format);
	DXGI_FORMAT EnsureNotTypelessFormat(DXGI_FORMAT format);

	EFormatSupport CheckFormatSupport(DXGI_FORMAT format);
	EFormatSupport CheckFormatSupport(FFormatVariant format);
	EFormatSupport CheckFormatSupport(EResourceFormat format);

	std::pair<DXGI_FORMAT, std::optional<DXGI_FORMAT>> D3DDepthStecilShaderAccessFormats(EDepthStencilFormat type);

	FFormatVariant FormatFromD3DFormat(DXGI_FORMAT format);
	EColorSpace ColorSpaceFromD3DSpace(DXGI_COLOR_SPACE_TYPE space);

	size_t BytesPerPixel(DXGI_FORMAT format);
	size_t BytesPerPixel(ETypelessColorFormat format);
	size_t BytesPerPixel(EColorFormat format);
	size_t BytesPerPixel(EDepthStencilFormat format);
	size_t BytesPerPixel(FFormatVariant format);

	struct FResourceMagnitude
	{
		FResourceMagnitude(uint32_t width, uint32_t height, uint32_t depth);
		FResourceMagnitude(uint32_t width, uint32_t height);
		FResourceMagnitude(uint32_t width);
		FResourceMagnitude() = default;

		uint32_t LargestMagnitude() const;
		bool operator==(const FResourceMagnitude& rhs);
		bool operator!=(const FResourceMagnitude& rhs);

		FResourceMagnitude XMultiplied(float m) const;
		FResourceMagnitude XYMultiplied(float m) const;
		FResourceMagnitude XYZMultiplied(float m) const;

		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Depth = 1;
	};
}