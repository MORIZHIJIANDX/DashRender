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
	class FByteAddressBuffer;
	class FGpuVertexBuffer;
	class FGpuIndexBuffer;
	class FGpuDynamicVertexBuffer;
	class FGpuDynamicIndexBuffer;
	class FReadbackBuffer;
	
	struct FShaderResource;
	class FShaderPass;
	class FShaderTechnique;

	// declaration types ref

	using FQueryHeapRef = TRefCountPtr<FQueryHeap>;

	using FGpuResourceRef = TRefCountPtr<FGpuResource>;
	using FColorBufferRef = TRefCountPtr<FColorBuffer>;
	using FDepthBufferRef = TRefCountPtr<FDepthBuffer>;
	using FTextureBufferRef = TRefCountPtr<FTextureBuffer>;
	using FGpuBufferRef = TRefCountPtr<FGpuBuffer>;
	using FGpuConstantBufferRef = TRefCountPtr<FGpuConstantBuffer>;
	using FStructuredBufferRef = TRefCountPtr<FStructuredBuffer>;
	using FByteAddressBufferRef = TRefCountPtr<FByteAddressBuffer>;
	using FGpuVertexBufferRef = TRefCountPtr<FGpuVertexBuffer>;
	using FGpuIndexBufferRef = TRefCountPtr<FGpuIndexBuffer>;
	using FGpuDynamicVertexBufferRef = TRefCountPtr<FGpuDynamicVertexBuffer>;
	using FGpuDynamicIndexBufferRef = TRefCountPtr<FGpuDynamicIndexBuffer>;
	using FReadbackBufferRef = TRefCountPtr<FReadbackBuffer>;

	using FShaderResourceRef = TRefCountPtr<FShaderResource>;
	using FShaderPassRef = TRefCountPtr<FShaderPass>;
	using FShaderTechniqueRef = TRefCountPtr<FShaderTechnique>;
}
