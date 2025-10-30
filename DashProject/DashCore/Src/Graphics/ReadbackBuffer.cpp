#include "PCH.h"
#include "ReadbackBuffer.h"
#include "GraphicsCore.h"
#include "RenderDevice.h"

namespace Dash
{
	FReadbackBuffer::FReadbackBuffer()
	{
		mDesiredHeapType = D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK;
	}

	FReadbackBuffer::~FReadbackBuffer()
	{
		Unmap();
	}

	void* FReadbackBuffer::Map()
	{
		return mResource->Map();;
	}

	void FReadbackBuffer::Unmap()
	{
		mResource->Unmap();
	}
}
