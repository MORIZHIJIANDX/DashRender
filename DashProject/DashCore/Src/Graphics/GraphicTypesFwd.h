#pragma once

#include "Utility/RefCounting.h"

namespace Dash
{
	class FRootSignature;
	class FPipelineStateObject;
	class FGraphicsPSO;
	class FComputePSO;

	class FQueryHeap;

	class FD3D12Heap;
	class FD3D12Resource;

	class FGpuResource;
	class FColorBuffer;
	class FDepthBuffer;
	class FTextureBuffer;
	class FGpuBuffer;
	class FGpuConstantBuffer;
	class FStructuredBuffer;
	class FGpuVertexBuffer;
	class FGpuIndexBuffer;
	class FGpuDynamicVertexBuffer;
	class FGpuDynamicIndexBuffer;
	class FReadbackBuffer;
	
	struct FShaderResource;
	class FShaderPass;
	class FShaderTechnique;

	// declaration types ref

	using FQueryHeapRef = std::shared_ptr<FQueryHeap>;

	using FGpuResourceRef = std::shared_ptr<FGpuResource>;
	using FColorBufferRef = std::shared_ptr<FColorBuffer>;
	using FDepthBufferRef = std::shared_ptr<FDepthBuffer>;
	using FTextureBufferRef = std::shared_ptr<FTextureBuffer>;
	using FGpuBufferRef = std::shared_ptr<FGpuBuffer>;
	using FGpuConstantBufferRef = std::shared_ptr<FGpuConstantBuffer>;
	using FStructuredBufferRef = std::shared_ptr<FStructuredBuffer>;
	using FGpuVertexBufferRef = std::shared_ptr<FGpuVertexBuffer>;
	using FGpuIndexBufferRef = std::shared_ptr<FGpuIndexBuffer>;
	using FGpuDynamicVertexBufferRef = std::shared_ptr<FGpuDynamicVertexBuffer>;
	using FGpuDynamicIndexBufferRef = std::shared_ptr<FGpuDynamicIndexBuffer>;
	using FReadbackBufferRef = std::shared_ptr<FReadbackBuffer>;

	using FShaderResourceRef = std::shared_ptr<FShaderResource>;
	using FShaderPassRef = std::shared_ptr<FShaderPass>;
	using FShaderTechniqueRef = std::shared_ptr<FShaderTechnique>;
}
