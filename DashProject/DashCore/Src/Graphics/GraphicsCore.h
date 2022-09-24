#pragma once

#include <d3d12.h>

namespace Dash
{	
	class CommandQueueManager;

	class Graphics
	{
	public:
		static void Initialize();
		static void Shutdown();

		static ID3D12Device* Device;
		static CommandQueueManager* QueueManager;

		static constexpr int BackBufferCount = 3;
	private:
		static bool mTypedUAVLoadSupport_R11G11B10_FLOAT;
		static bool mTypedUAVLoadSupport_R16G16B16A16_FLOAT;
		static D3D_ROOT_SIGNATURE_VERSION mHighestRootSignatureVersion;
	};
}