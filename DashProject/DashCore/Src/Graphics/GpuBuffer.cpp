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

	void FGpuBuffer::Create(const std::string& name, uint32_t numElements, uint32_t elementSize, D3D12_RESOURCE_FLAGS flags /*D3D12_RESOURCE_FLAG_NONE*/)
	{
		Destroy();

		mDesc = FBufferDescription::Create(elementSize, numElements, mCpuAccess);

		D3D12_RESOURCE_DESC resourceDesc = mDesc.D3DResourceDescription();
		resourceDesc.Flags |= flags;

		CreateBufferResource(resourceDesc);

		CreateViews();

		SetD3D12DebugName(mResource.Get(), name.c_str());
	}

	void FGpuBuffer::CreateBufferResource(const D3D12_RESOURCE_DESC& desc)
	{
		// D3D12_HEAP_TYPE_DEFAULT can not be accessed by CPU
		CD3DX12_HEAP_PROPERTIES heapProps(mCpuAccess ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_STATES initD3DState = D3DResourceState(mDesc.InitialStateMask);
		// D3D12_RESOURCE_STATE_COMMON 不可直接作为 SRV 和 UAV，需要转换为 D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER (D3D12_RESOURCE_STATE_GENERIC_READ) 状态
		DX_CALL(FGraphicsCore::Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, initD3DState, nullptr, mResource));

		FGpuResourcesStateTracker::AddGlobalResourceState(this->GetResource(), initD3DState);

		//GetGPUVirtualAddress is only useful for buffer resources, it will return zero for all texture resources.
		mGpuVirtualAddress = mResource->GetGPUVirtualAddress();
	}

	FGpuConstantBuffer::~FGpuConstantBuffer()
	{
		Unmap();
	}

	D3D12_GPU_VIRTUAL_ADDRESS FGpuConstantBuffer::GetGpuVirtualAddress(size_t offset /*= 0*/) const
	{
		return mGpuVirtualAddress + mDesc.Stride * offset;
	}

	void* FGpuConstantBuffer::Map()
	{
		if (mMappedData == nullptr)
		{
			DX_CALL(mResource->Map(0, nullptr, &mMappedData));
		}
		
		return mMappedData;
	}

	void FGpuConstantBuffer::Unmap()
	{
		if (mMappedData)
		{
			mResource->Unmap(0, nullptr);
			mMappedData = nullptr;
		}
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
		FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), &srvDesc, mShaderResourceView.GetDescriptorHandle());

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
		FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), &srvDesc, mShaderResourceView.GetDescriptorHandle());

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Size / 4);
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
		srvDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Count);
		srvDesc.Buffer.StructureByteStride = static_cast<UINT>(mDesc.Stride);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
		FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), &srvDesc, mShaderResourceView.GetDescriptorHandle());

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Count);
		uavDesc.Buffer.StructureByteStride = static_cast<UINT>(mDesc.Stride);
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
		srvDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Count);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		mShaderResourceView = FGraphicsCore::DescriptorAllocator->AllocateSRVDescriptor();
		FGraphicsCore::Device->CreateShaderResourceView(mResource.Get(), &srvDesc, mShaderResourceView.GetDescriptorHandle());

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = D3DFormat(mFormat);
		uavDesc.Buffer.NumElements = static_cast<UINT>(mDesc.Count);
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

	void* FGpuDynamicVertexBuffer::Map()
	{
		if (mMappedData == nullptr)
		{
			DX_CALL(mResource->Map(0, nullptr, &mMappedData));
		}

		return mMappedData;
	}

	void FGpuDynamicVertexBuffer::Unmap()
	{
		if (mMappedData)
		{
			mResource->Unmap(0, nullptr);
			mMappedData = nullptr;
		}
	}

	void FGpuDynamicVertexBuffer::UpdateData(void* data, size_t size)
	{
		ASSERT(data != nullptr);
		//ASSERT(size == mDesc.Size);

		Map();

		memcpy(mMappedData, data, size);

		Unmap();
	}

	void* FGpuDynamicIndexBuffer::Map()
	{
		if (mMappedData == nullptr)
		{
			DX_CALL(mResource->Map(0, nullptr, &mMappedData));
		}

		return mMappedData;
	}

	void FGpuDynamicIndexBuffer::Unmap()
	{
		if (mMappedData)
		{
			mResource->Unmap(0, nullptr);
			mMappedData = nullptr;
		}
	}

	void FGpuDynamicIndexBuffer::UpdateData(void* data, size_t size)
	{
		ASSERT(data != nullptr);
		ASSERT(size == mDesc.ResourceSizeInBytes());

		Map();

		memcpy(mMappedData, data, size);

		Unmap();
	}
}