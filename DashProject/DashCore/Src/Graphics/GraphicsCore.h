#pragma once

#include <d3d12.h>

namespace Dash
{	
	class FCommandQueueManager;
	class FCommandListManager;
	class FCpuDescriptorAllocatorManager;
	class FCommandContextManager;
	class FSwapChain;
	class FRenderDevice;

	class FGraphicsCore
	{
	public:
		static void Initialize();
		static void Shutdown();

		static D3D_ROOT_SIGNATURE_VERSION GetRootSignatureVersion();

		static FRenderDevice* Device;
		static FCommandQueueManager* CommandQueueManager;
		static FCommandListManager* CommandListManager;
		static FCpuDescriptorAllocatorManager* DescriptorAllocator;
		static FCommandContextManager* ContextManager;
		static FSwapChain* SwapChain;

		static constexpr int BackBufferCount = 3;

	private:
		//static void InitD3DDevice();
		//static void DestroyD3Device();

	private:
		static bool mTypedUAVLoadSupport_R11G11B10_FLOAT;
		static bool mTypedUAVLoadSupport_R16G16B16A16_FLOAT;
		static bool mSupportsUniversalHeaps;
		static D3D_ROOT_SIGNATURE_VERSION mHighestRootSignatureVersion;
	};
}