#include "PCH.h"
#include "ReadbackBuffer.h"
#include "GraphicsCore.h"
#include "RenderDevice.h"

namespace Dash
{
	FReadbackBuffer::FReadbackBuffer()
	{
		mCpuAccess = true;
	}

	FReadbackBuffer::~FReadbackBuffer()
	{
		Unmap();
	}

	void* FReadbackBuffer::Map()
	{
		if (mMappedData == nullptr)
		{
			DX_CALL(mResource->Map(0, nullptr, &mMappedData));
		}

		return mMappedData;
	}

	void FReadbackBuffer::Unmap()
	{
		if (mMappedData)
		{
			mResource->Unmap(0, nullptr);
			mMappedData = nullptr;
		}
	}

	void FReadbackBuffer::CreateReadbackBuffer(const std::string& name, uint32_t numElements, uint32_t elementSize)
	{
		Destroy();

		mDesc = FBufferDescription::Create(elementSize, numElements, mCpuAccess, 1, EResourceState::CopyDestination);

		// Create a readback buffer large enough to hold all texel data
		D3D12_HEAP_PROPERTIES HeapProps;
		HeapProps.Type = D3D12_HEAP_TYPE_READBACK;
		HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC desc = mDesc.D3DResourceDescription();
		DX_CALL(FGraphicsCore::Device->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &desc,
			D3DResourceState(mDesc.InitialStateMask), nullptr, mResource));

		mGpuVirtualAddress = mResource->GetGPUVirtualAddress();

		SetD3D12DebugName(mResource.Get(), name.c_str());
	}
}
