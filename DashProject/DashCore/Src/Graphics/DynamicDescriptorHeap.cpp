#include "PCH.h"
#include "DynamicDescriptorHeap.h"
#include "GraphicsCore.h"

namespace Dash
{
	FDynamicDescriptorHeap::FDynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap)
		: mDescriptorHeapType(type)
		, mNumDescriptorsPerHeap(numDescriptorsPerHeap)
		, mDescriptorTableBitMask(0)
		, mStaleDescriptorTableBitMask(0)
		, mStaleCBVBitMask(0)
		, mStaleSRVBitMask(0)
		, mStaleUAVBitMask(0)
	{
		mDescriptorHandleIncrementSize = FGraphicsCore::Device->GetDescriptorHandleIncrementSize(type);

		mDescriptorHandleCache = std::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(mNumDescriptorsPerHeap);
	}

	void FDynamicDescriptorHeap::StageDescriptors(uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors)
	{
		ASSERT(rootParameterIndex < MaxDescriptorTables && numDescriptors < mNumDescriptorsPerHeap);
	}
}