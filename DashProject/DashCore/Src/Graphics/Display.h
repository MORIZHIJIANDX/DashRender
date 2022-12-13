#pragma once

#include "ColorBuffer.h"
#include <dxgi1_6.h>

namespace Dash
{
	#define SWAP_CHAIN_BUFFER_COUNT 3

	class FDisplay
	{
	public:
		FDisplay(uint32_t displayWdith, uint32_t displayHeight, EResourceFormat swapChainFormat = EResourceFormat::RGB10A2_Unorm)
			: mDisplayWdith(displayWdith)
			, mDisplayHeight(displayHeight)
			, mSwapChainFormat(swapChainFormat)
		{
			Initialize();
		}
			
		void Destroy();
		void SetDisplayRate(float displayRate);
		void OnWindowResize(uint32_t newWidth, uint32_t newHeight);
		void Present();
		FColorBuffer& GetDisplayBuffer();
	
	protected:
		void Initialize();
		void CreateSwapChain(uint32_t newWidth, uint32_t newHeight);
		void CreateBuffers();
		void DestroyBuffers();
		void ForceRecreateBuffers(uint32_t newWidth, uint32_t newHeight);

	protected:
		uint32_t mDisplayWdith = 1080;
		uint32_t mDisplayHeight = 720;
		float mDisplayRate = 1.0f;
		EResourceFormat mSwapChainFormat;

		FColorBuffer mDisplayBuffer{FLinearColor::Yellow};
		FColorBuffer mSwapChainBuffer[SWAP_CHAIN_BUFFER_COUNT];
		uint64_t mFenceValue[SWAP_CHAIN_BUFFER_COUNT];

		Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;

		UINT mCurrentBackBufferIndex = 0;
	};
}