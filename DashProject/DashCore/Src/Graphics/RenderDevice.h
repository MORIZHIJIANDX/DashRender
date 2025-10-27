#pragma once

#include "GraphicTypesFwd.h"
#include "ResourceState.h"
#include "ResourceFormat.h"
#include "ResourceDescription.h"
#include "DX12Helper.h"
#include "dxgi1_4.h"

namespace Dash
{
	class FRenderDevice
	{
	public:
		FRenderDevice();
		~FRenderDevice();

		void Init();

		void Destroy();

		// Gets information about the features that are supported by the current graphics driver. (ID3D12Device.CheckFeatureSupport)
		HRESULT CheckFeatureSupport(
			D3D12_FEATURE feature, 
			void* pFeatureSupportData, 
			UINT featureSupportDataSize
		);

		// Copies descriptors from a source to a destination. (ID3D12Device.CopyDescriptors)
		void CopyDescriptors(
			UINT numDestDescriptorRanges, 
			const D3D12_CPU_DESCRIPTOR_HANDLE* pDestDescriptorRangeStarts, 
			const UINT* pDestDescriptorRangeSizes, 
			UINT numSrcDescriptorRanges, 
			const D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorRangeStarts,
			const UINT* pSrcDescriptorRangeSizes,
			D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapsType
		);

		// Copies descriptors from a source to a destination. (ID3D12Device.CopyDescriptorsSimple)
		void CopyDescriptorsSimple(
			UINT numDescriptors,
			D3D12_CPU_DESCRIPTOR_HANDLE destDescriptorRangeStart,
			D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptorRangeStart,
			D3D12_DESCRIPTOR_HEAP_TYPE  descriptorHeapsType
		);

		// Creates a command allocator object.
		HRESULT CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE type,
			TRefCountPtr<ID3D12CommandAllocator>& pCommandAllocator
		);

		// Creates a command list.
		HRESULT CreateCommandList(
			UINT nodeMask,
			D3D12_COMMAND_LIST_TYPE type,
			ID3D12CommandAllocator* pCommandAllocator,
			ID3D12PipelineState* pInitialState,
			TRefCountPtr<ID3D12GraphicsCommandList4>& pCommandList
		);

		// Creates a command queue.
		HRESULT CreateCommandQueue(
			const D3D12_COMMAND_QUEUE_DESC* pDesc,
			TRefCountPtr<ID3D12CommandQueue>& pCommandQueue
		);

		// This method creates a command signature.
		HRESULT CreateCommandSignature(
			const D3D12_COMMAND_SIGNATURE_DESC* pDesc,
			ID3D12RootSignature* pRootSignature,
			TRefCountPtr<ID3D12CommandSignature>& pvCommandSignature
		);

		TRefCountPtr<ID3D12Resource> CreateCommittedResource(
			const D3D12_HEAP_PROPERTIES* pHeapProperties,
			D3D12_HEAP_FLAGS            heapFlags,
			const D3D12_RESOURCE_DESC* pDesc,
			D3D12_RESOURCE_STATES       initialResourceState,
			const D3D12_CLEAR_VALUE* pOptimizedClearValue
		);

		// Creates a constant-buffer view for accessing resource data.
		void CreateConstantBufferView(
			const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE  destDescriptor
		);

		// Creates a depth-stencil view for accessing resource data.
		void CreateDepthStencilView(
			FD3D12Resource* pResource,
			const D3D12_DEPTH_STENCIL_VIEW_DESC* pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor
		);

		// Creates a descriptor heap object.
		HRESULT CreateDescriptorHeap(
			const D3D12_DESCRIPTOR_HEAP_DESC* pDescriptorHeapDesc,
			TRefCountPtr<ID3D12DescriptorHeap>& ppvHeap
		);

		// Creates a fence object. (ID3D12Device.CreateFence)
		HRESULT CreateFence(
			UINT64            initialValue,
			D3D12_FENCE_FLAGS flags,
			TRefCountPtr<ID3D12Fence>& ppFence
		);

		// Creates a pipeline state object.
		HRESULT CreatePipelineState(
			const D3D12_PIPELINE_STATE_STREAM_DESC* pDesc,
			TRefCountPtr<ID3D12PipelineState>& ppPipelineState
		);

		// Creates a heap that can be used with placed resources and reserved resources.
		HRESULT CreateHeap(
			const D3D12_HEAP_DESC* pDesc,
			TRefCountPtr<ID3D12Heap>& ppvHeap
		);

		// Creates a resource that is placed in a specific heap. Placed resources are the lightest weight resource objects available, and are the fastest to create and destroy. (ID3D12Device.CreatePlacedResource)
		HRESULT CreatePlacedResource(
			ID3D12Heap* pHeap,
			UINT64  heapOffset,
			const D3D12_RESOURCE_DESC* pDesc,
			D3D12_RESOURCE_STATES     initialState,
			const D3D12_CLEAR_VALUE* pOptimizedClearValue,
			TRefCountPtr<ID3D12Resource>& pvResource
		);

		// Creates a query heap. A query heap contains an array of queries.
		HRESULT CreateQueryHeap(
			const D3D12_QUERY_HEAP_DESC* pDesc,
			TRefCountPtr<ID3D12QueryHeap>& ppvHeap
		);

		// Creates a render-target view for accessing resource data. (ID3D12Device.CreateRenderTargetView)
		void CreateRenderTargetView(
			FD3D12Resource* pResource,
			const D3D12_RENDER_TARGET_VIEW_DESC* pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor
		);

		// Creates a resource that is reserved, and not yet mapped to any pages in a heap.
		HRESULT CreateReservedResource(
			const D3D12_RESOURCE_DESC* pDesc,
			D3D12_RESOURCE_STATES     initialState,
			const D3D12_CLEAR_VALUE* pOptimizedClearValue,
			TRefCountPtr<ID3D12Resource>& pvResource
		);

		// Creates a root signature layout.
		HRESULT CreateRootSignature(
			 UINT       nodeMask,
			const void* pBlobWithRootSignature,
			SIZE_T     blobLengthInBytes,
			TRefCountPtr<ID3D12RootSignature>& pvRootSignature
		);

		// Create a sampler object that encapsulates sampling information for a texture.
		void CreateSampler(
			const D3D12_SAMPLER_DESC* pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor
		);

		// Creates a shader-resource view for accessing data in a resource. (ID3D12Device.CreateShaderResourceView)
		void CreateShaderResourceView(
			FD3D12Resource* pResource,
			const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE           destDescriptor
		);

		// Creates a shared handle to a heap, resource, or fence object.
		HRESULT CreateSharedHandle(
			ID3D12DeviceChild* pObject,
			const SECURITY_ATTRIBUTES* pAttributes,
			DWORD  access,
			LPCWSTR name,
			HANDLE* pHandle
		);

		// Creates a view for unordered accessing.
		void CreateUnorderedAccessView(
			FD3D12Resource* pResource,
			FD3D12Resource* pCounterResource,
			const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc,
			D3D12_CPU_DESCRIPTOR_HANDLE  destDescriptor
		);

		// Enables the page-out of data, which precludes GPU access of that data.
		HRESULT Evict(
			UINT numObjects,
			ID3D12Pageable* const* ppObjects
		);

		// Gets a locally unique identifier for the current device (adapter).
		LUID GetAdapterLuid();

		// Gets a resource layout that can be copied. Helps the app fill-in D3D12_PLACED_SUBRESOURCE_FOOTPRINT and D3D12_SUBRESOURCE_FOOTPRINT when suballocating space in upload heaps.
		void GetCopyableFootprints(
			const D3D12_RESOURCE_DESC* pResourceDesc,
			UINT  firstSubresource,
			UINT  numSubresources,
			UINT64 baseOffset,
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
			UINT* pNumRows,
			UINT64* pRowSizeInBytes,
			UINT64* pTotalBytes
		);

		// Divulges the equivalent custom heap properties that are used for non-custom heap types, based on the adapter's architectural properties.
		D3D12_HEAP_PROPERTIES GetCustomHeapProperties(
			UINT            nodeMask,
			D3D12_HEAP_TYPE heapType
		);

		// Gets the size of the handle increment for the given type of descriptor heap. This value is typically used to increment a handle into a descriptor array by the correct amount.
		UINT GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType
		);

		// Gets the reason that the device was removed.
		HRESULT GetDeviceRemovedReason();

		// Reports the number of physical adapters (nodes) that are associated with this device.
		UINT GetNodeCount();

		// Gets the size and alignment of memory required for a collection of resources on this adapter.
		D3D12_RESOURCE_ALLOCATION_INFO GetResourceAllocationInfo(
			UINT                      visibleMask,
			UINT                      numResourceDescs,
			const D3D12_RESOURCE_DESC* pResourceDescs
		);

		// Gets info about how a tiled resource is broken into tiles.
		void GetResourceTiling(
			ID3D12Resource* pTiledResource,
			UINT* pNumTilesForEntireResource,
			D3D12_PACKED_MIP_INFO* pPackedMipDesc,
			D3D12_TILE_SHAPE* pStandardTileShapeForNonPackedMips,
			UINT* pNumSubresourceTilings,
			UINT  firstSubresourceTilingToGet,
			D3D12_SUBRESOURCE_TILING* pSubresourceTilingsForNonPackedMips
		);

		// Makes objects resident for the device.
		HRESULT MakeResident(
			UINT NumObjects,
			ID3D12Pageable* const* ppObjects
		);

		// A development-time aid for certain types of profiling and experimental prototyping.
		HRESULT SetStablePowerState(
			BOOL enable
		);

		FColorBufferRef CreateColorBuffer(const std::string& name, const TRefCountPtr<ID3D12Resource>& resource, EResourceState initStates = EResourceState::Common);
		FColorBufferRef CreateColorBuffer(const std::string& name, const FColorBufferDescription& desc);
		FColorBufferRef CreateColorBuffer(const std::string& name, uint32 width, uint32 height, uint32 numMips, EResourceFormat format);
		FColorBufferRef CreateColorBufferArray(const std::string& name, uint32 width, uint32 height, uint32 arrayCount, uint32 numMips, EResourceFormat format);

		FDepthBufferRef CreateDepthBuffer(const std::string& name, const FDepthBufferDescription& desc);
		FDepthBufferRef CreateDepthBuffer(const std::string& name, uint32 width, uint32 height, EResourceFormat format);
		FDepthBufferRef CreateDepthBuffer(const std::string& name, uint32 width, uint32 height, uint32 sampleCount, uint32 sampleQuality, EResourceFormat format);

		FTextureBufferRef CreateTextureBufferFromMemory(const std::string& name, const FTextureBufferDescription& desc, const std::vector<const void*>& initialMipsData);
		FTextureBufferRef CreateTextureBufferFromFile(const std::string& name, const std::string& fileName);

		FGpuVertexBufferRef CreateVertexBuffer(const std::string& name, uint32 numElements, uint32 elementSize, const void* initData = nullptr);
		template<typename VertexType>
		FGpuVertexBufferRef CreateVertexBuffer(const std::string& name, uint32 numElements, const void* initData)
		{
			return CreateVertexBuffer(name, numElements, sizeof(VertexType), initData);
		}
		template<typename VertexType>
		FGpuVertexBufferRef CreateVertexBuffer(const std::string& name, const std::vector<VertexType>& data)
		{
			return CreateVertexBuffer(name, static_cast<uint32>(data.size()), sizeof(VertexType), data.data());
		}

		FGpuIndexBufferRef CreateIndexBuffer(const std::string& name, uint32 numElements, const void* initData, bool is32Bit = false);

		FGpuDynamicVertexBufferRef CreateDynamicVertexBuffer(const std::string& name, uint32 numElements, uint32 elementSize);
		template<typename VertexType>
		FGpuDynamicVertexBufferRef CreateDynamicVertexBuffer(const std::string& name, uint32 numElements)
		{
			return CreateDynamicVertexBuffer(name, numElements, sizeof(VertexType));
		}

		FGpuDynamicIndexBufferRef CreateDynamicIndexBuffer(const std::string& name, uint32 numElements, bool is32Bit = false);

		FGpuConstantBufferRef CreateConstantBuffer(const std::string& name, uint32 sizeInBytes, const void* initData);
		template<typename ConstantBufferType>
		FGpuConstantBufferRef CreateConstantBuffer(const std::string& name, const ConstantBufferType* initData)
		{
			return CreateConstantBuffer(name, sizeof(ConstantBufferType), initData);
		}

		FStructuredBufferRef CreateStructuredBuffer(const std::string& name, uint32 numElements, uint32 elementSize, const void* initData);
		template<typename StructuredBufferType>
		FStructuredBufferRef CreateStructuredBuffer(const std::string& name, uint32 numElements, const StructuredBufferType* initData)
		{
			return CreateStructuredBuffer(name, numElements, sizeof(StructuredBufferType), initData);
		}

		FReadbackBufferRef CreateReadbackBuffer(const std::string& name, uint32 numElements, uint32 elementSize);

		bool UAVLoadSupportR11G11B10Float() const { return mTypedUAVLoadSupport_R11G11B10_FLOAT; }
		bool UAVLoadSupportR16G16B16A16Float() const { return mTypedUAVLoadSupport_R16G16B16A16_FLOAT; }
		bool SupportsUniversalHeaps() const { return mSupportsUniversalHeaps; }
		bool SupportsTearing() const { return mSupportsTearing; }
		bool SupportRayTracing() const { return mSupportRaytracing; }
		D3D_ROOT_SIGNATURE_VERSION HighestRootSignatureVersion() const { return mHighestRootSignatureVersion; }

		FD3D12VideoMemoryInfo QueryVideoMemoryInfo();

	private:
		TRefCountPtr<ID3D12Device5> mDevice = nullptr;
		TRefCountPtr<IDXGIAdapter3> mAdapter = nullptr;

		bool mTypedUAVLoadSupport_R11G11B10_FLOAT = false;
		bool mTypedUAVLoadSupport_R16G16B16A16_FLOAT = false;
		bool mSupportsUniversalHeaps = false;
		bool mSupportsTearing = false;
		bool mSupportRaytracing = false;
		bool mGPUUploadHeapSupported = false;
		D3D_ROOT_SIGNATURE_VERSION mHighestRootSignatureVersion = D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1_1;
	};
}