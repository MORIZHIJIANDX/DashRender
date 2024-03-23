#include "PCH.h"
#include "QueryHeap.h"
#include "GraphicsCore.h"
#include "RenderDevice.h"

namespace Dash
{
	FQueryHeap::FQueryHeap(const FQueryHeapDesc& desc)
	{
		CreateQueryHeap(desc);
	}

	FQueryHeap::~FQueryHeap()
	{
		mQueryHeap = nullptr;
	}

	ID3D12QueryHeap* FQueryHeap::D3DQueryHeap()
	{
		return mQueryHeap.Get();
	}

	const ID3D12QueryHeap* FQueryHeap::D3DQueryHeap() const
	{
		return mQueryHeap.Get();
	}

	FQueryHeap::operator ID3D12QueryHeap* () const
	{
		return mQueryHeap.Get();
	}

	const FQueryHeapDesc& FQueryHeap::GetDesc() const
	{
		return mDesc;
	}

	void FQueryHeap::CreateQueryHeap(const FQueryHeapDesc& desc)
	{
		mDesc = desc;
		
		D3D12_QUERY_HEAP_DESC d3dDesc;
		d3dDesc.Count = desc.count;
		d3dDesc.Type = D3DQueryHeapType(desc.type);
		d3dDesc.NodeMask = 0;

		FGraphicsCore::Device->CreateQueryHeap(&d3dDesc, mQueryHeap);
	}

	D3D12_QUERY_HEAP_TYPE D3DQueryHeapType(EGpuQueryType queryType)
	{
		switch (queryType)
		{
		case EGpuQueryType::Occlusion:
		case EGpuQueryType::BinaryOcclusion:
			return D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_OCCLUSION;
		case EGpuQueryType::Timestamp:
			return D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
		case EGpuQueryType::PipelineStatistics:
			return D3D12_QUERY_HEAP_TYPE::D3D12_QUERY_HEAP_TYPE_PIPELINE_STATISTICS;
		default: ASSERT_MSG(false, "Should never be hit"); return {};
		}
	}

	D3D12_QUERY_TYPE D3DQueryType(EGpuQueryType queryType)
	{
		switch (queryType)
		{
		case EGpuQueryType::Occlusion:
			return D3D12_QUERY_TYPE::D3D12_QUERY_TYPE_OCCLUSION;
		case EGpuQueryType::BinaryOcclusion:
			return D3D12_QUERY_TYPE::D3D12_QUERY_TYPE_BINARY_OCCLUSION;
		case EGpuQueryType::Timestamp:
			return D3D12_QUERY_TYPE::D3D12_QUERY_TYPE_TIMESTAMP;
		case EGpuQueryType::PipelineStatistics:
			return D3D12_QUERY_TYPE::D3D12_QUERY_TYPE_PIPELINE_STATISTICS;
		default: ASSERT_MSG(false, "Should never be hit"); return {};
		}
	}
}