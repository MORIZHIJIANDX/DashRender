#pragma once

#include "GpuResource.h"
#include "CpuDescriptorAllocation.h"

namespace Dash
{
	class FGpuBuffer : public FGpuResource
	{
	public:
		uint32 GetBufferSize() const { return mDesc.Size; }
		uint32 GetElementCount() const { return mDesc.Count; }
		uint32 GetElementSize() const { return mDesc.Stride; }
		bool GetCpuAccess() const { return mCpuAccess; }

		bool SupportConstantBufferView() const;
		bool SupportShaderResourceView() const;
		bool SupportUnorderedAccessView() const;

		D3D12_CPU_DESCRIPTOR_HANDLE GetConstantBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView() const;

	protected:
		FGpuBuffer() {}
		virtual ~FGpuBuffer() {}

		void InitResource(const std::string& name, uint32 numElements, uint32 elementSize, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
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
		friend class FRenderDevice;
	public:
		virtual ~FGpuConstantBuffer();
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(size_t offset = 0) const;
		
		void* Map();
		void Unmap();

	protected:
		FGpuConstantBuffer() { mCpuAccess = true; }

		virtual void CreateViews() override;
	};

	class FByteAddressBuffer : public FGpuBuffer
	{
		friend class FRenderDevice;
	public:
		friend class FStructuredBuffer;
		virtual ~FByteAddressBuffer() {}

	protected:
		virtual void CreateViews() override;
	};

	class FStructuredBuffer : public FGpuBuffer
	{
		friend class FRenderDevice;
	public:
		virtual ~FStructuredBuffer() {}

		virtual void Destroy() override
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
		friend class FRenderDevice;
	public:
		virtual ~FTypedBuffer() {}

		EResourceFormat GetFormat() const { return mFormat; };

	protected:
		FTypedBuffer(EResourceFormat format)
			: mFormat(format)
		{
		}
		virtual void CreateViews() override;

	protected:
		EResourceFormat mFormat;
	};

	class FGpuVertexBuffer : public FGpuBuffer
	{
		friend class FRenderDevice;
	public:
		virtual ~FGpuVertexBuffer() {}

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(uint32 offset, uint32 size, uint32 stride) const;
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView(uint32 baseVertexIndex = 0) const
		{
			ASSERT(baseVertexIndex < mDesc.Count);
			uint32 offset = baseVertexIndex * mDesc.Stride;
			return GetVertexBufferView(offset, static_cast<uint32>(mDesc.Size), static_cast<uint32>(mDesc.Stride));
		}

	protected:
		virtual void CreateViews() override {};
	};

	class FGpuIndexBuffer : public FGpuBuffer
	{
		friend class FRenderDevice;
	public:
		virtual ~FGpuIndexBuffer() {}

		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(uint32 offset, uint32 size) const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(uint32 startIndex = 0) const
		{
			ASSERT(startIndex < mDesc.Count);
			uint32 offset = startIndex * mDesc.Stride;
			return GetIndexBufferView(offset, static_cast<uint32>(mDesc.Size - offset));
		}

	protected:	
		void InitResource(const std::string& name, uint32 numElements, bool is32Bit);
		virtual void CreateViews() override {};
	
	protected:
		bool mIs32Bit = false;
	};

	class FGpuDynamicVertexBuffer : public FGpuVertexBuffer
	{
		friend class FRenderDevice;
	public:
		FGpuDynamicVertexBuffer() { mCpuAccess = true; }
		virtual ~FGpuDynamicVertexBuffer() {}

		void* Map();
		void Unmap();

		void UpdateData(void* data, size_t size);

	private:
		void* mMappedData = nullptr;
	};

	class FGpuDynamicIndexBuffer : public FGpuIndexBuffer
	{
		friend class FRenderDevice;
	public:
		FGpuDynamicIndexBuffer() { mCpuAccess = true; }
		virtual ~FGpuDynamicIndexBuffer() {}

		void* Map();
		void Unmap();

		void UpdateData(void* data, size_t size);
	};
}