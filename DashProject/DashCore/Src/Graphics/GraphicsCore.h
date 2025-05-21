#pragma once

namespace Dash
{	
	class FCommandQueueManager;
	class FCommandListManager;
	class FCpuDescriptorAllocatorManager;
	class FCommandContextManager;
	class FSwapChain;
	class FRenderDevice;
	class FGPUProfiler;

	class FGraphicsCore
	{
	public:
		static void Initialize(uint32 windowWidth, uint32 windowHeight);
		static void Shutdown();

		static D3D_ROOT_SIGNATURE_VERSION GetRootSignatureVersion();

		static FRenderDevice* Device;
		static FCommandQueueManager* CommandQueueManager;
		static FCommandListManager* CommandListManager;
		static FCpuDescriptorAllocatorManager* DescriptorAllocator;
		static FCommandContextManager* ContextManager;
		static FSwapChain* SwapChain;
		static FGPUProfiler* Profiler;

		static constexpr int BackBufferCount = 3;

	private:
		static void EnablePixCapture();

	private:
		static bool mTypedUAVLoadSupport_R11G11B10_FLOAT;
		static bool mTypedUAVLoadSupport_R16G16B16A16_FLOAT;
		static bool mSupportsUniversalHeaps;
		static D3D_ROOT_SIGNATURE_VERSION mHighestRootSignatureVersion;
	};
}