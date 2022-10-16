#pragma once

#include "CommandQueue.h"
#include "ColorBuffer.h"
#include "DynamicDescriptorHeap.h"
#include "PipelineStateObject.h"
#include "DepthBuffer.h"
#include "Viewport.h"
#include "GpuBuffer.h"

namespace Dash
{
	class FCommandContext;
	class FGraphicsCommandContext;
	class FComputeCommandContext;

	class FCommandContextManager
	{
	public:
		FCommandContextManager();

		FCommandContext* AllocateContext(D3D12_COMMAND_LIST_TYPE type);
		void FreeContext(FCommandContext* context);
		void Destroy();

	private:
		std::vector<std::unique_ptr<FCommandContext>> mContextPool[4];
		std::queue<FCommandContext*> mAvailableContexts[4];
		std::mutex mAllocationMutex;
	};

	class FCommandContext
	{
		friend FCommandContextManager;
	private:
		
		FCommandContext(D3D12_COMMAND_LIST_TYPE type);

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

		// Flush existing commands to the GPU but keep the context alive
		uint64_t Flush(bool waitForCompletion = false);

		// Flush existing commands and release the current context
		uint64_t Finish(bool waitForCompletion = false);

		// Prepare to render by reserving a command list and command allocator
		void Initialize();

		FGraphicsCommandContext& GetGraphicsCommandContext();
		FComputeCommandContext& GetComputeCommandContext();

		FCommandList* GetCommandList();
		ID3D12GraphicsCommandList* GetD3DCommandList();

		void TransitionBarrier(FGpuResource& resource, D3D12_RESOURCE_STATES newState, bool flushImmediate = false);
		void UAVBarrier(FGpuResource& resource, bool flushImmediate = false);
		void AliasingBarrier(FGpuResource& resource, bool flushImmediate = false);
		void FlushResourceBarriers();

		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap);
		void SetDescriptorHeaps(UINT count, D3D12_DESCRIPTOR_HEAP_TYPE types[], ID3D12DescriptorHeap* heaps[]);

		void SetPipelineState(const FPipelineStateObject& pso);

		static void InitializeBuffer(FGpuBuffer& dest, const void* bufferData, size_t numBytes, size_t offset = 0);

	protected:

		void BindDescriptorHeaps();

		FCommandList* mCommandList;
		ID3D12CommandList* mD3DCommandList;

		std::shared_ptr<FGpuResourcesStateTracker> mResourceStateTracker;

		// Keep track of the currently bound descriptor heaps. Only change descriptor
		// heaps if they are different than the currently bound descriptor heaps.
		ID3D12DescriptorHeap* mDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		FDynamicDescriptorHeap mDynamicViewDescriptor;
		FDynamicDescriptorHeap mDynamicSamplerDescriptor;

		using FTrackedObjects = std::vector<Microsoft::WRL::ComPtr<ID3D12Object>>;
		FTrackedObjects mTrackedObjects;

		std::wstring mID;
		D3D12_COMMAND_LIST_TYPE mType;
	};

	class FGraphicsCommandContext
	{
	public:
		void ClearUAV(FGpuResource& target);
		void ClearUAV(FColorBuffer& target);
		void ClearColor(FColorBuffer& target, D3D12_RECT* rect = nullptr);
		void ClearColor(FColorBuffer& target, const FLinearColor& color, D3D12_RECT* rect = nullptr);
		void ClearDepth(FDepthBuffer& Target);
		void ClearStencil(FDepthBuffer& target);
		void ClearDepthAndStencil(FDepthBuffer& target);

		void SetRootSignature(const FRootSignature& rootSignature);

		void SetRenderTargets(UINT numRTVs, const FColorBuffer* rtvs);
		void SetRenderTargets(UINT numRTVs, const FColorBuffer* rtvs, const FDepthBuffer& depthBuffer);
		void SetRenderTarget(const FColorBuffer& colorBuffer) { SetRenderTargets(1, &colorBuffer); }
		void SetRenderTarget(const FColorBuffer& colorBuffer, const FDepthBuffer& depthBuffer) { SetRenderTargets(1, &colorBuffer, depthBuffer); }
		void SetDepthStencilTarget(const FDepthBuffer& depthBuffer) { SetRenderTargets(1, nullptr, depthBuffer); }

		void SetViewport(const FViewport& vp);
		void SetViewport(Scalar x, Scalar y, Scalar w, Scalar h, Scalar minDepth = 0.0f, Scalar maxDepth = 0.0f);
		void SetScissor(const D3D12_RECT& rect);
		void SetScissor(UINT left, UINT top, UINT right, UINT bottom);
		void SetStencilRef(UINT stencilRef);
		void SetBlendFactor(const FLinearColor& color);
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);

		//Set Root Parameters

		void Set32BitConstants(UINT rootIndex, UINT numConstants, const void* constants);
		template<typename T>
		void Set32BitConstants(UINT rootIndex, const T& constants)
		{
			STATIC_ASSERT_MSG(sizeof(T) % sizeof(uint32_t) == 0, "Size of type must be a multiple of 4 bytes");
			Set32BitConstants(rootIndex, sizeof(T) / sizeof(uint32_t), &constants);
		}

		void SetRootConstantBufferView(UINT rootIndex, size_t sizeInBytes, const void* constants);
		template<typename T>
		void SetRootConstantBufferView(UINT rootIndex, const T& constants)
		{
			SetRootConstantBufferView(rootIndex, sizeof(T), &constants);
		}

		void SetRootShaderResourceView(UINT rootIndex, size_t sizeInBytes, const void* constants);
		template<typename T>
		void SetRootShaderResourceView(UINT rootIndex, const T& constants)
		{
			SetRootShaderResourceView(rootIndex, sizeof(T), &constants);
		}

		// FGpuConstantBuffer 初始状态 D3D12_RESOURCE_STATE_COMMON 不可直接作为 SRV 和 UAV，需要转换为 D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER (D3D12_RESOURCE_STATE_GENERIC_READ) 状态
		void SetRootConstantBufferView(UINT rootIndex, const FGpuConstantBuffer& constantBuffer, size_t bufferOffset = 0, 
			D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		void SetRootShaderResourceView(UINT rootIndex, const FGpuConstantBuffer& constantBuffer, size_t bufferOffset = 0,
			D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		void SetRootUnorderAccessView(UINT rootIndex, const FGpuConstantBuffer& constantBuffer, size_t bufferOffset = 0,
			D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


		// Set descriptor table parameters

		void SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, const FGpuConstantBuffer& buffer,
			D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		void SetUnorderAccessView(UINT rootIndex, UINT descriptorOffset, const FGpuConstantBuffer& buffer,
			D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		void SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, const FColorBuffer& buffer,
			D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, UINT firstSubResource = 0, 
			UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		void SetUnorderAccessView(UINT rootIndex, UINT descriptorOffset, const FColorBuffer& buffer,
			D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS, UINT firstSubResource = 0,
			UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		void SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, const FDepthBuffer& buffer,
			D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, UINT firstSubResource = 0,
			UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		void SetDynamicSampler(UINT rootIndex, UINT descriptorOffset, D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void SetDynamicSampler(UINT rootIndex, UINT descriptorOffset, UINT count, D3D12_CPU_DESCRIPTOR_HANDLE handles[]);

		void SetIndexBuffer(const FGpuIndexBuffer& indexBuffer);
		void SetVertexBuffer(UINT slot, const FGpuVertexBuffer& vertexBuffer);
		void SetVertexBuffers(UINT startSlot, UINT count, const FGpuVertexBuffer* vertexBuffer);

		void SetDynamicIndexBuffer(size_t indexCount, const uint16_t* data);
		void SetDynamicVertexBuffer(UINT slot, size_t vertexCount, size_t vertexStride, const void* data);

		void Draw(UINT vertexCount, UINT vertexStartOffset = 0);
		void DrawIndexed(UINT indexCount, UINT startIndexLocation = 0, UINT baseVertexLocation = 0);
		void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount,
			UINT startVertexLocation = 0, UINT startInstanceLocation = 0);
		void DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation,
			INT baseVertexLocation, UINT startInstanceLocation);
	};
}