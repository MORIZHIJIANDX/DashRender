#pragma once

#include "CommandQueue.h"
#include "ColorBuffer.h"
#include "DynamicDescriptorHeap.h"
#include "PipelineStateObject.h"
#include "DepthBuffer.h"
#include "Viewport.h"
#include "GpuBuffer.h"
#include "TextureBuffer.h"
#include "GpuLinearAllocator.h"
#include "ShaderPass.h"
#include "GpuResourcesStateTracker.h"
#include "RenderDevice.h"

namespace Dash
{
	class FCommandContext;
	class FGraphicsCommandContext;
	class FComputeCommandContext;
	class FSubResourceData;

	class FCommandContextManager
	{
	public:
		FCommandContextManager(){};

		FCommandContext* AllocateContext(D3D12_COMMAND_LIST_TYPE type);
		void FreeContext(uint64_t fenceValue, FCommandContext* context);
		void ResetAllContext();
		void Destroy();

	private:
		std::vector<std::unique_ptr<FCommandContext>> mContextPool[4];
		std::queue<FCommandContext*> mAvailableContexts[4];
		std::deque<std::pair<uint64_t, FCommandContext*>> mRetiredContexts[4];
		std::mutex mAllocationMutex;
	};

	class FCommandContext
	{
		friend FCommandContextManager;
	protected:
		
		FCommandContext(D3D12_COMMAND_LIST_TYPE type);

		// Prepare to render by reserving a command list and command allocator
		void Initialize();

		/**
		 * Reset the command list. This should only be called by the CommandQueue
		 * before the command list is returned from CommandQueue::GetCommandList.
		 */
		void Reset();

	public:
		virtual ~FCommandContext();

		//Disable Copy
		FCommandContext(const FCommandContext&) = delete;
		FCommandContext& operator=(const FCommandContext&) = delete;

		FCommandList* GetCommandList() { return mCommandList; }
		ID3D12GraphicsCommandList* GetD3DCommandList() { return mD3DCommandList; }

		void PIXBeginEvent(const std::string& label);
		void PIXEndEvent();
		void PIXSetMarker(const std::string& label);

	protected:
		
		static FCommandContext* Begin(const std::string& id = "", D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		void BindDescriptorHeaps();
		void TrackResource(FGpuResourceRef resource);
		void ReleaseTrackedObjects();
		uint64_t Execute();

		void InitParameterBindState();
		void InitStateForParameter(size_t parameterNum, std::vector<bool>& bindStateMap);
		void CheckUnboundShaderParameters();

	protected:
		D3D12_COMMAND_LIST_TYPE mType;

		FCommandList* mCommandList = nullptr;
		ID3D12GraphicsCommandList* mD3DCommandList = nullptr;

		FGpuResourcesStateTracker mResourceStateTracker;

		// Keep track of the currently bound descriptor heaps. Only change descriptor
		// heaps if they are different than the currently bound descriptor heaps.
		ID3D12DescriptorHeap* mDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		FDynamicDescriptorHeap mDynamicViewDescriptor;
		FDynamicDescriptorHeap mDynamicSamplerDescriptor;

		FGpuLinearAllocator mLinearAllocator;

		using FTrackedObjects = std::vector<FGpuResourceRef>;
		FTrackedObjects mTrackedObjects;

		ID3D12RootSignature* mCurrentRootSignature = nullptr;
		ID3D12PipelineState* mCurrentPipelineState = nullptr;

		FPipelineStateObjectRef mPSORef;

		std::vector<bool> mConstantBufferBindState;
		std::vector<bool> mShaderResourceViewBindState;
		std::vector<bool> mUnorderAccessViewBindState;
		std::vector<bool> mSamplerBindState;
	};

	class FCopyCommandContextBase : public FCommandContext
	{
	public:
		using FCommandContext::FCommandContext;

		void TransitionBarrier(FGpuResourceRef resource, EResourceState newState, UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flushImmediate = false);
		void UAVBarrier(FGpuResourceRef resource, bool flushImmediate = false);
		void AliasingBarrier(FGpuResourceRef resourceBefore, FGpuResourceRef resourceAfter, bool flushImmediate = false);
		void FlushResourceBarriers();

		static void InitializeBuffer(FGpuBufferRef dest, const void* bufferData, size_t numBytes, size_t offset = 0);
		static void UpdateTextureBuffer(FTextureBufferRef dest, uint32_t firstSubresource, uint32_t numSubresources, const FSubResourceData* subresourceData);

		// Flush existing commands to the GPU but keep the context alive
		uint64_t Flush(bool waitForCompletion = false);

		// Flush existing commands and release the current context
		uint64_t Finish(bool waitForCompletion = false);
	};

	class FComputeCommandContextBase : public FCopyCommandContextBase
	{
	public:
		using FCopyCommandContextBase::FCopyCommandContextBase;

		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap);
		void SetDescriptorHeaps(UINT count, D3D12_DESCRIPTOR_HEAP_TYPE types[], ID3D12DescriptorHeap* heaps[]);

		void SetComputePipelineState(FComputePSORef pso);
		
		//Set Root Parameters
		void SetRootConstantBufferView(const std::string& bufferName, size_t sizeInBytes, const void* constants);
		template<typename T>
		void SetRootConstantBufferView(const std::string& bufferName, const T& constants)
		{
			SetRootConstantBufferView(bufferName, sizeof(T), &constants);
		}

		// Set descriptor table parameters
		void SetShaderResourceView(const std::string& srvrName, FColorBufferRef buffer, EResourceState stateAfter = EResourceState::AnyShaderAccess, UINT firstSubResource = 0, UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		void SetShaderResourceView(const std::string& srvrName, FTextureBufferRef buffer, EResourceState stateAfter = EResourceState::AnyShaderAccess, UINT firstSubResource = 0, UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		void SetShaderResourceView(const std::string& srvrName, FStructuredBufferRef buffer, EResourceState stateAfter = EResourceState::AnyShaderAccess, UINT firstSubResource = 0, UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		void ClearUAV(FGpuBufferRef target);
		void ClearUAV(FColorBufferRef target);

		void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

	protected:

		void SetComputeRootSignature(FRootSignatureRef rootSignature);

		void SetRootConstantBufferView(UINT rootIndex, size_t sizeInBytes, const void* constants);

		void SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, FGpuResourceRef resource, const D3D12_CPU_DESCRIPTOR_HANDLE& srcDescriptors,
			EResourceState stateAfter = EResourceState::AnyShaderAccess, UINT firstSubResource = 0,
			UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		void SetUnorderAccessView(UINT rootIndex, UINT descriptorOffset, FColorBufferRef buffer,
			EResourceState stateAfter = EResourceState::UnorderedAccess, UINT firstSubResource = 0,
			UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		void SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, FDepthBufferRef buffer,
			EResourceState stateAfter = EResourceState::AnyShaderAccess, UINT firstSubResource = 0,
			UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
	};

	class FGraphicsCommandContextBase : public FComputeCommandContextBase
	{
	public:
		using FComputeCommandContextBase::FComputeCommandContextBase;

		void ClearColor(FColorBufferRef target, D3D12_RECT* rect = nullptr);
		void ClearColor(FColorBufferRef target, const FLinearColor& color, D3D12_RECT* rect = nullptr);
		void ClearDepth(FDepthBufferRef target);
		void ClearStencil(FDepthBufferRef target);
		void ClearDepthAndStencil(FDepthBufferRef target);

		void SetGraphicsPipelineState(FGraphicsPSORef pso);

		void SetRenderTargets(UINT numRTVs, FColorBufferRef* rtvs);
		void SetRenderTargets(UINT numRTVs, FColorBufferRef* rtvs, FDepthBufferRef depthBuffer);
		void SetRenderTarget(FColorBufferRef colorBuffer) { SetRenderTargets(1, &colorBuffer); }
		void SetRenderTarget(FColorBufferRef colorBuffer, FDepthBufferRef depthBuffer) { SetRenderTargets(1, &colorBuffer, depthBuffer); }
		void SetDepthStencilTarget(FDepthBufferRef depthBuffer) { SetRenderTargets(1, nullptr, depthBuffer); }

		void SetViewport(const FViewport& vp);
		void SetViewport(Scalar x, Scalar y, Scalar w, Scalar h, Scalar minDepth = 0.0f, Scalar maxDepth = 1.0f);
		void SetScissor(const D3D12_RECT& rect);
		void SetScissor(UINT left, UINT top, UINT right, UINT bottom);
		void SetViewportAndScissor(UINT x, UINT y, UINT width, UINT height);
		void SetStencilRef(UINT stencilRef);
		void SetBlendFactor(const FLinearColor& color);
		void SetPrimitiveTopology(EPrimitiveTopology primitiveTopology);

		void SetDynamicSampler(UINT rootIndex, UINT descriptorOffset, D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void SetDynamicSamplers(UINT rootIndex, UINT descriptorOffset, UINT count, D3D12_CPU_DESCRIPTOR_HANDLE handles[]);

		void SetIndexBuffer(FGpuIndexBufferRef indexBuffer);
		void SetVertexBuffer(UINT slot, FGpuVertexBufferRef vertexBuffer);
		void SetVertexBuffers(UINT startSlot, UINT count, const FGpuVertexBufferRef* vertexBuffer);

		void SetDynamicIndexBuffer(size_t indexCount, const uint16_t* data);
		void SetDynamicVertexBuffer(UINT slot, size_t vertexCount, size_t vertexStride, const void* data);

	protected:

		void SetGraphicsRootSignature(FRootSignatureRef rootSignature);
	};

	class FCopyCommandContext : public FCopyCommandContextBase
	{
	public:
		FCopyCommandContext()
			: FCopyCommandContextBase(D3D12_COMMAND_LIST_TYPE_COPY)
		{}
		virtual ~FCopyCommandContext() {}

		static FCopyCommandContext& Begin(const std::string& id = "");
	};

	class FComputeCommandContext : public FComputeCommandContextBase
	{
	public:
		FComputeCommandContext()
			: FComputeCommandContextBase(D3D12_COMMAND_LIST_TYPE_COMPUTE)
		{}
		virtual ~FComputeCommandContext() {}

		static FComputeCommandContext& Begin(const std::string& id = "");
	};

	class FGraphicsCommandContext : public FGraphicsCommandContextBase
	{
	public:
		FGraphicsCommandContext()
			: FGraphicsCommandContextBase(D3D12_COMMAND_LIST_TYPE_DIRECT)
		{}
		virtual ~FGraphicsCommandContext() {}

		static FGraphicsCommandContext& Begin(const std::string& id = "");

		void Draw(UINT vertexCount, UINT vertexStartOffset = 0);
		void DrawIndexed(UINT indexCount, UINT startIndexLocation = 0, UINT baseVertexLocation = 0);
		void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation = 0, UINT startInstanceLocation = 0);
		void DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation);
	};
}