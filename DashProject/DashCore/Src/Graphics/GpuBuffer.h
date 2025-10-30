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
		bool GetCpuAccess() const { return mDesiredHeapType == D3D12_HEAP_TYPE_UPLOAD || mDesiredHeapType == D3D12_HEAP_TYPE_READBACK; }

		bool SupportShaderResourceView() const;
		bool SupportUnorderedAccessView() const;

		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView() const;

	protected:
		FGpuBuffer() {}
		virtual ~FGpuBuffer() {}

		void InitResource(const std::string& name, uint32 numElements, uint32 elementSize, uint32 elementAlignment = 1, EResourceState initialStateMask = EResourceState::Common);
		void CreateBufferResource(const D3D12_RESOURCE_DESC& desc);
		virtual void CreateViews() = 0;

	protected:
		FBufferDescription mDesc;

		FCpuDescriptorAllocation mShaderResourceView;
		FCpuDescriptorAllocation mUnorderedAccessView;
		
		bool mNeedUnorderedAccess = false;
		D3D12_HEAP_TYPE mDesiredHeapType = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
	};

	// �󲿷�����²���Ҫʹ�� CreateConstantBufferView ������ constant buffer view ���� constant buffer ������Ҫ�� resource ���� map �� unmap ������ CopyBuffer. 
	// ����ʹ�� GpuLinearAllocator ���д���.
	class FGpuConstantBuffer : public FGpuBuffer
	{
		friend class FRenderDevice;
	public:
		virtual ~FGpuConstantBuffer();
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(size_t offset = 0) const;
		
		void* Map();
		void Unmap();

		bool SupportConstantBufferView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetConstantBufferView() const;

	protected:
		FGpuConstantBuffer() { mDesiredHeapType = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD; }

		FCpuDescriptorAllocation mConstantBufferView;

		virtual void CreateViews() override;
	};

	class FByteAddressBuffer : public FGpuBuffer
	{
		friend class FRenderDevice;
	public:
		friend class FStructuredBuffer;
		virtual ~FByteAddressBuffer() {}

	protected:
		FByteAddressBuffer() { mNeedUnorderedAccess = true; }
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
		FStructuredBuffer() { mNeedUnorderedAccess = true; }
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
			mNeedUnorderedAccess = true;
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
		FGpuDynamicVertexBuffer() { mDesiredHeapType = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD; }
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
		FGpuDynamicIndexBuffer() { mDesiredHeapType = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD; }
		virtual ~FGpuDynamicIndexBuffer() {}

		void* Map();
		void Unmap();

		void UpdateData(void* data, size_t size);
	};
}