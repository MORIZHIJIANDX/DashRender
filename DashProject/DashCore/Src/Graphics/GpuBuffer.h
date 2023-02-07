#pragma once

#include "GpuResource.h"
#include "CpuDescriptorAllocation.h"

namespace Dash
{
	class FGpuBuffer : public FGpuResource
	{
	public:
		uint64_t GetBufferSize() const { return mDesc.Size; }
		uint64_t GetElementCount() const { return mDesc.Count; }
		uint64_t GetElementSize() const { return mDesc.Stride; }

		bool SupportConstantBufferView() const;
		bool SupportShaderResourceView() const;
		bool SupportUnorderedAccessView() const;

		D3D12_CPU_DESCRIPTOR_HANDLE GetConstantBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView() const;

	protected:
		FGpuBuffer() {}
		virtual ~FGpuBuffer() {}

		void Create(const std::string& name, uint32_t numElements, uint32_t elementSize, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
		void CreateBufferResource(const D3D12_RESOURCE_DESC& desc);
		virtual void CreateViews() = 0;

	protected:
		FBufferDescription mDesc;

		FCpuDescriptorAllocation mConstantBufferView;
		FCpuDescriptorAllocation mShaderResourceView;
		FCpuDescriptorAllocation mUnorderedAccessView;

		bool mCpuAccess = false;
	};

	// 大部分情况下不需要使用 CreateConstantBufferView 来创建 constant buffer view ，且 constant buffer 更新需要对 resource 进行 map 和 unmap 或者是 CopyBuffer. 
	// 可以使用 GpuLinearAllocator 进行代替.
	class FGpuConstantBuffer : public FGpuBuffer
	{
	public:
		FGpuConstantBuffer(){ mCpuAccess = true; }
		virtual ~FGpuConstantBuffer();
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(size_t offset = 0) const;
		
		void* Map();
		void Unmap();

	protected:
		virtual void CreateViews() override;
	
	private:
		void* mMappedData = nullptr;
	};

	class FByteAddressBuffer : public FGpuBuffer
	{
	public:
		friend class FStructuredBuffer;
		virtual ~FByteAddressBuffer() {}

	protected:
		virtual void CreateViews() override;
	};

	class FStructuredBuffer : public FGpuBuffer
	{
	public:
		virtual ~FStructuredBuffer() {}

		virtual void Destroy()
		{
			mCounterBuffer.Destroy();
			FGpuBuffer::Destroy();
		}

		FByteAddressBuffer& GetCounterBuffer() { return mCounterBuffer; };

		D3D12_CPU_DESCRIPTOR_HANDLE GetCounterBufferShaderResourceView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCounterBufferUnorderedAccessView() const;

	protected:
		virtual void CreateViews() override;

	protected:
		FByteAddressBuffer mCounterBuffer;
	};

	class FTypedBuffer : public FGpuBuffer
	{
	public:
		FTypedBuffer(EResourceFormat format)
			: mFormat(format)
		{}

		virtual ~FTypedBuffer() {}

		EResourceFormat GetFormat() const { return mFormat; };

	protected:
		virtual void CreateViews() override;

	protected:
		EResourceFormat mFormat;
	};

	class FGpuVertexBuffer : public FGpuBuffer
	{
	public:
		virtual ~FGpuVertexBuffer() {}

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
		virtual ~FGpuIndexBuffer() {}

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
		bool mIs32Bit = false;
	};

	class FGpuDynamicVertexBuffer : public FGpuVertexBuffer
	{
	public:
		FGpuDynamicVertexBuffer() { mCpuAccess = true; }
		virtual ~FGpuDynamicVertexBuffer() {}

		void* Map();
		void Unmap();

	private:
		void* mMappedData = nullptr;
	};

	class FGpuDynamicIndexBuffer : public FGpuIndexBuffer
	{
	public:
		FGpuDynamicIndexBuffer() { mCpuAccess = true; }
		virtual ~FGpuDynamicIndexBuffer() {}

		void* Map();
		void Unmap();

	private:
		void* mMappedData = nullptr;
	};
}