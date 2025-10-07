#include "PCH.h"
#include "DynamicDescriptorHeap.h"
#include "GraphicsCore.h"
#include "RootSignature.h"
#include "DX12Helper.h"
#include "CommandContext.h"
#include "RenderDevice.h"

namespace Dash
{
	FDynamicDescriptorHeap::FDynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 numDescriptorsPerHeap)
		: mDescriptorHeapType(type)
		, mNumDescriptorsPerHeap(numDescriptorsPerHeap)
		, mDescriptorTableBitMask(0)
		, mStaleDescriptorTableBitMask(0)
		, mStaleCBVBitMask(0)
		, mStaleSRVBitMask(0)
		, mStaleUAVBitMask(0)
		, mNumFreeHandles(0)
	{
		mDescriptorHandleIncrementSize = FGraphicsCore::Device->GetDescriptorHandleIncrementSize(type);

		mDescriptorHandleCache = std::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(mNumDescriptorsPerHeap);

		for (uint32 index = 0; index < mNumDescriptorsPerHeap; index++)
		{
			mDescriptorHandleCache[index] = D3D12_CPU_DESCRIPTOR_HANDLE{ SIZE_T(-1) };
		}

		for (uint32 index = 0; index < MaxDescriptorTables; ++index)
		{
			mInlineCBV[index] = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			mInlineSRV[index] = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			mInlineUAV[index] = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
		}
	}

	FDynamicDescriptorHeap::~FDynamicDescriptorHeap()
	{
		Destroy();
	}

	void FDynamicDescriptorHeap::StageDescriptors(uint32 rootParameterIndex, uint32 offset, uint32 numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE& srcDescriptors)
	{
		ASSERT(rootParameterIndex < MaxDescriptorTables && numDescriptors < mNumDescriptorsPerHeap);

		ASSERT_MSG((offset + numDescriptors) <= mRootSignatureDescriptorTableCache[rootParameterIndex].NumDescriptors, 
			"Number of descriptors exceeds the number of descriptors in the root signature descriptor table.");

		D3D12_CPU_DESCRIPTOR_HANDLE* descriptorHandlePtr = mRootSignatureDescriptorTableCache[rootParameterIndex].BaseDescriptor + offset;

		for (UINT index = 0; index < numDescriptors; ++index)
		{
			descriptorHandlePtr[index] = CD3DX12_CPU_DESCRIPTOR_HANDLE(srcDescriptors, index, mDescriptorHandleIncrementSize);
		}

		mStaleDescriptorTableBitMask |= (1 << rootParameterIndex);
	}

	void FDynamicDescriptorHeap::StageInlineCBV(uint32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress)
	{
		ASSERT(rootParameterIndex < MaxDescriptorTables);

		mInlineCBV[rootParameterIndex] = gpuAddress;

		mStaleCBVBitMask |= (1 << rootParameterIndex);
	}

	void FDynamicDescriptorHeap::StageInlineSRV(uint32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress)
	{
		ASSERT(rootParameterIndex < MaxDescriptorTables);

		mInlineSRV[rootParameterIndex] = gpuAddress;

		mStaleSRVBitMask |= (1 << rootParameterIndex);
	}

	void FDynamicDescriptorHeap::StageInlineUAV(uint32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress)
	{
		ASSERT(rootParameterIndex < MaxDescriptorTables);

		mInlineUAV[rootParameterIndex] = gpuAddress;

		mStaleUAVBitMask |= (1 << rootParameterIndex);
	}

	void FDynamicDescriptorHeap::CommitStagedDescriptorsForDraw(FGraphicsCommandContextBase& context)
	{
		CommitDescriptorTables(context, &ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
		CommitInlineDescriptors(context, mInlineCBV, mStaleCBVBitMask, &ID3D12GraphicsCommandList::SetGraphicsRootConstantBufferView);
		CommitInlineDescriptors(context, mInlineSRV, mStaleSRVBitMask, &ID3D12GraphicsCommandList::SetGraphicsRootShaderResourceView);
		CommitInlineDescriptors(context, mInlineUAV, mStaleUAVBitMask, &ID3D12GraphicsCommandList::SetGraphicsRootUnorderedAccessView);
	}

	void FDynamicDescriptorHeap::CommitStagedDescriptorsForDispatch(FComputeCommandContextBase& context)
	{
		CommitDescriptorTables(context, &ID3D12GraphicsCommandList::SetComputeRootDescriptorTable);
		CommitInlineDescriptors(context, mInlineCBV, mStaleCBVBitMask, &ID3D12GraphicsCommandList::SetComputeRootConstantBufferView);
		CommitInlineDescriptors(context, mInlineSRV, mStaleSRVBitMask, &ID3D12GraphicsCommandList::SetComputeRootShaderResourceView);
		CommitInlineDescriptors(context, mInlineUAV, mStaleUAVBitMask, &ID3D12GraphicsCommandList::SetComputeRootUnorderedAccessView);
	}

	void FDynamicDescriptorHeap::ParseRootSignature(const FRootSignature& rootSignature)
	{
		ASSERT(rootSignature.IsFinalized());

		mStaleDescriptorTableBitMask = 0;

		mDescriptorTableBitMask = rootSignature.GetDescriptorTableBitMask(mDescriptorHeapType);
		uint32 descriptorTableBitMask = mDescriptorTableBitMask;

		uint32 currentOffset = 0;
		DWORD rootParameterIndex;
		while (_BitScanForward(&rootParameterIndex, descriptorTableBitMask))
		{
			uint32 numDescriptors = rootSignature.GetNumDescriptors(rootParameterIndex);

			ASSERT(numDescriptors > 0);

			FDescriptorTableCache& descriptorTableCache = mRootSignatureDescriptorTableCache[rootParameterIndex];
			descriptorTableCache.NumDescriptors = numDescriptors;
			descriptorTableCache.BaseDescriptor = mDescriptorHandleCache.get() + currentOffset;

			currentOffset += numDescriptors;

			// Flip the descriptor table bit so it's not scanned again for the current index.
			descriptorTableBitMask ^= (1 << rootParameterIndex);
		}

		// Make sure the maximum number of descriptors per descriptor heap has not been exceeded.
		ASSERT_MSG(
			currentOffset <= mNumDescriptorsPerHeap,
			"The root signature requires more than the maximum number of descriptors per descriptor heap. Consider increasing the maximum number of descriptors per descriptor heap.");
	}

	D3D12_GPU_DESCRIPTOR_HANDLE FDynamicDescriptorHeap::CopyAndSetDescriptor(FComputeCommandContextBase& context, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptor)
	{
		if (mCurrentDescriptorHeap == nullptr || mNumFreeHandles < 1)
		{
			mCurrentDescriptorHeap = RequestDescriptorHeap();
			mCurrentCpuDescriptorHandle = mCurrentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			mCurrentGpuDescriptorHandle = mCurrentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			mNumFreeHandles = mNumDescriptorsPerHeap;

			mStaleDescriptorTableBitMask = mDescriptorTableBitMask;
		}

		context.SetDescriptorHeap(mDescriptorHeapType, mCurrentDescriptorHeap);

		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = mCurrentGpuDescriptorHandle;
		FGraphicsCore::Device->CopyDescriptorsSimple(1, mCurrentCpuDescriptorHandle, srcDescriptor, mDescriptorHeapType);

		mCurrentCpuDescriptorHandle.Offset(1, mDescriptorHandleIncrementSize);
		mCurrentGpuDescriptorHandle.Offset(1, mDescriptorHandleIncrementSize);

		return gpuHandle;
	}

	void FDynamicDescriptorHeap::Reset()
	{
		mAvailableDescriptorHeaps = mDescriptorHeapPool;
		mCurrentDescriptorHeap = nullptr;

		mCurrentGpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
		mCurrentCpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);

		mNumFreeHandles = 0;
		mDescriptorTableBitMask = 0;
		mStaleDescriptorTableBitMask = 0;
		mStaleCBVBitMask = 0;
		mStaleSRVBitMask = 0;
		mStaleUAVBitMask = 0;

		for (uint32 index = 0; index < MaxDescriptorTables; ++index)
		{
			mRootSignatureDescriptorTableCache[index].Reset();
			mInlineCBV[index] = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			mInlineSRV[index] = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
			mInlineUAV[index] = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
		}
	}

	void FDynamicDescriptorHeap::Destroy()
	{
		mAvailableDescriptorHeaps = {};
		mDescriptorHeapPool = {};
	}

	ID3D12DescriptorHeap* FDynamicDescriptorHeap::RequestDescriptorHeap()
	{
		ID3D12DescriptorHeap* descriptorHeap = nullptr;

		if (!mAvailableDescriptorHeaps.empty())
		{
			descriptorHeap = mAvailableDescriptorHeaps.front().GetReference();
			mAvailableDescriptorHeaps.pop();
		}
		else
		{
			descriptorHeap = CreateDescriptorHeap();
		}

		return descriptorHeap;
	}

	ID3D12DescriptorHeap* FDynamicDescriptorHeap::CreateDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;
		desc.NumDescriptors = mNumDescriptorsPerHeap;
		desc.Type = mDescriptorHeapType;

		TRefCountPtr<ID3D12DescriptorHeap> descriptorHeap;
		DX_CALL(FGraphicsCore::Device->CreateDescriptorHeap(&desc, descriptorHeap));
		mDescriptorHeapPool.push(descriptorHeap);

		SetD3D12DebugName(descriptorHeap.GetReference(), "DynamicDescriptorHeap");

		return descriptorHeap.GetReference();
	}

	uint32 FDynamicDescriptorHeap::ComputeStaleDescriptorCount() const
	{
		uint32 staleDescriptorTableBitMask = mStaleDescriptorTableBitMask;

		uint32 numStaleDescriptors = 0;
		DWORD rootParameterIndex;
		while (_BitScanForward(&rootParameterIndex, staleDescriptorTableBitMask))
		{
			numStaleDescriptors += mRootSignatureDescriptorTableCache[rootParameterIndex].NumDescriptors;
			
			// Flip the descriptor table bit so it's not scanned again for the current index.
			staleDescriptorTableBitMask ^= (1 << rootParameterIndex);
		}

		return numStaleDescriptors;
	}

	void FDynamicDescriptorHeap::CommitDescriptorTables(FComputeCommandContextBase& context, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc)
	{
		// Compute the number of descriptors that need to be copied
		uint32 staleDescriptorCount = ComputeStaleDescriptorCount();

		if (staleDescriptorCount > 0)
		{
			if (!mCurrentDescriptorHeap || mNumFreeHandles < staleDescriptorCount)
			{
				mCurrentDescriptorHeap = RequestDescriptorHeap();
				mCurrentCpuDescriptorHandle = mCurrentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				mCurrentGpuDescriptorHandle = mCurrentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				mNumFreeHandles = mNumDescriptorsPerHeap;

				context.SetDescriptorHeap(mDescriptorHeapType, mCurrentDescriptorHeap);
				mStaleDescriptorTableBitMask = mDescriptorTableBitMask;
			}

			DWORD rootParameterIndex;
			while (_BitScanForward(&rootParameterIndex, mStaleDescriptorTableBitMask))
			{
				UINT srcNumDescriptors = mRootSignatureDescriptorTableCache[rootParameterIndex].NumDescriptors;
				D3D12_CPU_DESCRIPTOR_HANDLE* srcDescriptorHandlePtr = mRootSignatureDescriptorTableCache[rootParameterIndex].BaseDescriptor;

				UINT destDescriptorRanges[] = { srcNumDescriptors };
				D3D12_CPU_DESCRIPTOR_HANDLE destDescriptorHandles[] = { mCurrentCpuDescriptorHandle };

				ASSERT_MSG(srcDescriptorHandlePtr[0].ptr != SIZE_T(-1), "Src descriptor has not been staged.");

				FGraphicsCore::Device->CopyDescriptors(1, destDescriptorHandles, destDescriptorRanges, srcNumDescriptors, srcDescriptorHandlePtr, nullptr, mDescriptorHeapType);

				setFunc(context.GetD3DCommandList(), rootParameterIndex, mCurrentGpuDescriptorHandle);

				mCurrentCpuDescriptorHandle.Offset(srcNumDescriptors, mDescriptorHandleIncrementSize);
				mCurrentGpuDescriptorHandle.Offset(srcNumDescriptors, mDescriptorHandleIncrementSize);
				mNumFreeHandles -= srcNumDescriptors;

				// Flip the stale bit so the descriptor table is not recopied again unless it is updated with a new
				// descriptor.
				mStaleDescriptorTableBitMask ^= (1 << rootParameterIndex);
			}
		}
	}

	void FDynamicDescriptorHeap::CommitInlineDescriptors(FCommandContext& context, const D3D12_GPU_VIRTUAL_ADDRESS* gpuAddressArray, uint32& bitMask, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_VIRTUAL_ADDRESS)> setFunc)
	{
		if (bitMask != 0)
		{
			DWORD rootParameterIndex;
			while (_BitScanForward(&rootParameterIndex, bitMask))
			{
				ASSERT(gpuAddressArray[rootParameterIndex] != D3D12_GPU_VIRTUAL_ADDRESS_NULL);
				setFunc(context.GetD3DCommandList(), rootParameterIndex, gpuAddressArray[rootParameterIndex]);

				bitMask ^= (1 << rootParameterIndex);
			}
		}
	}

}