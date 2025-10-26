#pragma once

#include "GraphicTypesFwd.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "TextureBuffer.h"
#include <dxgi1_6.h>
#include "RenderDevice.h"
#include "CommandContext.h"

namespace Dash
{
	#define SWAP_CHAIN_BUFFER_COUNT 3

	enum class ESwapChainBitDepth : uint8
	{
		_8 = 0,
		_10,
		_16,
		SwapChainBitDepthCount
	};

	class FSwapChain
	{
	public:
		FSwapChain(uint32 displayWdith, uint32 displayHeight, EResourceFormat colorBufferFormat =
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

		uint32 GetDisplayWidth() const;
		uint32 GetDisplayHeight() const;

		void Present();
		void ToggleFullscreenMode();

		UINT GetCurrentBackBufferIndex() const;

		void OnKeyPressed(FKeyEventArgs& args);
		void OnWindowResize(uint32 newWidth, uint32 newHeight);
		void OnWindowMoved(uint32 xPos, uint32 yPos);

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
		void CreateSwapChain(uint32 newWidth, uint32 newHeight);
		void CreateBuffers();
		void DestroyBuffers();
		void ForceRecreateBuffers(uint32 newWidth, uint32 newHeight);

		void CheckDisplayHDRSupport();
		void EnsureSwapChainColorSpace(ESwapChainBitDepth swapChainBitDepth, bool enableST2084);
		void SetHDRMetaData(float MaxOutputNits = 1000.0f, float MinOutputNits = 0.001f, float MaxCLL = 2000.0f, float MaxFALL = 500.0f);

		EResourceFormat BitDepthToSwapChainFormat(ESwapChainBitDepth bitDepth);

	protected:
		uint32 mDisplayWdith = 1080;
		uint32 mDisplayHeight = 720;
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
		uint64 mFenceValue[SWAP_CHAIN_BUFFER_COUNT];

		TRefCountPtr<IDXGIFactory4> mDxgiFactory;
		TRefCountPtr<IDXGISwapChain4> mSwapChain;

		UINT mCurrentBackBufferIndex = 0;
		ESwapChainBitDepth mCurrentSwapChainBitDepth = ESwapChainBitDepth::_8;
		DXGI_COLOR_SPACE_TYPE mCurrentSwapChainColorSpace;
	};
}