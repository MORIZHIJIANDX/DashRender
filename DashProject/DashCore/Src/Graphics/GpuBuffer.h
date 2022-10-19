#pragma once

#include "GpuResource.h"
#include "CpuDescriptorAllocation.h"

namespace Dash
{
	class FGpuBuffer : public FGpuResource
	{
	public:
		void Create(const std::wstring& name, uint32_t numElements, uint32_t elementSize, const void* initData = nullptr, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

		size_t GetBufferSize() const { return mBufferSize; }
		uint32_t GetElementCount() const { return mElementCount; }
		uint32_t GetElementSize() const { return mElementSize; }

	protected:
		D3D12_RESOURCE_DESC DescribeBuffer();
		void CreateBufferResource(const D3D12_RESOURCE_DESC& desc);
		
		virtual void CreateViews() = 0;

	protected:
		FGpuBuffer()
			: mBufferSize(0)
			, mElementCount(0)
			, mElementSize(0)
			, mResourceFlag(D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
		{}

		size_t mBufferSize;
		uint32_t mElementCount;
		uint32_t mElementSize;
		D3D12_RESOURCE_FLAGS mResourceFlag;
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
		FTypedBuffer(DXGI_FORMAT format)
			: mFormat(format)
		{}

		DXGI_FORMAT GetFormat() const { return mFormat; };

	protected:
		virtual void CreateViews() override;

	protected:
		DXGI_FORMAT mFormat;
	};

	class FGpuVertexBuffer : public FGpuBuffer
	{
	public:
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(size_t offset, uint32_t size, uint32_t stride) const;
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(size_t baseVertexIndex = 0) const
		{
			ASSERT(baseVertexIndex < mElementCount);
			size_t offset = baseVertexIndex * mElementSize;
			return GetVertexBufferView(offset, static_cast<uint32_t>(mBufferSize - offset), mElementSize);
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
			ASSERT(startIndex < mElementCount);
			size_t offset = startIndex * mElementSize;
			return GetIndexBufferView(offset, static_cast<uint32_t>(mBufferSize - offset), mElementSize == 4);
		}

	protected:
		virtual void CreateViews() override {};

	protected:
		D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
	};
}