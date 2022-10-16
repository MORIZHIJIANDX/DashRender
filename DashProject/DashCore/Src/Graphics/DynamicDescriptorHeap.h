#pragma once

#include "d3dx12.h"
#include <wrl.h>

namespace Dash
{
	class FCommandContext;
	class FRootSignature;

	class FDynamicDescriptorHeap
	{
	public:
		FDynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap = 1024);
		~FDynamicDescriptorHeap();

		void StageDescriptors(uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors);

		void StageInlineCBV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress);

		void StageInlineSRV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress);

		void StageInlineUAV(uint32_t rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress);

		void CommitStagedDescriptorsForDraw(FCommandContext& context);
		void CommitStagedDescriptorsForDispatch(FCommandContext& context);

		void ParseRootSignature(const FRootSignature& rootSignature);

		void Reset();

		void Destroy();

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RequestDescriptorHeap();
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap();

		uint32_t ComputeStaleDescriptorCount() const;

		void CommitDescriptorTables(FCommandContext& context, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc);
		void CommitInlineDescriptors(FCommandContext& context, const D3D12_GPU_VIRTUAL_ADDRESS* gpuAddressArray, uint32_t& bitMask, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_VIRTUAL_ADDRESS)> setFunc);

		static const uint32_t MaxDescriptorTables = 32;

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

			uint32_t NumDescriptors;
			D3D12_CPU_DESCRIPTOR_HANDLE* BaseDescriptor;
		};

		D3D12_DESCRIPTOR_HEAP_TYPE mDescriptorHeapType;

		uint32_t mNumDescriptorsPerHeap;

		uint32_t mDescriptorHandleIncrementSize;

		std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE[]> mDescriptorHandleCache;

		FDescriptorTableCache mRootSignatureDescriptorTableCache[MaxDescriptorTables];

		D3D12_GPU_VIRTUAL_ADDRESS mInlineCBV[MaxDescriptorTables];

		D3D12_GPU_VIRTUAL_ADDRESS mInlineSRV[MaxDescriptorTables];

		D3D12_GPU_VIRTUAL_ADDRESS mInlineUAV[MaxDescriptorTables];

		uint32_t mDescriptorTableBitMask;

		uint32_t mStaleDescriptorTableBitMask;

		uint32_t mStaleCBVBitMask;

		uint32_t mStaleSRVBitMask;

		uint32_t mStaleUAVBitMask;

		using DescriptorHeapPool = std::queue<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>>;

		DescriptorHeapPool mDescriptorHeapPool;
		DescriptorHeapPool mAvailableDescriptorHeaps;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCurrentDescriptorHeap;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mCurrentGpuDescriptorHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE mCurrentCpuDescriptorHandle;

		uint32_t mNumFreeHandles;
	};
}