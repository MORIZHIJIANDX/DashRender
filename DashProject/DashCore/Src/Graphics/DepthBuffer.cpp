#include "PCH.h"
#include "DepthBuffer.h"
#include "GraphicsCore.h"
#include "CpuDescriptorAllocator.h"

namespace Dash
{
	void FDepthBuffer::Create(const std::string& name, const D3D12_RESOURCE_DESC& desc, float clearDepth, uint8_t clearStencil)
	{
        mClearDepth = clearDepth;
        mClearStencil = clearStencil;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = desc.Format;
        clearValue.DepthStencil.Depth = mClearDepth;
        clearValue.DepthStencil.Stencil = mClearStencil;

        CreateTextureResource(desc, clearValue, name);
        CreateViews();
	}

    void FDepthBuffer::Create(const std::string& name, uint32_t width, uint32_t height, DXGI_FORMAT format)
    {
        Create(name, width, height, 1, format);
    }

    void FDepthBuffer::Create(const std::string& name, uint32_t width, uint32_t height, uint32_t sampleCount, DXGI_FORMAT format)
    {
        D3D12_RESOURCE_DESC desc = DescribeTexture2D(width, height, 1, 1, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        desc.SampleDesc.Count = sampleCount;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = desc.Format;
        clearValue.DepthStencil.Depth = mClearDepth;
        clearValue.DepthStencil.Stencil = mClearStencil;

        CreateTextureResource(desc, clearValue, name);
        CreateViews();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE FDepthBuffer::GetDepthStencilView() const
    {
        return mDepthStencilView.GetDescriptorHandle();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE FDepthBuffer::GetShaderResourceView() const
    {
        return mShaderResourceView.GetDescriptorHandle();
    }

    void FDepthBuffer::CreateViews()
    {
        CheckFeatureSupport();

        if (mResource)
        {
            D3D12_RESOURCE_DESC desc = mResource->GetDesc();

            if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 && CheckDSVSupport())
            {
                mDepthStencilView = FGraphicsCore::DescriptorAllocator->AllocateDSVDescriptor();
                FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), nullptr, mShaderResourceView.GetDescriptorHandle());
            }

            if ((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && CheckSRVSupport())
            {
                mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
                FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), nullptr, mShaderResourceView.GetDescriptorHandle());
            }
        }
    }
}