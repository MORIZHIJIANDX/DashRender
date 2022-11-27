#include "PCH.h"
#include "GpuBuffer.h"
#include "GraphicsCore.h"
#include "CommandContext.h"
#include "CpuDescriptorAllocator.h"

namespace Dash
{
	void FGpuBuffer::Create(const std::string& name, uint32_t numElements, uint32_t elementSize, const void* initData /*= nullptr*/, D3D12_RESOURCE_FLAGS flags /*D3D12_RESOURCE_FLAG_NONE*/)
	{
		Destroy();

		mElementCount = numElements;
		mElementSize = elementSize;
		mBufferSize = static_cast<size_t>(numElements * elementSize);
		mResourceFlag |= flags;

		D3D12_RESOURCE_DESC resourceDesc = DescribeBuffer();

		CreateBufferResource(resourceDesc);

		if (initData)
		{
			FCommandContext::InitializeBuffer(*this, initData, mBufferSize);
		}

		CreateViews();

		SetD3D12DebugName(mResource.Get(), name.c_str());
	}

	D3D12_RESOURCE_DESC FGpuBuffer::DescribeBuffer()
	{
		ASSERT(mBufferSize != 0);

		D3D12_RESOURCE_DESC desc{};
		desc.Alignment = 0;
		desc.DepthOrArraySize = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Flags = mResourceFlag;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Height = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Width = static_cast<UINT64>(mBufferSize);

		return desc;
	}

	void FGpuBuffer::CreateBufferResource(const D3D12_RESOURCE_DESC& desc)
	{
		// D3D12_HEAP_TYPE_DEFAULT can not be accessed by CPU
		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
		// D3D12_RESOURCE_STATE_COMMON 不可直接作为 SRV 和 UAV，需要转换为 D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER (D3D12_RESOURCE_STATE_GENERIC_READ) 状态
		DX_CALL(FGraphicsCore::Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&mResource)));

		FGpuResourcesStateTracker::AddGlobalResourceState(*this, D3D12_RESOURCE_STATE_COMMON);

		//GetGPUVirtualAddress is only useful for buffer resources, it will return zero for all texture resources.
		mGpuVirtualAddress = mResource->GetGPUVirtualAddress();
	}

	D3D12_GPU_VIRTUAL_ADDRESS FGpuConstantBuffer::GetGpuVirtualAddress(size_t offset /*= 0*/) const
	{
		return mGpuVirtualAddress + mElementSize * offset;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FGpuConstantBuffer::GetShaderResourceView() const
	{
		return mShaderResourceView.GetDescriptorHandle();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FGpuConstantBuffer::GetUnorderedAccessView() const
	{
		return mUnorderedAccessView.GetDescriptorHandle();
	}

	void FByteAddressBuffer::CreateViews()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = static_cast<UINT>(mBufferSize / 4);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

		mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
		FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), &srvDesc, mShaderResourceView.GetDescriptorHandle());

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.Buffer.NumElements = static_cast<UINT>(mBufferSize / 4);
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		mUnorderedAccessView = FGraphicsCore::DescriptorAllocator->AllocateUAVDescriptor();
		FGraphicsCore::Device->CreateUnorderedAccessView(mResource.Get(), nullptr, &uavDesc, mUnorderedAccessView.GetDescriptorHandle());
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FStructuredBuffer::GetCounterBufferShaderResourceView() const
	{	
		return mCounterBuffer.GetShaderResourceView();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FStructuredBuffer::GetCounterBufferUnorderedAccessView() const
	{
		return mCounterBuffer.GetUnorderedAccessView();
	}

	void FStructuredBuffer::CreateViews()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = mElementCount;
		srvDesc.Buffer.StructureByteStride = mElementSize;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
		FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), &srvDesc, mShaderResourceView.GetDescriptorHandle());

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.NumElements = mElementCount;
		uavDesc.Buffer.StructureByteStride = mElementSize;
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		mCounterBuffer.Create("FStructuredBuffer::CounterBuffer", 1, 4);

		mUnorderedAccessView = FGraphicsCore::DescriptorAllocator->AllocateUAVDescriptor();
		FGraphicsCore::Device->CreateUnorderedAccessView(mResource.Get(), mCounterBuffer.GetResource(), &uavDesc, mUnorderedAccessView.GetDescriptorHandle());
	}

	void FTypedBuffer::CreateViews()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Format = D3DFormat(mFormat);
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = mElementCount;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
		FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), &srvDesc, mShaderResourceView.GetDescriptorHandle());

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = D3DFormat(mFormat);
		uavDesc.Buffer.NumElements = mElementCount;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		mUnorderedAccessView = FGraphicsCore::DescriptorAllocator->AllocateUAVDescriptor();
		FGraphicsCore::Device->CreateUnorderedAccessView(mResource.Get(), nullptr, &uavDesc, mUnorderedAccessView.GetDescriptorHandle());
	}

	D3D12_VERTEX_BUFFER_VIEW FGpuVertexBuffer::GetVertexBufferView(size_t offset, uint32_t size, uint32_t stride) const
	{
		D3D12_VERTEX_BUFFER_VIEW view{};
		view.BufferLocation = mGpuVirtualAddress + offset;
		view.SizeInBytes = size;
		view.StrideInBytes = stride;
		return view;
	}

	D3D12_INDEX_BUFFER_VIEW FGpuIndexBuffer::GetIndexBufferView(size_t offset, uint32_t size, bool is32Bit /*= false*/) const
	{
		D3D12_INDEX_BUFFER_VIEW view{};
		view.BufferLocation = mGpuVirtualAddress + offset;
		view.SizeInBytes = size;
		view.Format = is32Bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		return view;
	}

}