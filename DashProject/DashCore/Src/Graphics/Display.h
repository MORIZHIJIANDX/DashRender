#pragma once

#include "ColorBuffer.h"
#include <dxgi1_6.h>

namespace Dash
{
	#define SWAP_CHAIN_BUFFER_COUNT 3

	class FDisplay
	{
	public:
		FDisplay(uint32_t displayWdith, uint32_t displayHeight, EColorFormat swapChainFormat = EColorFormat::RGB10A2_Unorm)
			: mDisplayWdith(displayWdith)
			, mDisplayHeight(displayHeight)
			, mSwapChainFormat(swapChainFormat)
		{
			Initialize();
		}
			
		void Destroy();
		void Resize(uint32_t displayWdith, uint32_t displayHeight);
		void Present();
		FColorBuffer& GetDisplayBuffer();
	
	protected:
		void Initialize();
		void CreateSwapChain(uint32_t displayWdith, uint32_t displayHeight);
		void CreateBuffers();
		void DestroyBuffers();

	protected:
		uint32_t mDisplayWdith;
		uint32_t mDisplayHeight;
		EColorFormat mSwapChainFormat;

		FColorBuffer mDisplayBuffer{FLinearColor::Yellow};
		FColorBuffer mSwapChainBuffer[SWAP_CHAIN_BUFFER_COUNT];
		uint64_t mFenceValue[SWAP_CHAIN_BUFFER_COUNT];

		Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;

		UINT mCurrentBackBufferIndex = 0;
	};
}