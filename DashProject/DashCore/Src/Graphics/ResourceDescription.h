#pragma once

#include "ResourceFormat.h"

namespace Dash
{
	struct FTextureProperties
	{
		EResourceFormat Format;
		ETextureDimension Dimension;
		FResourceMagnitude Magnitude;
		FClearValue OptimizedClearValue;
		EResourceState InitialStateMask;
		uint32_t MipCount;

		static FTextureProperties CreateTextureProperties(EResourceFormat format, ETextureDimension dimension, const FResourceMagnitude& magnitude,
			const FClearValue& optimizedClearValue, EResourceState initialStateMask = EResourceState::Common, uint32_t mipCount = 1);

		static FTextureProperties CreateTextureProperties1D(EResourceFormat format, uint32_t width, const FClearValue& optimizedClearValue, EResourceState initialStateMask = EResourceState::Common, uint32_t mipCount = 1);
		static FTextureProperties CreateTextureProperties2D(EResourceFormat format, uint32_t width, uint32_t height, const FClearValue& optimizedClearValue, EResourceState initialStateMask = EResourceState::Common, uint32_t mipCount = 1);
		static FTextureProperties CreateTextureProperties3D(EResourceFormat format, uint32_t width, uint32_t height, uint32_t depth, const FClearValue& optimizedClearValue, EResourceState initialStateMask = EResourceState::Common, uint32_t mipCount = 1);

		FResourceMagnitude MipSize(uint8_t mip) const;
	};

	struct FBufferProperties
	{
		uint64_t Size = 1;
		uint64_t Stride = 1;
		EResourceState InitialStateMask;

		template<typename ElementType>
		static FBufferProperties Create(uint64_t elementCount, uint64_t elementAlignment = 1, EResourceState initialStateMask = EResourceState::Common);
	};

	template<typename ElementType>
	FBufferProperties FBufferProperties::Create(uint64_t elementCount, uint64_t elementAlignment /*= 1*/, EResourceState initialStateMask /*= EResourceState::Common*/)
	{
		FBufferProperties properties;
		properties.Stride = FMath::AlignUp(sizeof(ElementType), elementAlignment);
		properties.Size = properties.Stride * elementCount;
		properties.InitialStateMask = initialStateMask;

		return properties;
	}

	using FResourcePropertiesVariant = std::variant<FTextureProperties, FBufferProperties>;

	class FResourceDescription
	{
	public:
		FResourceDescription(const FTextureProperties& properties);
		FResourceDescription(const FBufferProperties& properties);
		
	public:
		D3D12_RESOURCE_DESC D3DResourceDescription() const { return mDescription; }


	private:
		D3D12_RESOURCE_DESC mDescription;
		FResourcePropertiesVariant mResourceProperties;
		uint64_t mSubresourceCount = 1;
		uint64_t mResourceAlignment = 0;
		uint64_t mResourceSizeInBytes = 0;
	};
}