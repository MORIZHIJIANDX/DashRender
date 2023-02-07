#pragma once

#include "ResourceFormat.h"

namespace Dash
{
	struct FResourceDescription
	{
	public:
		D3D12_RESOURCE_DESC D3DResourceDescription() const { return mDescription; }
		uint64_t SubresourceCount() const { return mSubresourceCount; }
		uint64_t ResourceAlignment() const { return mResourceAlignment; }
		uint64_t ResourceSizeInBytes() const { return mResourceSizeInBytes; }

	protected:
		void QueryAllocationInfo();
		virtual void ResolveResourceDimensionData(bool allowUAV = true, bool allowRTV = true) {};

	public:
		EResourceState InitialStateMask = EResourceState::Common;

	protected:
		D3D12_RESOURCE_DESC mDescription{};
		uint64_t mSubresourceCount = 1;
		uint64_t mResourceAlignment = 0;
		uint64_t mResourceSizeInBytes = 0;
	};

	struct FTextureDescription : public FResourceDescription
	{
	public:
		EResourceFormat Format = EResourceFormat::Unknown;
		ETextureDimension Dimension = ETextureDimension::Texture1D;
		FResourceMagnitude Magnitude;
		uint32_t MipCount = 1;
		uint32_t MsaaSampleCount = 1;
		uint32_t MsaaQuality = 1;

		FResourceMagnitude ComputeMipSize(uint8_t mip) const;

	protected:
		virtual void ResolveResourceDimensionData(bool allowUAV, bool allowRTV) override;
		uint32_t ComputeNumMips() const;
	};

	struct FColorBufferDescription : public FTextureDescription
	{
	public:
		FLinearColor ClearValue;

		static FColorBufferDescription Create(EResourceFormat format, ETextureDimension dimension, const FResourceMagnitude& magnitude,
			const FLinearColor& optimizedClearValue, uint32_t mipCount = 1, EResourceState initialStateMask = EResourceState::Common, uint32_t sampleCount = 1, uint32_t msaaQuality = 0);

		static FColorBufferDescription Create1D(EResourceFormat format, uint32_t width, const FLinearColor& optimizedClearValue, uint32_t mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
		static FColorBufferDescription Create2D(EResourceFormat format, uint32_t width, uint32_t height, const FLinearColor& optimizedClearValue, uint32_t mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
		static FColorBufferDescription Create2DArray(EResourceFormat format, uint32_t width, uint32_t height, uint32_t arraySize, const FLinearColor& optimizedClearValue, uint32_t mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
		static FColorBufferDescription Create3D(EResourceFormat format, uint32_t width, uint32_t height, uint32_t depth, const FLinearColor& optimizedClearValue, uint32_t mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
	};

	struct FTextureBufferDescription : public FTextureDescription
	{
	public:
		static FTextureBufferDescription Create(EResourceFormat format, ETextureDimension dimension, const FResourceMagnitude& magnitude,
			uint32_t mipCount = 1, EResourceState initialStateMask = EResourceState::Common);

		static FTextureBufferDescription Create1D(EResourceFormat format, uint32_t width, uint32_t mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
		static FTextureBufferDescription Create2D(EResourceFormat format, uint32_t width, uint32_t height, uint32_t mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
		static FTextureBufferDescription Create2DArray(EResourceFormat format, uint32_t width, uint32_t height, uint32_t arraySize, uint32_t mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
		static FTextureBufferDescription Create3D(EResourceFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
	};

	struct FDepthBufferDescription : public FTextureDescription
	{
	public:
		FDepthStencilClearValue ClearValue{};

		static FDepthBufferDescription Create(EResourceFormat format, uint32_t width, uint32_t height,
			const FDepthStencilClearValue& optimizedClearValue, uint32_t mipCount = 1, EResourceState initialStateMask = EResourceState::Common, uint32_t sampleCount = 1, uint32_t msaaQuality = 0);
	};

	struct FBufferDescription : public FResourceDescription
	{
	public:
		uint64_t Size = 1;
		uint64_t Count = 1;
		uint64_t Stride = 1;

		static FBufferDescription Create(uint64_t elementSize, uint64_t elementCount, uint64_t elementAlignment = 1, EResourceState initialStateMask = EResourceState::GenericRead);

		template<typename ElementType>
		static FBufferDescription Create(uint64_t elementCount, uint64_t elementAlignment = 1, EResourceState initialStateMask = EResourceState::GenericRead);

	protected:
		virtual void ResolveResourceDimensionData(bool allowUAV, bool allowRTV) override;
	};

	template<typename ElementType>
	FBufferDescription FBufferDescription::Create(uint64_t elementCount, uint64_t elementAlignment /*= 1*/, EResourceState initialStateMask /*= EResourceState::Common*/)
	{
		return Create(sizeof(ElementType), elementCount, elementAlignment, initialStateMask);
	}
}