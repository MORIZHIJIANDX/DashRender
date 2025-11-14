#include "PCH.h"
#include "ResourceDescription.h"
#include "GraphicsCore.h"
#include "RenderDevice.h"
#include "DirectXTex/DirectXTex.h"
#include "DX12Helper.h"

namespace Dash
{
	void FResourceDescription::GetPitch(size_t& rowPitch, size_t slicePitch, size_t mipIndex) const
	{
		DX_CALL(DirectX::ComputePitch(mDescription.Format, mDescription.Width >> mipIndex, mDescription.Height >> mipIndex, rowPitch, slicePitch));
	}

	void FResourceDescription::QueryAllocationInfo()
	{
		if (mDescription.Width == 0)
		{
			return;
		}

		D3D12_RESOURCE_ALLOCATION_INFO allocInfo = FGraphicsCore::Device->GetResourceAllocationInfo(0, 1, &mDescription);
		
		mResourceAlignment = static_cast<uint32>(allocInfo.Alignment);
		mResourceSizeInBytes = static_cast<uint32>(allocInfo.SizeInBytes);
		mDescription.Alignment = mResourceAlignment;
	}

	FResourceMagnitude FTextureDescription::ComputeMipSize(uint8 mip) const
	{
		FResourceMagnitude mipMagnitude = Magnitude.XYZMultiplied(1.0f / FMath::Pow(Scalar(2), Scalar(mip)));
		mipMagnitude.Depth = FMath::Max(mipMagnitude.Depth, uint32(1));
		return mipMagnitude;
	}

	void FTextureDescription::ResolveResourceDimensionData(bool allowUAV, bool allowRTV, bool allowDSV)
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

		bool isArray = (Dimension == ETextureDimension::Texture1D || Dimension == ETextureDimension::Texture2D) && Magnitude.Depth > 1;
		mSubresourceCount = isArray ? Magnitude.Depth * MipCount : MipCount;

		mDescription.Format = D3DFormat(Format);
		mDescription.Width = Magnitude.Width;
		mDescription.Height = Magnitude.Height;
		mDescription.DepthOrArraySize = Magnitude.Depth;
		mDescription.SampleDesc.Count = MsaaSampleCount;
		mDescription.SampleDesc.Quality = MsaaQuality;
		mDescription.MipLevels = (MipCount == 0 ? ComputeNumMips() : MipCount);
		mDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		mDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		QueryAllocationInfo();

		EFormatSupport formatSupport = CheckFormatSupport(mDescription.Format);

		if (allowUAV && EnumMaskContains(formatSupport, EFormatSupport::UnorderAccessView) && MsaaSampleCount == 1)
		{
			mDescription.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		if (allowRTV && EnumMaskContains(formatSupport, EFormatSupport::RenderTargetView) && !IsCompressedFormat(mDescription.Format))
		{
			mDescription.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if (allowDSV && EnumMaskContains(formatSupport, EFormatSupport::DepthStencilView) && !IsCompressedFormat(mDescription.Format))
		{
			mDescription.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}
	}

	uint32 FTextureDescription::ComputeNumMips() const
	{
		uint32 highBit;
		_BitScanReverse((unsigned long*)&highBit, Magnitude.Width | Magnitude.Height);
		return highBit + 1;
	}

	FColorBufferDescription FColorBufferDescription::Create(EResourceFormat format, ETextureDimension dimension, const FResourceMagnitude& magnitude, const FLinearColor& optimizedClearValue, uint32 mipCount /*= 1*/,
			EResourceState initialStateMask /*= EResourceState::Common*/, uint32 sampleCount /*= 1*/, uint32 msaaQuality /*= 0*/)
	{
		FColorBufferDescription desc;
		desc.Dimension = dimension;
		desc.Format = format;
		//desc.InitialStateMask = initialStateMask;
		desc.Magnitude = magnitude;
		desc.MipCount = mipCount;
		desc.MsaaSampleCount = sampleCount;
		desc.MsaaQuality = msaaQuality;
		desc.ClearValue = optimizedClearValue;

		desc.ResolveResourceDimensionData(true, true, false);

		return desc;
	}

	FColorBufferDescription FColorBufferDescription::Create1D(EResourceFormat format, uint32 width, const FLinearColor& optimizedClearValue, uint32 mipCount /*= 1*/, EResourceState initialStateMask /*= EResourceState::Common*/)
	{
		return FColorBufferDescription::Create(format, ETextureDimension::Texture1D, FResourceMagnitude(width), optimizedClearValue, mipCount, initialStateMask);
	}

	FColorBufferDescription FColorBufferDescription::Create2D(EResourceFormat format, uint32 width, uint32 height, const FLinearColor& optimizedClearValue, uint32 mipCount /*= 1*/, EResourceState initialStateMask /*= EResourceState::Common*/)
	{
		return FColorBufferDescription::Create(format, ETextureDimension::Texture2D, FResourceMagnitude(width, height), optimizedClearValue, mipCount, initialStateMask);
	}

	FColorBufferDescription FColorBufferDescription::Create2DArray(EResourceFormat format, uint32 width, uint32 height, uint32 arraySize, const FLinearColor& optimizedClearValue, uint32 mipCount, EResourceState initialStateMask)
	{
		return FColorBufferDescription::Create(format, ETextureDimension::Texture2D, FResourceMagnitude(width, height, arraySize), optimizedClearValue, mipCount, initialStateMask);
	}

	FColorBufferDescription FColorBufferDescription::Create3D(EResourceFormat format, uint32 width, uint32 height, uint32 depth, const FLinearColor& optimizedClearValue, uint32 mipCount /*= 1*/, EResourceState initialStateMask /*= EResourceState::Common*/)
	{
		return FColorBufferDescription::Create(format, ETextureDimension::Texture3D, FResourceMagnitude(width, height, depth), optimizedClearValue, mipCount, initialStateMask);
	}

	FDepthBufferDescription FDepthBufferDescription::Create(EResourceFormat format, uint32 width, uint32 height, const FDepthStencilClearValue& optimizedClearValue, uint32 mipCount /*= 1*/, 
		EResourceState initialStateMask /*= EResourceState::Common*/, uint32 sampleCount /*= 1*/, uint32 msaaQuality /*= 0*/)
	{
		FDepthBufferDescription desc;
		desc.Dimension = ETextureDimension::Texture2D;
		desc.Format = format;
		//desc.InitialStateMask = initialStateMask;
		desc.Magnitude = FResourceMagnitude(width, height);
		desc.MipCount = mipCount;
		desc.MsaaSampleCount = sampleCount;
		desc.MsaaQuality = msaaQuality;
		desc.ClearValue = optimizedClearValue;

		desc.ResolveResourceDimensionData(false, true, true);

		return desc;
	}

	FBufferDescription FBufferDescription::Create(uint32 elementSize, uint32 elementCount, bool unorderedAccess, uint32 elementAlignment /*= 1*/, EResourceState initialStateMask /*= EResourceState::Common*/)
	{
		ASSERT(elementSize > 0);

		FBufferDescription desc;
		desc.Count = elementCount;
		desc.Stride = FMath::AlignUp(elementSize, elementAlignment);
		desc.Size = desc.Stride * elementCount;
		//desc.InitialStateMask = initialStateMask;

		desc.ResolveResourceDimensionData(unorderedAccess, false);

		return desc;
	}

	FBufferDescription FBufferDescription::Create(uint32 size, uint32 stride, EBufferUsage usage, EResourceBindFlag bindFlags, EBufferMiscFlag miscFlags, EResourceFormat format)
	{
		FBufferDescription desc;
		desc.Size = size;
		desc.Stride = stride;
		desc.Usage = usage;
		desc.BindFlags = bindFlags;
		desc.MiscFlags = miscFlags;
		desc.Format = format;

		//desc.ResolveResourceDimensionData(unorderedAccess, false);

		return desc;
	}

	FBufferDescription FBufferDescription::CreateVertex(uint32 elementSize, uint32 elementCount)
	{
		return FBufferDescription::Create(elementSize * elementCount, elementSize, EBufferUsage::Default, EResourceBindFlag::None, EBufferMiscFlag::VertexBuffer, EResourceFormat::Unknown);
	}

	FBufferDescription FBufferDescription::CreateDynamicVertex(uint32 elementSize, uint32 elementCount)
	{
		return FBufferDescription::Create(elementSize * elementCount, elementSize, EBufferUsage::Upload, EResourceBindFlag::None, EBufferMiscFlag::VertexBuffer, EResourceFormat::Unknown);
	}

	FBufferDescription FBufferDescription::CreateIndex(uint32 size, uint32 stride)
	{
		return FBufferDescription::Create(size, stride, EBufferUsage::Default, EResourceBindFlag::None, EBufferMiscFlag::IndexBuffer, EResourceFormat::Unknown);
	}

	FBufferDescription FBufferDescription::CreateDynamicIndexBuffer(uint32 size, uint32 stride)
	{
		return FBufferDescription::Create(size, stride, EBufferUsage::Upload, EResourceBindFlag::None, EBufferMiscFlag::IndexBuffer, EResourceFormat::Unknown);
	}

	FBufferDescription FBufferDescription::CreateStructured(uint32 elementSize, uint32 elementCount, bool uav, bool dynamic)
	{
		ASSERT_MSG(uav ^ dynamic, "Buffer cannot be dynamic and be accessed as UAV at the same time!");
		EResourceBindFlag bindFlag = EResourceBindFlag::ShaderResource;
		if (uav)
		{
			bindFlag |= EResourceBindFlag::UnorderedAccess;
		}
		return FBufferDescription::Create(elementSize * elementCount, elementSize, dynamic ? EBufferUsage::Upload : EBufferUsage::Default, 
			bindFlag, EBufferMiscFlag::StructuredBuffer, EResourceFormat::Unknown);
	}

	FBufferDescription FBufferDescription::CreateCounter()
	{
		return FBufferDescription::Create(sizeof(uint32), 0, EBufferUsage::Default, EResourceBindFlag::UnorderedAccess, EBufferMiscFlag::None, EResourceFormat::Unknown);
	}

	void FBufferDescription::ResolveResourceDimensionData(bool allowUAV, bool allowRTV, bool allowDSV)
	{
		mSubresourceCount = 1;

		mDescription.Alignment = 0;
		mDescription.Width = Size;
		mDescription.Height = 1;
		mDescription.DepthOrArraySize = 1;
		mDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		mDescription.Flags = allowUAV ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;
		mDescription.Format = DXGI_FORMAT_UNKNOWN;
		mDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		mDescription.MipLevels = 1;
		mDescription.SampleDesc.Count = 1;
		mDescription.SampleDesc.Quality = 0;

		QueryAllocationInfo();
	}

	FTextureBufferDescription FTextureBufferDescription::Create(EResourceFormat format, ETextureDimension dimension, const FResourceMagnitude& magnitude, uint32 mipCount)
	{
		FTextureBufferDescription desc;
		desc.Dimension = dimension;
		desc.Format = format;
		desc.Magnitude = magnitude;
		desc.MipCount = desc.AutoGenerateMips ? desc.ComputeNumMips() : mipCount;
		desc.MsaaSampleCount = 1;
		desc.MsaaQuality = 0;

		desc.ResolveResourceDimensionData(true, false, false);

		return desc;
	}

	FTextureBufferDescription FTextureBufferDescription::Create1D(EResourceFormat format, uint32 width, uint32 mipCount)
	{
		return Create(format, ETextureDimension::Texture1D, FResourceMagnitude(width), mipCount);
	}

	FTextureBufferDescription FTextureBufferDescription::Create2D(EResourceFormat format, uint32 width, uint32 height, uint32 mipCount)
	{
		return Create(format, ETextureDimension::Texture2D, FResourceMagnitude(width, height), mipCount);
	}

	FTextureBufferDescription FTextureBufferDescription::Create2DArray(EResourceFormat format, uint32 width, uint32 height, uint32 arraySize, uint32 mipCount)
	{
		return Create(format, ETextureDimension::Texture2D, FResourceMagnitude(width, height, arraySize), mipCount);
	}

	FTextureBufferDescription FTextureBufferDescription::Create3D(EResourceFormat format, uint32 width, uint32 height, uint32 depth, uint32 mipCount)
	{
		return Create(format, ETextureDimension::Texture3D, FResourceMagnitude(width, height, depth), mipCount);
	}

}