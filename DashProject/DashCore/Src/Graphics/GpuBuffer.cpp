#include "PCH.h"
#include "GpuBuffer.h"
#include "GraphicsCore.h"
#include "CommandContext.h"
#include "CpuDescriptorAllocator.h"
#include "GpuResourcesStateTracker.h"
#include "RenderDevice.h"

namespace Dash
{
	bool FGpuBuffer::SupportConstantBufferView() const
	{
		return mConstantBufferView.IsValid();
	}

	bool FGpuBuffer::SupportShaderResourceView() const
	{
		return mShaderResourceView.IsValid();
	}

	bool FGpuBuffer::SupportUnorderedAccessView() const
	{
		return mUnorderedAccessView.IsValid();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FGpuBuffer::GetConstantBufferView() const
	{
		return mConstantBufferView.GetDescriptorHandle();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FGpuBuffer::GetShaderResourceView() const
	{
		return mShaderResourceView.GetDescriptorHandle();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FGpuBuffer::GetUnorderedAccessView() const
	{
		return mUnorderedAccessView.GetDescriptorHandle();
	}

	void FGpuBuffer::InitResource(const std::string& name, uint32 numElements, uint32 elementSize, D3D12_RESOURCE_FLAGS flags /*D3D12_RESOURCE_FLAG_NONE*/)
	{
		Destroy();

		mDesc = FBufferDescription::Create(elementSize, numElements, mCpuAccess);

		D3D12_RESOURCE_DESC resourceDesc = mDesc.D3DResourceDescription();
		resourceDesc.Flags |= flags;

		CreateBufferResource(resourceDesc);

		CreateViews();

		SetName(name);
	}

	void FGpuBuffer::CreateBufferResource(const D3D12_RESOURCE_DESC& desc)
	{
		// D3D12_HEAP_TYPE_DEFAULT can not be accessed by CPU
		CD3DX12_HEAP_PROPERTIES heapProps(mCpuAccess ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_STATES initD3DState = mCpuAccess ? D3D12_RESOURCE_STATE_GENERIC_READ : D3DResourceState(mDesc.InitialStateMask);
		// D3D12_RESOURCE_STATE_COMMON 不可直接作为 SRV 和 UAV，需要转换为 D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER (D3D12_RESOURCE_STATE_GENERIC_READ) 状态
		TRefCountPtr<ID3D12Resource> d3d12Resource = FGraphicsCore::Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, initD3DState, nullptr);

		mResource = MakeRefCounted<FD3D12Resource>(d3d12Resource, desc, nullptr, heapProps.Type);

		FGpuResourcesStateTracker::AddGlobalResourceState(this->GetResource(), initD3DState);

		//GetGPUVirtualAddress is only useful for buffer resources, it will return zero for all texture resources.

		ASSERT(GetGpuVirtualAddress() != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN && GetGpuVirtualAddress() != D3D12_GPU_VIRTUAL_ADDRESS_NULL);
	}

	FGpuConstantBuffer::~FGpuConstantBuffer()
	{
		Unmap();
	}

	D3D12_GPU_VIRTUAL_ADDRESS FGpuConstantBuffer::GetGpuVirtualAddress(size_t offset /*= 0*/) const
	{
		return FGpuBuffer::GetGpuVirtualAddress() + mDesc.Stride * offset;
	}

	void* FGpuConstantBuffer::Map()
	{
		return mResource->Map();
	}

	void FGpuConstantBuffer::Unmap()
	{
		mResource->Unmap();
	}

	void FGpuConstantBuffer::CreateViews()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Count);
		srvDesc.Buffer.StructureByteStride = static_cast<UINT>(mDesc.Stride);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

		mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
		FGraphicsCore::Device->CreateShaderResourceView(mResource.GetReference(), &srvDesc, mShaderResourceView.GetDescriptorHandle());

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
		cbvDesc.BufferLocation = mResource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = static_cast<UINT>(mDesc.Size);

		mConstantBufferView = FGraphicsCore::DescriptorAllocator->AllocateCBVDescriptor();
		FGraphicsCore::Device->CreateConstantBufferView(&cbvDesc, mConstantBufferView.GetDescriptorHandle());
	}

	void FByteAddressBuffer::CreateViews()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Size / 4);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

		mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
		FGraphicsCore::Device->CreateShaderResourceView(mResource.GetReference(), &srvDesc, mShaderResourceView.GetDescriptorHandle());

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Size / 4);
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		mUnorderedAccessView = FGraphicsCore::DescriptorAllocator->AllocateUAVDescriptor();
		FGraphicsCore::Device->CreateUnorderedAccessView(mResource.GetReference(), nullptr, &uavDesc, mUnorderedAccessView.GetDescriptorHandle());
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
		srvDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Count);
		srvDesc.Buffer.StructureByteStride = static_cast<UINT>(mDesc.Stride);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
		FGraphicsCore::Device->CreateShaderResourceView(mResource.GetReference(), &srvDesc, mShaderResourceView.GetDescriptorHandle());

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Count);
		uavDesc.Buffer.StructureByteStride = static_cast<UINT>(mDesc.Stride);
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		mCounterBuffer.InitResource("FStructuredBuffer::CounterBuffer", 1, 4);

		mUnorderedAccessView = FGraphicsCore::DescriptorAllocator->AllocateUAVDescriptor();
		FGraphicsCore::Device->CreateUnorderedAccessView(mResource.GetReference(), mCounterBuffer.GetResource(), &uavDesc, mUnorderedAccessView.GetDescriptorHandle());
	}

	void FTypedBuffer::CreateViews()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Format = D3DFormat(mFormat);
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Count);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
		FGraphicsCore::Device->CreateShaderResourceView(mResource.GetReference(), &srvDesc, mShaderResourceView.GetDescriptorHandle());

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = D3DFormat(mFormat);
		uavDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Count);
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		mUnorderedAccessView = FGraphicsCore::DescriptorAllocator->AllocateUAVDescriptor();
		FGraphicsCore::Device->CreateUnorderedAccessView(mResource.GetReference(), nullptr, &uavDesc, mUnorderedAccessView.GetDescriptorHandle());
	}

	D3D12_VERTEX_BUFFER_VIEW FGpuVertexBuffer::GetVertexBufferView(uint32 offset, uint32 size, uint32 stride) const
	{
		D3D12_VERTEX_BUFFER_VIEW view{};
		view.BufferLocation = GetGpuVirtualAddress() + offset;
		view.SizeInBytes = size - offset;
		view.StrideInBytes = stride;
		return view;
	}

	D3D12_INDEX_BUFFER_VIEW FGpuIndexBuffer::GetIndexBufferView(uint32 offset, uint32 size) const
	{
		ASSERT((mDesc.Stride == 4 && mIs32Bit) || (mDesc.Stride == 2 && !mIs32Bit));

		D3D12_INDEX_BUFFER_VIEW view{};
		view.BufferLocation = GetGpuVirtualAddress() + offset;
		view.SizeInBytes = size - offset;
		view.Format = mIs32Bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		return view;
	}

	void FGpuIndexBuffer::InitResource(const std::string& name, uint32 numElements, bool is32Bit)
	{
		FGpuBuffer::InitResource(name, numElements, is32Bit ? sizeof(uint32) : sizeof(uint16));
		mIs32Bit = is32Bit;
	}

	void* FGpuDynamicVertexBuffer::Map()
	{
		return mResource->Map();;
	}

	void FGpuDynamicVertexBuffer::Unmap()
	{
		mResource->Unmap();
	}

	void FGpuDynamicVertexBuffer::UpdateData(void* data, size_t size)
	{
		ASSERT(data != nullptr);
		//ASSERT(size == mDesc.Size);

		Map();

		memcpy(mResource->GetMappedAddress(), data, size);

		Unmap();
	}

	void* FGpuDynamicIndexBuffer::Map()
	{
		return mResource->Map();
	}

	void FGpuDynamicIndexBuffer::Unmap()
	{
		mResource->Unmap();
	}

	void FGpuDynamicIndexBuffer::UpdateData(void* data, size_t size)
	{
		ASSERT(data != nullptr);
		ASSERT(size == mDesc.ResourceSizeInBytes());

		Map();

		memcpy(mResource->GetMappedAddress(), data, size);

		Unmap();
	}
}