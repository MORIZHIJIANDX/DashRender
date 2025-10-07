#pragma once

#include "Utility/RefCounting.h"

namespace Dash
{
	class FCommandContext;
	class FComputeCommandContextBase;
	class FGraphicsCommandContextBase;
	class FRootSignature;

	class FDynamicDescriptorHeap
	{
	public:
		FDynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 numDescriptorsPerHeap = 1024);
		~FDynamicDescriptorHeap();

		void StageDescriptors(uint32 rootParameterIndex, uint32 offset, uint32 numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE& srcDescriptors);

		void StageInlineCBV(uint32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress);

		void StageInlineSRV(uint32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress);

		void StageInlineUAV(uint32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress);

		void CommitStagedDescriptorsForDraw(FGraphicsCommandContextBase& context);
		void CommitStagedDescriptorsForDispatch(FComputeCommandContextBase& context);

		void ParseRootSignature(const FRootSignature& rootSignature);

		D3D12_GPU_DESCRIPTOR_HANDLE CopyAndSetDescriptor(FComputeCommandContextBase& context, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptor);

		void Reset();

		void Destroy();

	private:
		ID3D12DescriptorHeap* RequestDescriptorHeap();
		ID3D12DescriptorHeap* CreateDescriptorHeap();

		uint32 ComputeStaleDescriptorCount() const;

		void CommitDescriptorTables(FComputeCommandContextBase& context, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc);
		void CommitInlineDescriptors(FCommandContext& context, const D3D12_GPU_VIRTUAL_ADDRESS* gpuAddressArray, uint32& bitMask, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_VIRTUAL_ADDRESS)> setFunc);

		static const uint32 MaxDescriptorTables = 32;

		struct FDescriptorTableCache
		{
			FDescriptorTableCache()
				: NumDescriptors(0)
				, BaseDescriptor(nullptr)
			{}

			void Reset()
			{
				NumDescriptors = 0;
				BaseDescriptor = nullptr;
			}

			uint32 NumDescriptors;
			D3D12_CPU_DESCRIPTOR_HANDLE* BaseDescriptor;
		};

		D3D12_DESCRIPTOR_HEAP_TYPE mDescriptorHeapType;

		uint32 mNumDescriptorsPerHeap;

		uint32 mDescriptorHandleIncrementSize;

		std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE[]> mDescriptorHandleCache;

		FDescriptorTableCache mRootSignatureDescriptorTableCache[MaxDescriptorTables];

		D3D12_GPU_VIRTUAL_ADDRESS mInlineCBV[MaxDescriptorTables];

		D3D12_GPU_VIRTUAL_ADDRESS mInlineSRV[MaxDescriptorTables];

		D3D12_GPU_VIRTUAL_ADDRESS mInlineUAV[MaxDescriptorTables];

		uint32 mDescriptorTableBitMask;

		uint32 mStaleDescriptorTableBitMask;

		uint32 mStaleCBVBitMask;

		uint32 mStaleSRVBitMask;

		uint32 mStaleUAVBitMask;

		using DescriptorHeapPool = std::queue<TRefCountPtr<ID3D12DescriptorHeap>>;

		DescriptorHeapPool mDescriptorHeapPool;
		DescriptorHeapPool mAvailableDescriptorHeaps;

		ID3D12DescriptorHeap* mCurrentDescriptorHeap = nullptr;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mCurrentGpuDescriptorHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mCurrentCpuDescriptorHandle;

		uint32 mNumFreeHandles;
	};
}