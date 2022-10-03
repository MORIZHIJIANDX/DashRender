#include "PCH.h"
#include "ColorBuffer.h"
#include "GraphicsCore.h"
#include "CpuDescriptorAllocator.h"

namespace Dash
{
    void FColorBuffer::Create(const std::wstring& name, ID3D12Resource* resource, D3D12_RESOURCE_STATES initStates)
    {
        ASSERT(resource != nullptr);

        D3D12_RESOURCE_DESC desc = resource->GetDesc();
        mNumMips = desc.MipLevels;
        mMsaaNumSmples = desc.SampleDesc.Count;
        mMsaaQuality = desc.SampleDesc.Quality;

        AssociateWithResource(resource, initStates, name);
        CreateViews();
    }

    void FColorBuffer::Create(const std::wstring& name, const D3D12_RESOURCE_DESC& desc, const FLinearColor& clearColor)
    {
        mNumMips = desc.MipLevels;
        mMsaaNumSmples = desc.SampleDesc.Count;
        mMsaaQuality = desc.SampleDesc.Quality;

        mClearColor = clearColor;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = desc.Format;
        clearValue.Color[0] = mClearColor.r;
        clearValue.Color[1] = mClearColor.g;
        clearValue.Color[2] = mClearColor.b;
        clearValue.Color[3] = mClearColor.a;
        
        CreateTextureResource(desc, clearValue, name);
        CreateViews();
    }

    void FColorBuffer::Create(const std::wstring& name, uint32_t width, uint32_t height, uint32_t numMips, DXGI_FORMAT format)
    {
        mNumMips = (numMips == 0 ? ComputeNumMips(width, height) : numMips);
        D3D12_RESOURCE_FLAGS flags = CombineResourceFlgs();
        D3D12_RESOURCE_DESC desc = DescribeTexture2D(width, height, 1, numMips, format, flags);

        desc.SampleDesc.Count = mMsaaNumSmples;
        desc.SampleDesc.Quality = mMsaaQuality;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = desc.Format;
        clearValue.Color[0] = mClearColor.r;
        clearValue.Color[1] = mClearColor.g;
        clearValue.Color[2] = mClearColor.b;
        clearValue.Color[3] = mClearColor.a;

        CreateTextureResource(desc, clearValue, name);
        CreateViews();
    }

    void FColorBuffer::CreateArray(const std::wstring& name, uint32_t width, uint32_t height, uint32_t arrayCount, DXGI_FORMAT format)
    {
        D3D12_RESOURCE_FLAGS flags = CombineResourceFlgs();
        D3D12_RESOURCE_DESC desc = DescribeTexture2D(width, height, arrayCount, 1, format, flags);

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = desc.Format;
        clearValue.Color[0] = mClearColor.r;
        clearValue.Color[1] = mClearColor.g;
        clearValue.Color[2] = mClearColor.b;
        clearValue.Color[3] = mClearColor.a;

        CreateTextureResource(desc, clearValue, name);
        CreateViews();
    }

    D3D12_UNORDERED_ACCESS_VIEW_DESC FColorBuffer::GetUAVDesc(const D3D12_RESOURCE_DESC& resourceDesc, UINT mipSlice, UINT arraySlice, UINT planeSlice) const
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = GetUAVFormat(resourceDesc.Format);

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
        CheckFeatureSupport();

        if (mResource)
        {
            D3D12_RESOURCE_DESC desc = mResource->GetDesc();

            if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && CheckRTVSupport())
            {
                mRenderTargetView = FGraphicsCore::DescriptorAllocator->AllocateRTVDescriptor();
                FGraphicsCore::Device->CreateRenderTargetView(mResource.Get(), nullptr, mRenderTargetView.GetDescriptorHandle());
            }

            if ((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && CheckSRVSupport())
            {
                mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
                FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), nullptr, mShaderResourceView.GetDescriptorHandle());
            }

            if ((desc.SampleDesc.Count <= 1) && (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0 && CheckUAVSupport())
            {
                mUnorderedAccessView = FGraphicsCore::DescriptorAllocator->AllocateUAVDescriptor();

                for (uint32_t i = 0; i < desc.MipLevels; i++)
                {
                    D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = GetUAVDesc(desc, i);
                    FGraphicsCore::Device->CreateUnorderedAccessView(mResource.Get(), nullptr, &UAVDesc, mUnorderedAccessView.GetDescriptorHandle(i));
                }
            }
        }
    }

    D3D12_RESOURCE_FLAGS FColorBuffer::CombineResourceFlgs() const
    {
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

        if (flags == D3D12_RESOURCE_FLAG_NONE && mMsaaNumSmples == 1)
        {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        return D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | flags;
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
        ASSERT(mipIndex >= 0 && mipIndex < mNumMips);
        return mUnorderedAccessView.GetDescriptorHandle(mipIndex);
    }

    void FColorBuffer::SetClearColor(const FLinearColor& clearColor)
    {
        mClearColor = clearColor;
    }

    void FColorBuffer::SetMsaaMode(uint32_t numSamples, uint32_t quality)
    {
        mMsaaNumSmples = numSamples;
        mMsaaQuality = quality;
    }
}