#pragma once

#include "Utility/RefCounting.h"

namespace Dash
{
	class FQueryHeap;
	using FQueryHeapRef = std::shared_ptr<FQueryHeap>;

	enum class EGpuQueryType : uint8
	{
		Occlusion,
		BinaryOcclusion,
		Timestamp,
		PipelineStatistics
	};

	D3D12_QUERY_HEAP_TYPE D3DQueryHeapType(EGpuQueryType queryType);
	D3D12_QUERY_TYPE D3DQueryType(EGpuQueryType queryType);

	struct FQueryHeapDesc
	{
		uint32 count;
		EGpuQueryType type;
	};

	class FQueryHeap
	{
	public:
		FQueryHeap(const FQueryHeapDesc& desc);
		~FQueryHeap();

		ID3D12QueryHeap* D3DQueryHeap();
		const ID3D12QueryHeap* D3DQueryHeap() const;
		operator ID3D12QueryHeap* () const;

		const FQueryHeapDesc& GetDesc() const;

	protected:
		void CreateQueryHeap(const FQueryHeapDesc& desc);

	private:
		FQueryHeapDesc mDesc{};
		TRefCountPtr<ID3D12QueryHeap> mQueryHeap = nullptr;
	};
}