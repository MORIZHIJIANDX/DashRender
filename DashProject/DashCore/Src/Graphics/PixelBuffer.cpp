#include "PCH.h"
#include "PixelBuffer.h"
#include "d3dx12.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"

namespace Dash
{	
	D3D12_RESOURCE_DESC FPixelBuffer::DescribeTexture2D(uint32_t width, uint32_t height, uint32_t depthOrArraySize, uint32_t numMips, FFormatVariant format, UINT flag)
	{
		D3D12_RESOURCE_DESC desc{};
		desc.Alignment = 0;
		desc.DepthOrArraySize = static_cast<UINT16>(depthOrArraySize);
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Flags = static_cast<D3D12_RESOURCE_FLAGS>(flag);
		desc.Format = D3DTypelessFormat(D3DFormat(format));
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.MipLevels = static_cast<UINT16>(numMips);
		desc.Width = static_cast<UINT64>(width);
		desc.Height = static_cast<UINT>(height);
		desc.SampleDesc.Quality = 0;
		desc.SampleDesc.Count = 1;

		mWidth = width;
		mHeight = height;
		mArraySize = depthOrArraySize;
		mFormat = format;

		return desc;
	}

	void FPixelBuffer::AssociateWithResource(ID3D12Resource* resource, const D3D12_RESOURCE_STATES& currentState, const std::string& name)
	{
		ASSERT(resource != nullptr);
		
		Destroy();

		mResource.Attach(resource);

		D3D12_RESOURCE_DESC desc = resource->GetDesc();
		
		mWidth = static_cast<uint32_t>(desc.Width);
		mHeight = desc.Height;
		mArraySize = desc.DepthOrArraySize;
		mFormat = FormatFromD3DFormat(desc.Format);

		SetName(name);

		FGpuResourcesStateTracker::AddGlobalResourceState(*this, currentState);	
	}

	void FPixelBuffer::CreateTextureResource(const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue, const std::string& name)
	{
		Destroy();

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
		DX_CALL(FGraphicsCore::Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(&mResource)));

		SetName(name);

		FGpuResourcesStateTracker::AddGlobalResourceState(*this, D3D12_RESOURCE_STATE_COMMON);

		mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

		mWidth = static_cast<uint32_t>(resourceDesc.Width);
		mHeight = resourceDesc.Height;
		mArraySize = resourceDesc.DepthOrArraySize;
		mFormat = FormatFromD3DFormat(resourceDesc.Format);
	}

	void FPixelBuffer::CheckFeatureSupport()
	{
		if (mResource)
		{
			D3D12_RESOURCE_DESC desc = mResource->GetDesc();
			mFormatSupport.Format = desc.Format;
			DX_CALL(FGraphicsCore::Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &mFormatSupport, sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)));
		}
	}

	bool FPixelBuffer::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const
	{
		return (mFormatSupport.Support1 & formatSupport) != 0;
	}

	bool FPixelBuffer::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const
	{
		return (mFormatSupport.Support2 & formatSupport) != 0;
	}
}