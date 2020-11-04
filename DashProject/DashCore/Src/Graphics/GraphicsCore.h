#pragma once

#include "d3dx12.h"

namespace Dash
{
	class Graphics
	{
	public:
		static void Initialize();
		static void Shutdown();

		static ID3D12Device* Device;

		static ID3D12CommandQueue* CommandQueue;

		static constexpr int BackBufferCount = 3;
	};
}