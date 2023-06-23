#pragma once

#include <dxgiformat.h>
#include <dxgicommon.h>
#include <variant>
#include <optional>
#include "Math/MathType.h"
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

	enum class EResourceFormat : uint32_t
	{
		Unknown,

		// Color Formats
		R8_Unsigned_Norm, RG8_Usigned_Norm, RGBA8_Unsigned_Norm, RGBA16_Unsigned_Norm,

		RGBA8_Unsigned_Norm_Srgb, BGRA8_Unsigned_Norm_Srgb,

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

		BC1_Unsigned_Norm_Srgb, BC2_Unsigned_Norm_Srgb, BC3_Unsigned_Norm_Srgb, BC7_Unsigned_Norm_Srgb,

		// Typeless Color Format
		R8_Typeless, RG8_Typeless, RGBA8_Typeless, BGRA8_Typeless,
		R16_Typeless, RG16_Typeless, RGBA16_Typeless,
		R24G8_Typeless, R32G8X24_Typeless,
		R32_Typeless, RG32_Typeless, RGB32_Typeless, RGBA32_Typeless,

		// Depth Stencil Format 
		Depth16_Float, Depth24_Float_Stencil8_Unsigned, Depth32_Float, Depth32_Float_Stencil8_Unsigned
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

	DXGI_FORMAT D3DBaseFormat(DXGI_FORMAT format);
	DXGI_FORMAT D3DUnorderedAccessViewFormat(DXGI_FORMAT format);
	DXGI_FORMAT D3DDepthStencilViewFormat(DXGI_FORMAT format);
	DXGI_FORMAT D3DDepthFormat(DXGI_FORMAT format);
	DXGI_FORMAT D3DStencilFormat(DXGI_FORMAT format);

	DXGI_FORMAT D3DFormat(EResourceFormat format);
	DXGI_COLOR_SPACE_TYPE D3DColorSpace(EColorSpace space);

	DXGI_FORMAT MakeSRGB(DXGI_FORMAT format) noexcept;

	EResourceFormat BaseFormat(EResourceFormat format);

	bool IsDepthStencilFormat(DXGI_FORMAT format);
	bool IsDepthStencilFormat(EResourceFormat format);

	bool IsColorFormat(EResourceFormat format);

	bool IsCompressedFormat(DXGI_FORMAT format);
	bool IsCompressedFormat(EResourceFormat format);

	DXGI_FORMAT EnsureNotTypelessFormat(DXGI_FORMAT format);
	EResourceFormat EnsureNotTypelessFormat(EResourceFormat format);

	EFormatSupport CheckFormatSupport(DXGI_FORMAT format);
	EFormatSupport CheckFormatSupport(EResourceFormat format);

	std::pair<DXGI_FORMAT, std::optional<DXGI_FORMAT>> D3DDepthStecilShaderAccessFormats(EResourceFormat format);

	EResourceFormat ResourceFormatFromD3DFormat(DXGI_FORMAT format);
	EColorSpace ColorSpaceFromD3DSpace(DXGI_COLOR_SPACE_TYPE space);

	size_t BitsPerPixel(DXGI_FORMAT format);

	size_t BytesPerPixel(DXGI_FORMAT format);
	size_t BytesPerPixel(EResourceFormat format);

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

	template<typename T>
	FORCEINLINE EResourceFormat GetFormatForType()
	{
		ASSERT_FAIL("Invalid Format Type!");
		return EResourceFormat::UnKwon;
	}

	template<>
	FORCEINLINE EResourceFormat GetFormatForType<float>()
	{
		return EResourceFormat::R32_Float;
	}

	template<>
	FORCEINLINE EResourceFormat GetFormatForType<unsigned short>()
	{
		return EResourceFormat::R16_Unsigned;
	}

	template<>
	FORCEINLINE EResourceFormat GetFormatForType<unsigned int>()
	{
		return EResourceFormat::R32_Unsigned;
	}

	template<>
	FORCEINLINE EResourceFormat GetFormatForType<FVector2f>()
	{
		return EResourceFormat::RG32_Float;
	}

	template<>
	FORCEINLINE EResourceFormat GetFormatForType<FVector3f>()
	{
		return EResourceFormat::RGB32_Float;
	}

	template<>
	FORCEINLINE EResourceFormat GetFormatForType<FVector4f>()
	{
		return EResourceFormat::RGBA32_Float;
	}

	template<>
	FORCEINLINE EResourceFormat GetFormatForType<FColor>()
	{
		return EResourceFormat::RGBA8_Unsigned;
	}

	template<>
	FORCEINLINE EResourceFormat GetFormatForType<FLinearColor>()
	{
		return EResourceFormat::RGBA32_Float;
	}
}