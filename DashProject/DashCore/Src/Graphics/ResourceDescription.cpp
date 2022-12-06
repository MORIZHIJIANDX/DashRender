#include "PCH.h"
#include "ResourceDescription.h"

namespace Dash
{
	FTextureProperties FTextureProperties::CreateTextureProperties(FFormatVariant format, ETextureDimension dimension, const FResourceMagnitude& magnitude, const FClearValue& optimizedClearValue, EResourceState initialStateMask, uint32_t mipCount)
	{
		FTextureProperties properties;
		properties.Format = format;
		properties.Dimension = dimension;
		properties.Magnitude = magnitude;
		properties.OptimizedClearValue = optimizedClearValue;
		properties.InitialStateMask = initialStateMask;
		properties.MipCount = mipCount;

		return properties;
	}

	FTextureProperties FTextureProperties::CreateTextureProperties1D(FFormatVariant format, uint32_t width, const FClearValue& optimizedClearValue, EResourceState initialStateMask, uint32_t mipCount)
	{
		return CreateTextureProperties(format, ETextureDimension::Texture1D, FResourceMagnitude{ width }, optimizedClearValue, EResourceState::Common, mipCount);
	}

	FTextureProperties FTextureProperties::CreateTextureProperties2D(FFormatVariant format, uint32_t width, uint32_t height, const FClearValue& optimizedClearValue, EResourceState initialStateMask /*= EResourceState::Common*/, uint32_t mipCount /*= 1*/)
	{
		return CreateTextureProperties(format, ETextureDimension::Texture2D, FResourceMagnitude{ width, height }, optimizedClearValue, EResourceState::Common, mipCount);
	}

	FTextureProperties FTextureProperties::CreateTextureProperties3D(FFormatVariant format, uint32_t width, uint32_t height, uint32_t depth, const FClearValue& optimizedClearValue, EResourceState initialStateMask /*= EResourceState::Common*/, uint32_t mipCount /*= 1*/)
	{
		return CreateTextureProperties(format, ETextureDimension::Texture2D, FResourceMagnitude{ width, height, depth }, optimizedClearValue, EResourceState::Common, mipCount);
	}

	FResourceMagnitude FTextureProperties::MipSize(uint8_t mip) const
	{
		FResourceMagnitude mipMagnitude = Magnitude.XYZMultiplied(1.0f / FMath::Pow(Scalar(2), Scalar(mip)));
		mipMagnitude.Depth = FMath::Max(mipMagnitude.Depth, uint32_t(1));
		return mipMagnitude;
	}
}