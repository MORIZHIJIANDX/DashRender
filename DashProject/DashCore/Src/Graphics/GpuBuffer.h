#pragma once

#include "GpuResource.h"
#include "CpuDescriptorAllocation.h"

namespace Dash
{
	class FGpuBuffer : public FGpuResource
	{
	public:
		void Create(const std::string& name, uint32_t numElements, uint32_t elementSize, const void* initData = nullptr, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

		uint64_t GetBufferSize() const { return mDesc.Size; }
		uint64_t GetElementCount() const { return mDesc.Count; }
		uint64_t GetElementSize() const { return mDesc.Stride; }

	protected:
		void CreateBufferResource(const D3D12_RESOURCE_DESC& desc);
		
		virtual void CreateViews() = 0;

	protected:
		FGpuBuffer()
		{}

		FBufferDescription mDesc;
	};

	// 大部分情况下不需要使用 CreateConstantBufferView 来创建 constant buffer view ，且 constant buffer 更新需要对 resource 进行 map 和 unmap 或者是 CopyBuffer. 
	// 可以使用 GpuLinearAllocator 进行代替.
	class FGpuConstantBuffer : public FGpuBuffer
	{
	public:
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(size_t offset = 0) const;

		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView() const;
	
	protected:
		FCpuDescriptorAllocation mShaderResourceView;
		FCpuDescriptorAllocation mUnorderedAccessView;
	};

	class FByteAddressBuffer : public FGpuConstantBuffer
	{
	protected:
		virtual void CreateViews() override;
	};

	class FStructuredBuffer : public FGpuConstantBuffer
	{
	public:
		virtual void Destroy()
		{
			mCounterBuffer.Destroy();
			FGpuConstantBuffer::Destroy();
		}

		FByteAddressBuffer& GetCounterBuffer() { return mCounterBuffer; };

		D3D12_CPU_DESCRIPTOR_HANDLE GetCounterBufferShaderResourceView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCounterBufferUnorderedAccessView() const;

	protected:
		virtual void CreateViews() override;

	protected:
		FByteAddressBuffer mCounterBuffer;
	};

	class FTypedBuffer : public FGpuConstantBuffer
	{
	public:
		FTypedBuffer(EResourceFormat format)
			: mFormat(format)
		{}

		EResourceFormat GetFormat() const { return mFormat; };

	protected:
		virtual void CreateViews() override;

	protected:
		EResourceFormat mFormat;
	};

	class FGpuVertexBuffer : public FGpuBuffer
	{
	public:
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(size_t offset, uint32_t size, uint32_t stride) const;
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(size_t baseVertexIndex = 0) const
		{
			ASSERT(baseVertexIndex < mDesc.Count);
			size_t offset = baseVertexIndex * mDesc.Stride;
			return GetVertexBufferView(offset, static_cast<uint32_t>(mDesc.Size - offset), static_cast<uint32_t>(mDesc.Stride));
		}

	protected:
		virtual void CreateViews() override {};
	};

	class FGpuIndexBuffer : public FGpuBuffer
	{
	public:
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(size_t offset, uint32_t size, bool is32Bit = false) const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(size_t startIndex = 0) const
		{
			ASSERT(startIndex < mDesc.Count);
			size_t offset = startIndex * mDesc.Stride;
			return GetIndexBufferView(offset, static_cast<uint32_t>(mDesc.Size - offset), mDesc.Stride == 4);
		}

	protected:
		virtual void CreateViews() override {};

	protected:
		D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
	};
}