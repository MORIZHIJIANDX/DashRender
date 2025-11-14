#pragma once

#include "GraphicTypesFwd.h"
#include "GraphicsDefines.h"
#include "CpuDescriptorAllocation.h"


namespace Dash
{
	template <typename TType>
	struct TResourceRange
	{
		TType First = 0;
		TType Num = 0;

		TResourceRange() = default;
		TResourceRange(uint32 InFirst, uint32 InNum)
			: First(InFirst)
			, Num(InNum)
		{
			ASSERT(InFirst < TScalarTraits<TType>::Max()
				&& InNum < TScalarTraits<TType>::Max()
				&& (InFirst + InNum) < TScalarTraits<TType>::Max());
		}

		TType ExclusiveLast() const { return First + Num; }
		TType InclusiveLast() const { return First + Num - 1; }

		bool IsInRange(uint32 Value) const
		{
			ASSERT(Value < TScalarTraits<TType>::Max());
			return Value >= First && Value < ExclusiveLast();
		}
	};

	struct FD3D12ViewRange
	{
		uint32 FirstSubresource = 0;
		uint32 NumSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		TResourceRange<uint16> Mip{0, 0};
		TResourceRange<uint16> Array{ 0, 0 };
		TResourceRange<uint16> Plane{ 0, 0 };
	};

	class FD3D12View
	{
	public:
		FD3D12View(D3D12_DESCRIPTOR_HEAP_TYPE type);

		virtual ~FD3D12View(){}

	protected:
		FGpuResource* mBaseResource;
		D3D12_DESCRIPTOR_HEAP_TYPE mType;

		FCpuDescriptorAllocation mCpuDescriptor;
		// BindlessHandle
		FD3D12ViewRange mViewRange;
	};

	class FShaderResourceView : public FD3D12View
	{
	public:
		FShaderResourceView()
			: FD3D12View(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			, mViewDesc()
		{
		}
		virtual ~FShaderResourceView(){}

		void CreateView(FGpuResource* resource, const CD3DX12_SHADER_RESOURCE_VIEW_DESC& viewDesc);

	private:
		void ParseViewRange(const CD3DX12_SHADER_RESOURCE_VIEW_DESC& viewDesc);

	private:
		CD3DX12_SHADER_RESOURCE_VIEW_DESC mViewDesc;
	};

	class FUnorderedAccessView : public FD3D12View
	{
	public:
		FUnorderedAccessView()
			: FD3D12View(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			, mViewDesc()
		{
		}
		virtual ~FUnorderedAccessView() {}

		void CreateView(FGpuResource* resource, const CD3DX12_UNORDERED_ACCESS_VIEW_DESC& viewDesc, bool needCounter = false);

		FByteAddressBuffer* GetCounterBuffer() const;

	private:
		void ParseViewRange(const CD3DX12_UNORDERED_ACCESS_VIEW_DESC& viewDesc);

	private:
		CD3DX12_UNORDERED_ACCESS_VIEW_DESC mViewDesc;
		FByteAddressBufferRef mCounterBuffer;
	};

	class FRenderTargetView : public FD3D12View
	{
	public:
		FRenderTargetView()
			: FD3D12View(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
			, mViewDesc()
		{
		}
		virtual ~FRenderTargetView() {}

		void CreateView(FGpuResource* resource, const D3D12_RENDER_TARGET_VIEW_DESC& viewDesc);

	private:
		void ParseViewRange(const D3D12_RENDER_TARGET_VIEW_DESC& viewDesc);

	private:
		D3D12_RENDER_TARGET_VIEW_DESC mViewDesc;
	};

	class FDepthStencilView : public FD3D12View
	{
	public:
		FDepthStencilView()
			: FD3D12View(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
			, mViewDesc()
		{
		}
		virtual ~FDepthStencilView() {}

		void CreateView(FGpuResource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC& viewDesc);

	private:
		void ParseViewRange(const D3D12_DEPTH_STENCIL_VIEW_DESC& viewDesc);

	private:
		D3D12_DEPTH_STENCIL_VIEW_DESC mViewDesc;
	};
}