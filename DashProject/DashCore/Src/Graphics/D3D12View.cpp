#include "PCH.h"
#include "D3D12View.h"
#include "GpuResource.h"
#include "GraphicsCore.h"
#include "CpuDescriptorAllocator.h"
#include "RenderDevice.h"
#include "GpuBuffer.h"

namespace Dash
{
	void ComputeCalcSubresourceCount(FD3D12ViewRange& viewRange, FD3D12Resource* resource)
	{
		uint16 mipLevels = resource->GetMipLevels();
		uint16 arraySize = resource->GetArraySize();

		viewRange.FirstSubresource = D3D12CalcSubresource(viewRange.Mip.First, viewRange.Array.First, viewRange.Plane.First, mipLevels, arraySize);
		viewRange.NumSubresources = D3D12CalcSubresource(viewRange.Mip.InclusiveLast(), viewRange.Array.InclusiveLast(), viewRange.Plane.InclusiveLast(), mipLevels, arraySize) - viewRange.FirstSubresource;
		viewRange.NumSubresources = FMath::Max(uint32(1), viewRange.NumSubresources);
	}

	FD3D12View::FD3D12View(D3D12_DESCRIPTOR_HEAP_TYPE type)
		: mBaseResource(nullptr)
		, mType(type)
	{
		mCpuDescriptor = FGraphicsCore::DescriptorAllocator->Allocate(type);
	}

	void FShaderResourceView::CreateView(FGpuResource* resource, const CD3DX12_SHADER_RESOURCE_VIEW_DESC& viewDesc)
	{
		mBaseResource = resource;
		mViewDesc = viewDesc;

		ParseViewRange(viewDesc);

		FGraphicsCore::Device->CreateShaderResourceView(mBaseResource->GetResource(), &mViewDesc, mCpuDescriptor.GetDescriptorHandle());
	}

	void FShaderResourceView::ParseViewRange(const CD3DX12_SHADER_RESOURCE_VIEW_DESC& viewDesc)
	{
		switch (viewDesc.ViewDimension)
		{
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER:
		{
			mViewRange.Mip = {0, 1};
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1D:
		{
			mViewRange.Mip = { viewDesc.Texture1D.MostDetailedMip, viewDesc.Texture1D.MipLevels };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
		{
			mViewRange.Mip = { viewDesc.Texture1DArray.MostDetailedMip, viewDesc.Texture1DArray.MipLevels };
			mViewRange.Array = { viewDesc.Texture1DArray.FirstArraySlice, viewDesc.Texture1DArray.ArraySize };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D:
		{
			mViewRange.Mip = { viewDesc.Texture2D.MostDetailedMip, viewDesc.Texture2D.MipLevels };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { viewDesc.Texture2D.PlaneSlice, 1 };
			break;
		}
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
		{
			mViewRange.Mip = { viewDesc.Texture2DArray.MostDetailedMip, viewDesc.Texture2DArray.MipLevels };
			mViewRange.Array = { viewDesc.Texture2DArray.FirstArraySlice, viewDesc.Texture2DArray.ArraySize };
			mViewRange.Plane = { viewDesc.Texture2DArray.PlaneSlice, 1 };
			break;
		}
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DMS:
		{
			mViewRange.Mip = { 0, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
		{
			mViewRange.Mip = { 0, 1 };
			mViewRange.Array = { viewDesc.Texture2DMSArray.FirstArraySlice, viewDesc.Texture2DMSArray.ArraySize };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE3D:
		{
			mViewRange.Mip = { viewDesc.Texture3D.MostDetailedMip, viewDesc.Texture3D.MipLevels };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURECUBE:
		{
			mViewRange.Mip = { viewDesc.TextureCube.MostDetailedMip, viewDesc.TextureCube.MipLevels };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
		{
			mViewRange.Mip = { viewDesc.TextureCubeArray.MostDetailedMip, viewDesc.TextureCubeArray.MipLevels };
			mViewRange.Array = { viewDesc.TextureCubeArray.First2DArrayFace, viewDesc.TextureCubeArray.NumCubes };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE:
		{
			mViewRange.Mip = { 0, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		default:
			ASSERT(false);
			break;
		}

		ComputeCalcSubresourceCount(mViewRange, mBaseResource->GetResource());
	}

	void FUnorderedAccessView::CreateView(FGpuResource* resource, const CD3DX12_UNORDERED_ACCESS_VIEW_DESC& viewDesc, bool needCounter)
	{
		mBaseResource = resource;
		mViewDesc = viewDesc;

		ParseViewRange(viewDesc);

		if (needCounter)
		{
			mCounterBuffer = FGraphicsCore::Device->CreateByteAddressBuffer(resource->GetName() + " CounterBuffer", 1, 4, EResourceState::UnorderedAccess);
		}

		FGraphicsCore::Device->CreateUnorderedAccessView(mBaseResource->GetResource(), needCounter ? mCounterBuffer->GetResource() : nullptr, &mViewDesc, mCpuDescriptor.GetDescriptorHandle());
	}

	FByteAddressBuffer* FUnorderedAccessView::GetCounterBuffer() const
	{
		return mCounterBuffer.GetReference();
	}

	void FUnorderedAccessView::ParseViewRange(const CD3DX12_UNORDERED_ACCESS_VIEW_DESC& viewDesc)
	{
		switch (viewDesc.ViewDimension)
		{
		case D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_BUFFER:
		{
			mViewRange.Mip = { 0, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE1D:
		{
			mViewRange.Mip = { viewDesc.Texture1D.MipSlice, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE1DARRAY:
		{
			mViewRange.Mip = { viewDesc.Texture1DArray.MipSlice, 1 };
			mViewRange.Array = { viewDesc.Texture1DArray.FirstArraySlice, viewDesc.Texture1DArray.ArraySize };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2D:
		{
			mViewRange.Mip = { viewDesc.Texture2D.MipSlice, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { viewDesc.Texture2D.PlaneSlice, 1 };
			break;
		}
		case D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
		{
			mViewRange.Mip = { viewDesc.Texture2DArray.MipSlice, 1 };
			mViewRange.Array = { viewDesc.Texture2DArray.FirstArraySlice, viewDesc.Texture2DArray.ArraySize };
			mViewRange.Plane = { viewDesc.Texture2DArray.PlaneSlice, 1 };
			break;
		}
		case D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2DMS:
		{
			mViewRange.Mip = { 0, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 1 };
			break;
		}
		case D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2DMSARRAY:
		{
			mViewRange.Mip = { 0, 1 };
			mViewRange.Array = { viewDesc.Texture2DMSArray.FirstArraySlice, viewDesc.Texture2DMSArray.ArraySize };
			mViewRange.Plane = { 0, 1 };
			break;
		}
		case D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE3D:
		{
			mViewRange.Mip = { viewDesc.Texture3D.MipSlice, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 1 };
			break;
		}
		default:
			ASSERT(false);
			break;
		}

		ComputeCalcSubresourceCount(mViewRange, mBaseResource->GetResource());
	}

	void FRenderTargetView::CreateView(FGpuResource* resource, const D3D12_RENDER_TARGET_VIEW_DESC& viewDesc)
	{
		mBaseResource = resource;
		mViewDesc = viewDesc;

		ParseViewRange(viewDesc);

		FGraphicsCore::Device->CreateRenderTargetView(mBaseResource->GetResource(), &mViewDesc, mCpuDescriptor.GetDescriptorHandle());
	}

	void FRenderTargetView::ParseViewRange(const D3D12_RENDER_TARGET_VIEW_DESC& viewDesc)
	{
		switch (viewDesc.ViewDimension)
		{
		case D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_BUFFER:
		{
			mViewRange.Mip = { 0, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE1D:
		{
			mViewRange.Mip = { viewDesc.Texture1D.MipSlice, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
		{
			mViewRange.Mip = { viewDesc.Texture1DArray.MipSlice, 1 };
			mViewRange.Array = { viewDesc.Texture1DArray.FirstArraySlice, viewDesc.Texture1DArray.ArraySize };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D:
		{
			mViewRange.Mip = { viewDesc.Texture2D.MipSlice, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { viewDesc.Texture2D.PlaneSlice, 1 };
			break;
		}
		case D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
		{
			mViewRange.Mip = { viewDesc.Texture2DArray.MipSlice, 1 };
			mViewRange.Array = { viewDesc.Texture2DArray.FirstArraySlice, viewDesc.Texture2DArray.ArraySize };
			mViewRange.Plane = { viewDesc.Texture2DArray.PlaneSlice, 1 };
			break;
		}
		case D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2DMS:
		{
			mViewRange.Mip = { 0, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 1 };
			break;
		}
		case D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
		{
			mViewRange.Mip = { 0, 1 };
			mViewRange.Array = { viewDesc.Texture2DMSArray.FirstArraySlice, viewDesc.Texture2DMSArray.ArraySize };
			mViewRange.Plane = { 0, 1 };
			break;
		}
		case D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE3D:
		{
			mViewRange.Mip = { viewDesc.Texture3D.MipSlice, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 1 };
			break;
		}
		default:
			ASSERT(false);
			break;
		}

		ComputeCalcSubresourceCount(mViewRange, mBaseResource->GetResource());
	}

	void FDepthStencilView::CreateView(FGpuResource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC& viewDesc)
	{
		mBaseResource = resource;
		mViewDesc = viewDesc;

		ParseViewRange(viewDesc);

		FGraphicsCore::Device->CreateDepthStencilView(mBaseResource->GetResource(), &mViewDesc, mCpuDescriptor.GetDescriptorHandle());
	}

	void FDepthStencilView::ParseViewRange(const D3D12_DEPTH_STENCIL_VIEW_DESC& viewDesc)
	{
		switch (viewDesc.ViewDimension)
		{
		case D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE1D:
		{
			mViewRange.Mip = { viewDesc.Texture1D.MipSlice, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE1DARRAY:
		{
			mViewRange.Mip = { viewDesc.Texture1DArray.MipSlice, 1 };
			mViewRange.Array = { viewDesc.Texture1DArray.FirstArraySlice, viewDesc.Texture1DArray.ArraySize };
			mViewRange.Plane = { 0, 0 };
			break;
		}
		case D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D:
		{
			mViewRange.Mip = { viewDesc.Texture2D.MipSlice, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 1 };
			break;
		}
		case D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2DARRAY:
		{
			mViewRange.Mip = { viewDesc.Texture2DArray.MipSlice, 1 };
			mViewRange.Array = { viewDesc.Texture2DArray.FirstArraySlice, viewDesc.Texture2DArray.ArraySize };
			mViewRange.Plane = { 0, 1 };
			break;
		}
		case D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2DMS:
		{
			mViewRange.Mip = { 0, 1 };
			mViewRange.Array = { 0, 1 };
			mViewRange.Plane = { 0, 1 };
			break;
		}
		case D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY:
		{
			mViewRange.Mip = { 0, 1 };
			mViewRange.Array = { viewDesc.Texture2DMSArray.FirstArraySlice, viewDesc.Texture2DMSArray.ArraySize };
			mViewRange.Plane = { 0, 1 };
			break;
		}
		default:
			ASSERT(false);
			break;
		}

		ComputeCalcSubresourceCount(mViewRange, mBaseResource->GetResource());
	}
}