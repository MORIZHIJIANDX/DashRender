#pragma once

#include "d3dx12.h"

namespace Dash
{
	// CommandAllocatorPool		分配和管理command allocator
	// CommandQueue				代表一个硬件队列( Direct, Compute or Copy )
	// CommandListManager		拥有 Direct, Compute 和 Copy Queue, 负责创建新的Command List
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