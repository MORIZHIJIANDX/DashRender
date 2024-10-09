#pragma once

#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "TextureBuffer.h"
#include <dxgi1_6.h>
#include "RenderDevice.h"
#include "CommandContext.h"

namespace Dash
{
	#define SWAP_CHAIN_BUFFER_COUNT 3

	enum class ESwapChainBitDepth : uint8_t
	{
		_8 = 0,
		_10,
		_16,
		SwapChainBitDepthCount
	};

	class FSwapChain
	{
	public:
		FSwapChain(uint32_t displayWdith, uint32_t displayHeight, EResourceFormat colorBufferFormat =
			EResourceFormat::RGBA16_Float, EResourceFormat depthBufferFormat = EResourceFormat::Depth32_Float, ESwapChainBitDepth swapChainBitDepth = ESwapChainBitDepth::_8)
			: mDisplayWdith(displayWdith)
			, mDisplayHeight(displayHeight)
			, mColorBufferFormat(colorBufferFormat)
			, mDepthBufferFormat(depthBufferFormat)
			, mCurrentSwapChainBitDepth(swapChainBitDepth)
		{
			Initialize();
		}
		
		void SetVSyncEnable(bool enable);
		void SetDisplayRate(float displayRate);
		void SetWindowBounds(int left, int top, int right, int bottom);
		void SetBackBufferBitDepth(ESwapChainBitDepth bitDepth);

		FColorBufferRef GetColorBuffer();
		FDepthBufferRef GetDepthBuffer();
		FColorBufferRef GetCurrentBackBuffer();
		EResourceFormat GetBackBufferFormat() const;
		EResourceFormat GetColorBufferFormat() const;
		EResourceFormat GetDepthBufferFormat() const;

		uint32_t GetDisplayWidth() const;
		uint32_t GetDisplayHeight() const;

		void Present();
		void ToggleFullscreenMode();

		UINT GetCurrentBackBufferIndex() const;

		void OnKeyPressed(FKeyEventArgs& args);
		void OnWindowResize(uint32_t newWidth, uint32_t newHeight);
		void OnWindowMoved(uint32_t xPos, uint32_t yPos);

		void Destroy();
	
	public:

		static const float HDRMetaDataPool[4][4];

	protected:

		struct EDisplayChromaticities
		{
			float RedX;
			float RedY;
			float GreenX;
			float GreenY;
			float BlueX;
			float BlueY;
			float WhiteX;
			float WhiteY;
		};

	protected:

		void Initialize();
		void CreateDxgiFactory();
		void CreateSwapChain(uint32_t newWidth, uint32_t newHeight);
		void CreateBuffers();
		void DestroyBuffers();
		void ForceRecreateBuffers(uint32_t newWidth, uint32_t newHeight);

		void CheckDisplayHDRSupport();
		void EnsureSwapChainColorSpace(ESwapChainBitDepth swapChainBitDepth, bool enableST2084);
		void SetHDRMetaData(float MaxOutputNits = 1000.0f, float MinOutputNits = 0.001f, float MaxCLL = 2000.0f, float MaxFALL = 500.0f);

		EResourceFormat BitDepthToSwapChainFormat(ESwapChainBitDepth bitDepth);

	protected:
		uint32_t mDisplayWdith = 1080;
		uint32_t mDisplayHeight = 720;
		float mDisplayRate = 1.0f;
		EResourceFormat mColorBufferFormat;
		EResourceFormat mDepthBufferFormat;
		EResourceFormat mSwapChainFormat;
		bool mVSyncEnable = false;
		bool mHDRSupport = false;

		FKeyboardEventDelegate KeyboardPressDelegate;
		bool mFullScreenMode = false;
		UINT mWindowStyle = WS_OVERLAPPEDWINDOW;
		RECT mWindowRect;

		FColorBufferRef mColorBuffer = nullptr;
		FDepthBufferRef mDepthBuffer = nullptr;
		FColorBufferRef mSwapChainBuffer[SWAP_CHAIN_BUFFER_COUNT];
		uint64_t mFenceValue[SWAP_CHAIN_BUFFER_COUNT];

		Microsoft::WRL::ComPtr<IDXGIFactory4> mDxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;

		UINT mCurrentBackBufferIndex = 0;
		ESwapChainBitDepth mCurrentSwapChainBitDepth = ESwapChainBitDepth::_8;
		DXGI_COLOR_SPACE_TYPE mCurrentSwapChainColorSpace;
	};
}