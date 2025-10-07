#include "PCH.h"
#include "DepthBuffer.h"
#include "GraphicsCore.h"
#include "CpuDescriptorAllocator.h"
#include "RenderDevice.h"

namespace Dash
{
	void FDepthBuffer::Create(const std::string& name, const FDepthBufferDescription& desc)
	{
        mDesc = desc;

        CreateBuffer(name);
	}

    void FDepthBuffer::Create(const std::string& name, uint32 width, uint32 height, EResourceFormat format)
    {
        Create(name, width, height, 1, 0, format);
    }

    void FDepthBuffer::Create(const std::string& name, uint32 width, uint32 height, uint32 sampleCount, uint32 sampleQuality, EResourceFormat format)
    {
        ASSERT(IsDepthStencilFormat(format));

        mDesc = FDepthBufferDescription::Create(format, width, height, mDesc.ClearValue, 1, EResourceState::Common, sampleCount, sampleQuality);

        CreateBuffer(name);
    }

	void FDepthBuffer::CreateBuffer(const std::string& name)
	{
		CreateTextureResource(mDesc.D3DResourceDescription(), GetD3DClearValue(), name);
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
        if (mResource)
        {
            EFormatSupport formatSupport = CheckFormatSupport(mDesc.Format);

            D3D12_RESOURCE_DESC desc = mResource->GetDesc();

            if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 && EnumMaskContains(formatSupport, EFormatSupport::DepthStencilView))
            {
                mDepthStencilView = FGraphicsCore::DescriptorAllocator->AllocateDSVDescriptor();
                FGraphicsCore::Device->CreateDepthStencilView(mResource.GetReference(), nullptr, mDepthStencilView.GetDescriptorHandle());
            }

            if ((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && EnumMaskContains(formatSupport, EFormatSupport::ShaderResourceView))
            {
                mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
                FGraphicsCore::Device->CreateShaderResourceView(mResource.GetReference(), nullptr, mShaderResourceView.GetDescriptorHandle());
            }
        }
    }

    D3D12_CLEAR_VALUE FDepthBuffer::GetD3DClearValue() const
    {
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = D3DFormat(mDesc.Format);
        clearValue.DepthStencil.Depth = mDesc.ClearValue.Depth;
        clearValue.DepthStencil.Stencil = mDesc.ClearValue.Stencil;
        return clearValue;
    }
}