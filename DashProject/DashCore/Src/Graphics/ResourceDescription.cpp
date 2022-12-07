#include "PCH.h"
#include "ResourceDescription.h"
#include "GraphicsCore.h"

namespace Dash
{
	void FResourceDescription::QueryAllocationInfo()
	{
		if (mDescription.Width == 0)
		{
			return;
		}

		D3D12_RESOURCE_ALLOCATION_INFO allocInfo = FGraphicsCore::Device->GetResourceAllocationInfo(0, 1, &mDescription);
		
		mResourceAlignment = allocInfo.Alignment;
		mResourceSizeInBytes = allocInfo.SizeInBytes;
		mDescription.Alignment = mResourceAlignment;
	}

	FResourceMagnitude FTextureDescription::MipSize(uint8_t mip) const
	{
		FResourceMagnitude mipMagnitude = Magnitude.XYZMultiplied(1.0f / FMath::Pow(Scalar(2), Scalar(mip)));
		mipMagnitude.Depth = FMath::Max(mipMagnitude.Depth, uint32_t(1));
		return mipMagnitude;
	}

	void FTextureDescription::ResolveResourceDimensionData()
	{
		switch (Dimension)
		{
		case ETextureDimension::Texture1D:
			mDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
			break;
		case ETextureDimension::Texture2D:
			mDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			break;
		case ETextureDimension::Texture3D:
			mDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			break;
		default:
			break;
		}

		bool isArray = (Dimension == ETextureDimension::Texture1D || Dimension == ETextureDimension::Texture3D) && Magnitude.Depth > 1;
		mSubresourceCount = isArray ? Magnitude.Depth * MipCount : MipCount;

		mDescription.Width = Magnitude.Width;
		mDescription.Height = Magnitude.Height;
		mDescription.DepthOrArraySize = Magnitude.Depth;
		mDescription.SampleDesc.Count = MsaaSampleCount;
		mDescription.SampleDesc.Quality = MsaaQuality;
		mDescription.MipLevels = MipCount;
		mDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		mDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (MsaaSampleCount == 1)
		{
			mDescription.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		QueryAllocationInfo();
	}

	FColorBufferDescription FColorBufferDescription::Create(EResourceFormat format, ETextureDimension dimension, const FResourceMagnitude& magnitude, const FClearValue& optimizedClearValue, uint32_t mipCount /*= 1*/,
			EResourceState initialStateMask /*= EResourceState::Common*/, uint32_t sampleCount /*= 1*/, uint32_t msaaQuality /*= 0*/)
	{
		FColorBufferDescription desc;
		desc.Dimension = dimension;
		desc.Format = format;
		desc.InitialStateMask = initialStateMask;
		desc.Magnitude = magnitude;
		desc.MipCount = mipCount;
		desc.MsaaSampleCount = sampleCount;
		desc.MsaaQuality = msaaQuality;

		desc.ResolveResourceDimensionData();

		return desc;
	}

	FColorBufferDescription FColorBufferDescription::Create1D(EResourceFormat format, uint32_t width, const FVector4f& optimizedClearValue, uint32_t mipCount /*= 1*/, EResourceState initialStateMask /*= EResourceState::Common*/)
	{
		return FColorBufferDescription::Create(format, ETextureDimension::Texture1D, FResourceMagnitude(width), optimizedClearValue, mipCount, initialStateMask);
	}

	FColorBufferDescription FColorBufferDescription::Create2D(EResourceFormat format, uint32_t width, uint32_t height, const FVector4f& optimizedClearValue, uint32_t mipCount /*= 1*/, EResourceState initialStateMask /*= EResourceState::Common*/)
	{
		return FColorBufferDescription::Create(format, ETextureDimension::Texture2D, FResourceMagnitude(width, height), optimizedClearValue, mipCount, initialStateMask);
	}

	FColorBufferDescription FColorBufferDescription::Create3D(EResourceFormat format, uint32_t width, uint32_t height, uint32_t depth, const FVector4f& optimizedClearValue, uint32_t mipCount /*= 1*/, EResourceState initialStateMask /*= EResourceState::Common*/)
	{
		return FColorBufferDescription::Create(format, ETextureDimension::Texture3D, FResourceMagnitude(width, height, depth), optimizedClearValue, mipCount, initialStateMask);
	}

	FDepthBufferDescription FDepthBufferDescription::Create(EResourceFormat format, uint32_t width, uint32_t height, const FDepthStencilClearValue& optimizedClearValue, uint32_t mipCount /*= 1*/, 
		EResourceState initialStateMask /*= EResourceState::Common*/, uint32_t sampleCount /*= 1*/, uint32_t msaaQuality /*= 0*/)
	{
		FDepthBufferDescription desc;
		desc.Dimension = ETextureDimension::Texture2D;
		desc.Format = format;
		desc.InitialStateMask = initialStateMask;
		desc.Magnitude = FResourceMagnitude(width, height);
		desc.MipCount = mipCount;
		desc.MsaaSampleCount = sampleCount;
		desc.MsaaQuality = msaaQuality;

		desc.ResolveResourceDimensionData();

		return desc;
	}

	Dash::FBufferDescription FBufferDescription::Create(uint64_t elementSize, uint64_t elementCount, uint64_t elementAlignment /*= 1*/, EResourceState initialStateMask /*= EResourceState::Common*/)
	{
		ASSERT(elementSize > 0);

		FBufferDescription properties;
		properties.Stride = FMath::AlignUp(elementSize, elementAlignment);
		properties.Size = properties.Stride * elementCount;
		properties.InitialStateMask = initialStateMask;

		return properties;
	}

	void FBufferDescription::ResolveResourceDimensionData()
	{
		mSubresourceCount = 1;

		mDescription.Alignment = 0;
		mDescription.Width = Size;
		mDescription.Height = 1;
		mDescription.DepthOrArraySize = 1;
		mDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		mDescription.Flags = D3D12_RESOURCE_FLAG_NONE;
		mDescription.Format = DXGI_FORMAT_UNKNOWN;
		mDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		mDescription.MipLevels = 1;
		mDescription.SampleDesc.Count = 1;
		mDescription.SampleDesc.Quality = 0;

		QueryAllocationInfo();
	}

}