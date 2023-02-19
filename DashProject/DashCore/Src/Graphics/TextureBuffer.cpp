#include "PCH.h"
#include "TextureBuffer.h"
#include "GraphicsCore.h"
#include "CpuDescriptorAllocator.h"
#include "ResourceFormat.h"
#include "RenderDevice.h"

namespace Dash
{

	D3D12_CPU_DESCRIPTOR_HANDLE FTextureBuffer::GetShaderResourceView() const
	{
		return mShaderResourceView.GetDescriptorHandle();
	}

	void FTextureBuffer::Create(const std::string& name, const FTextureBufferDescription& desc)
	{
		mDesc = desc;

		CreateBuffer(name);
	}

	void FTextureBuffer::Create(const std::string& name, uint32_t width, uint32_t height, uint32_t numMips, EResourceFormat format)
	{
		ASSERT(IsColorFormat(format));

		mDesc = FTextureBufferDescription::Create2D(format, width, height, numMips);

		CreateBuffer(name);
	}

	void FTextureBuffer::Create(const std::string& name, uint32_t width, uint32_t height, uint32_t arrayCount, uint32_t numMips, EResourceFormat format)
	{
		ASSERT(IsColorFormat(format));

		mDesc = FTextureBufferDescription::Create2DArray(format, width, height, arrayCount, numMips);

		CreateBuffer(name);
	}

	void FTextureBuffer::CreateBuffer(const std::string& name)
	{
		CreateTextureResource(mDesc.D3DResourceDescription(), CD3DX12_CLEAR_VALUE{}, name, mDesc.InitialStateMask);
		CreateViews();
	}

	void FTextureBuffer::CreateViews()
	{
		EFormatSupport formatSupport = CheckFormatSupport(mDesc.Format);

		if (mResource)
		{
			D3D12_RESOURCE_DESC desc = mResource->GetDesc();

			if ((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && EnumMaskContains(formatSupport, EFormatSupport::ShaderResourceView))
			{
				mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
				FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), nullptr, mShaderResourceView.GetDescriptorHandle());
			}
		}
	}

}