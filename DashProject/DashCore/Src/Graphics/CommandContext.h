#pragma once

#include "CommandQueue.h"
#include "ColorBuffer.h"
#include "DynamicDescriptorHeap.h"
#include "PipelineStateObject.h"
#include "DepthBuffer.h"
#include "Viewport.h"
#include "GpuBuffer.h"
#include "GpuLinearAllocator.h"
#include "ShaderPass.h"

namespace Dash
{
	class FCommandContext;
	class FGraphicsCommandContext;
	class FComputeCommandContext;

	class FCommandContextManager
	{
	public:
		FCommandContextManager(){};

		FCommandContext* AllocateContext(D3D12_COMMAND_LIST_TYPE type);
		void FreeContext(uint64_t fenceValue, FCommandContext* context);
		void ReleaseAllTrackObjects();
		void Destroy();

	private:
		std::vector<std::unique_ptr<FCommandContext>> mContextPool[4];
		std::queue<FCommandContext*> mAvailableContexts[4];
		std::queue<std::pair<uint64_t, FCommandContext*>> mRetiredContexts[4];
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

		static FCommandContext& Begin(const std::string& id = "", D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

		// Flush existing commands to the GPU but keep the context alive
		uint64_t Flush(bool waitForCompletion = false);

		// Flush existing commands and release the current context
		uint64_t Finish(bool waitForCompletion = false);

		FGraphicsCommandContext& GetGraphicsCommandContext();
		//FComputeCommandContext& GetComputeCommandContext();

		FCommandList* GetCommandList() { return mCommandList; }
		ID3D12GraphicsCommandList* GetD3DCommandList() { return mD3DCommandList; }

		void TransitionBarrier(FGpuResource& resource, EResourceState newState, UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flushImmediate = false);
		void UAVBarrier(FGpuResource& resource, bool flushImmediate = false);
		void AliasingBarrier(FGpuResource& resourceBefore, FGpuResource& resourceAfter, bool flushImmediate = false);
		void FlushResourceBarriers();

		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap);
		void SetDescriptorHeaps(UINT count, D3D12_DESCRIPTOR_HEAP_TYPE types[], ID3D12DescriptorHeap* heaps[]);

		void SetPipelineState(const FPipelineStateObject& pso);
		virtual void SetRootSignature(const FRootSignature& rootSignature) = 0;

		static void InitializeBuffer(FGpuBuffer& dest, const void* bufferData, size_t numBytes, size_t offset = 0);

	protected:

		void SetID(const std::string& id) { mID = id; }
		void BindDescriptorHeaps();
		void TrackResource(FGpuResource& resource);
		void ReleaseTrackedObjects();
		uint64_t Execute();

		void InitParameterBindState();
		void CheckUnboundShaderParameters();

	protected:
		std::string mID;
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

		using FTrackedObjects = std::vector<Microsoft::WRL::ComPtr<ID3D12Object>>;
		FTrackedObjects mTrackedObjects;

		ID3D12RootSignature* mCurrentRootSignature = nullptr;
		ID3D12PipelineState* mCurrentPipelineState = nullptr;

		const FRootSignature* mRootSignature = nullptr;
		const FPipelineStateObject* mPSO = nullptr;

		std::map<std::string, bool> mConstantBufferBindState;
		std::map<std::string, bool> mShaderResourceViewBindState;
		std::map<std::string, bool> mUnorderAccessViewBindState;
		std::map<std::string, bool> mSamplerBindState;
	};

	class FGraphicsCommandContext : public FCommandContext
	{
	public:
		FGraphicsCommandContext()
			: FCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT)
		{}

		static FGraphicsCommandContext& Begin(const std::string& id = "")
		{
			return FCommandContext::Begin(id, D3D12_COMMAND_LIST_TYPE_DIRECT).GetGraphicsCommandContext();
		}

		void ClearUAV(FGpuConstantBuffer& target);
		void ClearUAV(FColorBuffer& target);
		void ClearColor(FColorBuffer& target, D3D12_RECT* rect = nullptr);
		void ClearColor(FColorBuffer& target, const FLinearColor& color, D3D12_RECT* rect = nullptr);
		void ClearDepth(FDepthBuffer& target);
		void ClearStencil(FDepthBuffer& target);
		void ClearDepthAndStencil(FDepthBuffer& target);

		virtual void SetRootSignature(const FRootSignature& rootSignature) override;

		void SetRenderTargets(UINT numRTVs, FColorBuffer* rtvs);
		void SetRenderTargets(UINT numRTVs, FColorBuffer* rtvs, FDepthBuffer& depthBuffer);
		void SetRenderTarget(FColorBuffer& colorBuffer) { SetRenderTargets(1, &colorBuffer); }
		void SetRenderTarget(FColorBuffer& colorBuffer, FDepthBuffer& depthBuffer) { SetRenderTargets(1, &colorBuffer, depthBuffer); }
		void SetDepthStencilTarget(FDepthBuffer& depthBuffer) { SetRenderTargets(1, nullptr, depthBuffer); }

		void SetViewport(const FViewport& vp);
		void SetViewport(Scalar x, Scalar y, Scalar w, Scalar h, Scalar minDepth = 0.0f, Scalar maxDepth = 1.0f);
		void SetScissor(const D3D12_RECT& rect);
		void SetScissor(UINT left, UINT top, UINT right, UINT bottom);
		void SetViewportAndScissor(UINT x, UINT y, UINT width, UINT height);
		void SetStencilRef(UINT stencilRef);
		void SetBlendFactor(const FLinearColor& color);
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY type);

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

		void SetRootConstantBufferView(const std::string& bufferName, size_t sizeInBytes, const void* constants);
		template<typename T>
		void SetRootConstantBufferView(const std::string& bufferName, const T& constants)
		{
			SetRootConstantBufferView(bufferName, sizeof(T), &constants);
		}

		// FGpuConstantBuffer 初始状态 D3D12_RESOURCE_STATE_COMMON 不可直接作为 SRV 和 UAV，需要转换为 D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER (D3D12_RESOURCE_STATE_GENERIC_READ) 状态
		void SetRootConstantBufferView(UINT rootIndex, FGpuConstantBuffer& constantBuffer, size_t bufferOffset = 0, 
			EResourceState stateAfter = EResourceState::ConstantBuffer);

		void SetRootShaderResourceView(UINT rootIndex, FGpuConstantBuffer& constantBuffer, size_t bufferOffset = 0,
			EResourceState stateAfter = EResourceState::AnyShaderAccess);

		void SetRootUnorderAccessView(UINT rootIndex, FGpuConstantBuffer& constantBuffer, size_t bufferOffset = 0,
			EResourceState stateAfter = EResourceState::UnorderedAccess);

		void SetRootConstantBufferView(const std::string& bufferName, FGpuConstantBuffer& constantBuffer, EResourceState stateAfter = EResourceState::ConstantBuffer);

		// Set descriptor table parameters

		void SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, FGpuConstantBuffer& buffer,
			EResourceState stateAfter = EResourceState::AnyShaderAccess);

		void SetUnorderAccessView(UINT rootIndex, UINT descriptorOffset, FGpuConstantBuffer& buffer,
			EResourceState stateAfter = EResourceState::UnorderedAccess);

		void SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, FColorBuffer& buffer,
			EResourceState stateAfter = EResourceState::AnyShaderAccess, UINT firstSubResource = 0,
			UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		void SetShaderResourceView(const std::string& srvrName, FColorBuffer& buffer,
			EResourceState stateAfter = EResourceState::AnyShaderAccess, UINT firstSubResource = 0,
			UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		void SetUnorderAccessView(UINT rootIndex, UINT descriptorOffset, FColorBuffer& buffer,
			EResourceState stateAfter = EResourceState::UnorderedAccess, UINT firstSubResource = 0,
			UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		void SetShaderResourceView(UINT rootIndex, UINT descriptorOffset, FDepthBuffer& buffer,
			EResourceState stateAfter = EResourceState::AnyShaderAccess, UINT firstSubResource = 0,
			UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		void SetDynamicSampler(UINT rootIndex, UINT descriptorOffset, D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void SetDynamicSamplers(UINT rootIndex, UINT descriptorOffset, UINT count, D3D12_CPU_DESCRIPTOR_HANDLE handles[]);

		void SetIndexBuffer(FGpuIndexBuffer& indexBuffer);
		void SetVertexBuffer(UINT slot, FGpuVertexBuffer& vertexBuffer);
		void SetVertexBuffers(UINT startSlot, UINT count, FGpuVertexBuffer* vertexBuffer);

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