#pragma once

#include "ColorBuffer.h"

namespace Dash
{
	#define SWAP_CHAIN_BUFFER_COUNT 3

	class IDXGISwapChain1;

	class FDisplay
	{
	public:
		FDisplay(uint32_t displayWdith, uint32_t displayHeight, DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R10G10B10A2_UNORM)
			: mDisplayWdith(displayWdith)
			, mDisplayHeight(displayHeight)
			, mSwapChainFormat(swapChainFormat)
		{}
		
		void Initialize();
		void Destroy();
		void Resize(uint32_t displayWdith, uint32_t displayHeight);
		void Present();
		FColorBuffer& GetDisplayBuffer();
	
	protected:
		void CreateSwapChain();

	protected:
		uint32_t mDisplayWdith;
		uint32_t mDisplayHeight;
		DXGI_FORMAT mSwapChainFormat;

		FColorBuffer mDisplayBuffer;
		FColorBuffer mSwapChainBuffer[SWAP_CHAIN_BUFFER_COUNT];

		Microsoft::WRL::ComPtr<IDXGISwapChain1> mSwapChain;
	};
}