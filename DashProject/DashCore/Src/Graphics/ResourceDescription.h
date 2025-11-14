#pragma once

#include "ResourceFormat.h"
#include "GraphicsDefines.h"

namespace Dash
{
	struct FResourceDescription
	{
	public:
		D3D12_RESOURCE_DESC D3DResourceDescription() const { return mDescription; }
		uint32 SubresourceCount() const { return mSubresourceCount; }
		uint32 ResourceAlignment() const { return mResourceAlignment; }
		uint32 ResourceSizeInBytes() const { return mResourceSizeInBytes; }
		void GetPitch(size_t& rowPitch, size_t slicePitch, size_t mipIndex = 0) const;

	protected:
		void QueryAllocationInfo();
		virtual void ResolveResourceDimensionData(bool allowUAV = true, bool allowRTV = true, bool allowDSV = false) {};

	protected:
		D3D12_RESOURCE_DESC mDescription{};
		uint32 mSubresourceCount = 1;
		uint32 mResourceAlignment = 0;
		uint32 mResourceSizeInBytes = 0;
	};

	struct FTextureDescription : public FResourceDescription
	{
	public:
		EResourceFormat Format = EResourceFormat::Unknown;
		ETextureDimension Dimension = ETextureDimension::Texture1D;
		FResourceMagnitude Magnitude;
		uint32 MipCount = 1;
		uint32 MsaaSampleCount = 1;
		uint32 MsaaQuality = 1;

		FResourceMagnitude ComputeMipSize(uint8 mip) const;
		
	protected:
		virtual void ResolveResourceDimensionData(bool allowUAV, bool allowRTV, bool allowDSV) override;
		uint32 ComputeNumMips() const;
	};

	struct FColorBufferDescription : public FTextureDescription
	{
	public:
		FLinearColor ClearValue;

		static FColorBufferDescription Create(EResourceFormat format, ETextureDimension dimension, const FResourceMagnitude& magnitude,
			const FLinearColor& optimizedClearValue, uint32 mipCount = 1, EResourceState initialStateMask = EResourceState::Common, uint32 sampleCount = 1, uint32 msaaQuality = 0);

		static FColorBufferDescription Create1D(EResourceFormat format, uint32 width, const FLinearColor& optimizedClearValue, uint32 mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
		static FColorBufferDescription Create2D(EResourceFormat format, uint32 width, uint32 height, const FLinearColor& optimizedClearValue, uint32 mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
		static FColorBufferDescription Create2DArray(EResourceFormat format, uint32 width, uint32 height, uint32 arraySize, const FLinearColor& optimizedClearValue, uint32 mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
		static FColorBufferDescription Create3D(EResourceFormat format, uint32 width, uint32 height, uint32 depth, const FLinearColor& optimizedClearValue, uint32 mipCount = 1, EResourceState initialStateMask = EResourceState::Common);
	};

	struct FTextureBufferDescription : public FTextureDescription
	{
	public:
		static FTextureBufferDescription Create(EResourceFormat format, ETextureDimension dimension, const FResourceMagnitude& magnitude,
			uint32 mipCount = 1);

		static FTextureBufferDescription Create1D(EResourceFormat format, uint32 width, uint32 mipCount = 1);
		static FTextureBufferDescription Create2D(EResourceFormat format, uint32 width, uint32 height, uint32 mipCount = 1);
		static FTextureBufferDescription Create2DArray(EResourceFormat format, uint32 width, uint32 height, uint32 arraySize, uint32 mipCount = 1);
		static FTextureBufferDescription Create3D(EResourceFormat format, uint32 width, uint32 height, uint32 depth, uint32 mipCount = 1);

	public:
		bool AutoGenerateMips = false;
	};

	struct FDepthBufferDescription : public FTextureDescription
	{
	public:
		FDepthStencilClearValue ClearValue{};

		static FDepthBufferDescription Create(EResourceFormat format, uint32 width, uint32 height,
			const FDepthStencilClearValue& optimizedClearValue, uint32 mipCount = 1, EResourceState initialStateMask = EResourceState::Common, uint32 sampleCount = 1, uint32 msaaQuality = 0);
	};

	struct FBufferDescription : public FResourceDescription
	{
	public:
		uint32 Size = 1;
		uint32 Stride = 1;
		EBufferUsage Usage = EBufferUsage::Default;
		EResourceBindFlag BindFlags = EResourceBindFlag::None;
		EBufferMiscFlag MiscFlags = EBufferMiscFlag::None;
		EResourceFormat Format = EResourceFormat::Unknown;

		static FBufferDescription Create(uint32 size, uint32 stride, EBufferUsage usage, EResourceBindFlag bindFlags, EBufferMiscFlag miscFlags, EResourceFormat format);

		static FBufferDescription CreateVertex(uint32 elementSize, uint32 elementCount);
		template<typename TVertexType>
		static FBufferDescription CreateVertex(uint32 elementCount)
		{
			CreateVertex(sizeof(TVertexType), elementCount);
		}

		static FBufferDescription CreateDynamicVertex(uint32 elementSize, uint32 elementCount);
		template<typename TVertexType>
		static FBufferDescription CreateDynamicVertex(uint32 elementCount)
		{
			CreateDynamicVertex(sizeof(TVertexType), elementCount);
		}

		static FBufferDescription CreateIndex(uint32 size, uint32 stride);
		template<typename TIndexType>
		static FBufferDescription CreateIndex(uint32 elementCount)
		{
			CreateIndex(elementCount * sizeof(TIndexType), sizeof(TIndexType));
		}

		static FBufferDescription CreateDynamicIndexBuffer(uint32 size, uint32 stride);
		template<typename TIndexType>
		static FBufferDescription CreateDynamicIndexBuffer(uint32 elementCount)
		{
			CreateDynamicIndexBuffer(elementCount * sizeof(TIndexType), sizeof(TIndexType));
		}

		static FBufferDescription CreateStructured(uint32 elementSize, uint32 elementCount, bool uav = false, bool dynamic = false);
		template<typename TStructureType>
		static FBufferDescription CreateStructured(uint32 elementCount, bool uav = false, bool dynamic = false)
		{
			CreateStructured(sizeof(TStructureType), elementCount, uav, dynamic);
		}

		static FBufferDescription CreateCounter();

	protected:
		virtual void ResolveResourceDimensionData(bool allowUAV, bool allowRTV, bool allowDSV = false) override;
	};
}