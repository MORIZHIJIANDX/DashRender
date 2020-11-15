#pragma once

#include "d3dx12.h"

namespace Dash
{
	// CommandAllocatorPool		����͹���command allocator
	// CommandQueue				����һ��Ӳ������( Direct, Compute or Copy )
	// CommandListManager		ӵ�� Direct, Compute �� Copy Queue, ���𴴽��µ�Command List
	// GpuResource
	class Graphics
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void OnRender();

		static ID3D12Device* Device;

		static ID3D12CommandQueue* CommandQueue;

		static constexpr int BackBufferCount = 3;
	};
}