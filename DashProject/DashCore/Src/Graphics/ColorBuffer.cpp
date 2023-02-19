#include "PCH.h"
#include "ColorBuffer.h"
#include "GraphicsCore.h"
#include "CpuDescriptorAllocator.h"
#include "ResourceFormat.h"
#include "RenderDevice.h"

namespace Dash
{
	void FColorBuffer::Create(const std::string& name, ID3D12Resource* resource, EResourceState initStates)
    {
        ASSERT(resource != nullptr);

        D3D12_RESOURCE_DESC desc = resource->GetDesc();
       
        ETextureDimension dimension = ETextureDimension::Texture1D;
        switch (desc.Dimension)
        {
        case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE1D:
            dimension = ETextureDimension::Texture1D;
            break;
        case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            dimension = ETextureDimension::Texture2D;
            break;
        case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE3D:
            dimension = ETextureDimension::Texture3D;
            break;
        default:
            break;
        }
        
        mDesc = FColorBufferDescription::Create(ResourceFormatFromD3DFormat(desc.Format), dimension, FResourceMagnitude(static_cast<uint32_t>(desc.Width), static_cast<uint32_t>(desc.Height), static_cast<uint32_t>(desc.DepthOrArraySize)), mDesc.ClearValue,
            desc.MipLevels, initStates, desc.SampleDesc.Count, desc.SampleDesc.Quality);

        AssociateWithResource(resource, initStates, name);
        CreateViews();
    }

    void FColorBuffer::Create(const std::string& name, const FColorBufferDescription& desc, const FLinearColor& clearColor)
    {
        mDesc = desc;

        CreateBuffer(name);
    }

    void FColorBuffer::Create(const std::string& name, uint32_t width, uint32_t height, uint32_t numMips, EResourceFormat format)
    {
        ASSERT(IsColorFormat(format));

        mDesc = FColorBufferDescription::Create2D(format, width, height, mDesc.ClearValue, numMips);

        CreateBuffer(name);
    }

    void FColorBuffer::CreateArray(const std::string& name, uint32_t width, uint32_t height, uint32_t arrayCount, uint32_t numMips, EResourceFormat format)
    {
        ASSERT(IsColorFormat(format));

        mDesc = FColorBufferDescription::Create2DArray(format, width, height, arrayCount, mDesc.ClearValue, numMips);

        CreateBuffer(name);
    }

	void FColorBuffer::CreateBuffer(const std::string& name)
	{
		CreateTextureResource(mDesc.D3DResourceDescription(), GetD3DClearValue(), name, mDesc.InitialStateMask);
		CreateViews();
	}

	D3D12_UNORDERED_ACCESS_VIEW_DESC FColorBuffer::GetUAVDesc(const D3D12_RESOURCE_DESC& resourceDesc, UINT mipSlice, UINT arraySlice, UINT planeSlice) const
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = D3DUnorderedAccessViewFormat(resourceDesc.Format);

        switch (resourceDesc.Dimension)
        {
        case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
            if (resourceDesc.DepthOrArraySize > 1)
            {
                uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
                uavDesc.Texture1DArray.ArraySize = resourceDesc.DepthOrArraySize - arraySlice;
                uavDesc.Texture1DArray.FirstArraySlice = arraySlice;
                uavDesc.Texture1DArray.MipSlice = mipSlice;
            }
            else
            {
                uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
                uavDesc.Texture1D.MipSlice = mipSlice;
            }
            break;
        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            if (resourceDesc.DepthOrArraySize > 1)
            {
                uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
                uavDesc.Texture2DArray.ArraySize = resourceDesc.DepthOrArraySize - arraySlice;
                uavDesc.Texture2DArray.FirstArraySlice = arraySlice;
                uavDesc.Texture2DArray.PlaneSlice = planeSlice;
                uavDesc.Texture2DArray.MipSlice = mipSlice;
            }
            else
            {
                uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
                uavDesc.Texture2D.PlaneSlice = planeSlice;
                uavDesc.Texture2D.MipSlice = mipSlice;
            }
            break;
        case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
            uavDesc.Texture3D.WSize = resourceDesc.DepthOrArraySize - arraySlice;
            uavDesc.Texture3D.FirstWSlice = arraySlice;
            uavDesc.Texture3D.MipSlice = mipSlice;
            break;
        default:
            ASSERT_FAIL("Invalid resource dimension.");
            break;
        }

        return uavDesc;
    }

    void FColorBuffer::CreateViews()
    {
        EFormatSupport formatSupport = CheckFormatSupport(mDesc.Format);

        if (mResource)
        {
            D3D12_RESOURCE_DESC desc = mResource->GetDesc();

            if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && EnumMaskContains(formatSupport, EFormatSupport::RenderTargetView))
            {
                mRenderTargetView = FGraphicsCore::DescriptorAllocator->AllocateRTVDescriptor();
                FGraphicsCore::Device->CreateRenderTargetView(mResource.Get(), nullptr, mRenderTargetView.GetDescriptorHandle());
            }

            if ((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && EnumMaskContains(formatSupport, EFormatSupport::ShaderResourceView))
            {
                mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
                FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), nullptr, mShaderResourceView.GetDescriptorHandle());
            }

            if ((desc.SampleDesc.Count <= 1) && (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0 && EnumMaskContains(formatSupport, EFormatSupport::UnorderAccessView))
            {
                mUnorderedAccessView = FGraphicsCore::DescriptorAllocator->AllocateUAVDescriptor(desc.MipLevels);

                for (uint32_t i = 0; i < desc.MipLevels; i++)
                {
                    D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = GetUAVDesc(desc, i);
                    FGraphicsCore::Device->CreateUnorderedAccessView(mResource.Get(), nullptr, &UAVDesc, mUnorderedAccessView.GetDescriptorHandle(i));
                }
            }
        }
    }

    D3D12_CLEAR_VALUE FColorBuffer::GetD3DClearValue() const
    {
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = D3DFormat(mDesc.Format);
        clearValue.Color[0] = mDesc.ClearValue.r;
        clearValue.Color[1] = mDesc.ClearValue.g;
        clearValue.Color[2] = mDesc.ClearValue.b;
        clearValue.Color[3] = mDesc.ClearValue.a;

        return clearValue;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE FColorBuffer::GetRenderTargetView() const
    {
        return mRenderTargetView.GetDescriptorHandle();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE FColorBuffer::GetShaderResourceView() const
    {
        return mShaderResourceView.GetDescriptorHandle();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE FColorBuffer::GetUnorderedAccessView(uint32_t mipIndex) const
    {
        ASSERT(mipIndex >= 0 && mipIndex < mDesc.MipCount);
        return mUnorderedAccessView.GetDescriptorHandle(mipIndex);
    }

    void FColorBuffer::SetClearColor(const FLinearColor& clearColor)
    {
        mDesc.ClearValue = clearColor;
    }

    void FColorBuffer::SetMsaaMode(uint32_t numSamples, uint32_t quality)
    {
        mDesc.MsaaSampleCount = numSamples;
        mDesc.MsaaQuality = quality;
    }
}