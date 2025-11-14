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

	void FTextureBuffer::InitResource(const std::string& name, const FTextureBufferDescription& desc)
	{
		mDesc = desc;

		CreateBuffer(name);
	}

	void FTextureBuffer::InitResource(const std::string& name, uint32 width, uint32 height, uint32 numMips, EResourceFormat format)
	{
		ASSERT(IsColorFormat(format));

		mDesc = FTextureBufferDescription::Create2D(format, width, height, numMips);

		CreateBuffer(name);
	}

	void FTextureBuffer::InitResource(const std::string& name, uint32 width, uint32 height, uint32 arrayCount, uint32 numMips, EResourceFormat format)
	{
		ASSERT(IsColorFormat(format));

		mDesc = FTextureBufferDescription::Create2DArray(format, width, height, arrayCount, numMips);

		CreateBuffer(name);
	}

	void FTextureBuffer::CreateBuffer(const std::string& name)
	{
		CreateTextureResource(mDesc.D3DResourceDescription(), CD3DX12_CLEAR_VALUE{}, name);
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
				FGraphicsCore::Device->CreateShaderResourceView(mResource.GetReference(), nullptr, mShaderResourceView.GetDescriptorHandle());
			}
		}
	}

}