#pragma once

#include "ColorBuffer.h"
#include "TextureBuffer.h"
#include <dxgi1_6.h>
#include "RenderDevice.h"

namespace Dash
{
	#define SWAP_CHAIN_BUFFER_COUNT 3

	class FSwapChain
	{
	public:
		FSwapChain(uint32_t displayWdith, uint32_t displayHeight, EResourceFormat swapChainFormat = EResourceFormat::RGB10A2_Unorm)
			: mDisplayWdith(displayWdith)
			, mDisplayHeight(displayHeight)
			, mSwapChainFormat(swapChainFormat)
		{
			Initialize();
		}
		
		void SetVSyncEnable(bool enable);
		void Destroy();
		void SetDisplayRate(float displayRate);
		void OnWindowResize(uint32_t newWidth, uint32_t newHeight);
		void Present();
		FColorBufferRef GetDisplayBuffer();
		FColorBufferRef GetCurrentBackBuffer();
		EResourceFormat GetBackBufferFormat() const;
	
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
		bool mVSyncEnable = true;

		FColorBufferRef mDisplayBuffer = nullptr;
		FColorBufferRef mSwapChainBuffer[SWAP_CHAIN_BUFFER_COUNT];
		uint64_t mFenceValue[SWAP_CHAIN_BUFFER_COUNT];

		Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;

		UINT mCurrentBackBufferIndex = 0;

		FTextureBufferRef mTexture;
	};
}