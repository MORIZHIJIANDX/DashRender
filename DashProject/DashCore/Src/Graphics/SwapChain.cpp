#include "PCH.h"
#include "SwapChain.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"
#include "Framework/GameApp.h"
#include "CommandQueue.h"
#include "CommandContext.h"
#include "ShaderResource.h"
#include "ShaderMap.h"
#include "PipelineStateObject.h"
#include "RootSignature.h"
#include "GpuBuffer.h"
#include "ShaderPass.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "PrimitiveTopology.h"
#include "Utility/FileUtility.h"
#include "CpuDescriptorAllocator.h"

#include "Utility/Keyboard.h"

namespace Dash
{
	using namespace Microsoft::WRL;

	const float FSwapChain::HDRMetaDataPool[4][4] =
	{
		// MaxOutputNits, MinOutputNits, MaxCLL, MaxFALL
		// These values are made up for testing. You need to figure out those numbers for your app.
		{ 1000.0f, 0.001f, 2000.0f, 500.0f },
		{ 500.0f, 0.001f, 2000.0f, 500.0f },
		{ 500.0f, 0.100f, 500.0f, 100.0f },
		{ 2000.0f, 1.000f, 2000.0f, 1000.0f }
	};

	void FSwapChain::Initialize()
	{
		mDisplayWdith = IGameApp::GetInstance()->GetWindowWidth();
		mDisplayHeight = IGameApp::GetInstance()->GetWindowHeight();
		mSwapChainFormat = BitDepthToSwapChainFormat(mCurrentSwapChainBitDepth);

		CreateSwapChain(IGameApp::GetInstance()->GetWindowWidth(), IGameApp::GetInstance()->GetWindowHeight());
		CreateBuffers();

		for (uint32_t index = 0; index < SWAP_CHAIN_BUFFER_COUNT; ++index)
		{
			mFenceValue[index] = ((uint64_t)D3D12_COMMAND_LIST_TYPE_DIRECT) << COMMAND_TYPE_MASK;
		}

		KeyboardPressDelegate = FKeyboardEventDelegate::Create<FSwapChain, &FSwapChain::OnKeyPressed>(this);

		FKeyboard::Get().KeyPressed += KeyboardPressDelegate;
	}

	void FSwapChain::SetVSyncEnable(bool enable)
	{
		mVSyncEnable = enable;
	}

	void FSwapChain::Destroy()
	{
		FKeyboard::Get().KeyPressed -= KeyboardPressDelegate;

		if (!FGraphicsCore::Device->SupportsTearing())
		{
			mSwapChain->SetFullscreenState(FALSE, nullptr);
		}
		
		DestroyBuffers();

		mSwapChain = nullptr;
	}

	void FSwapChain::SetDisplayRate(float displayRate)
	{
		if (mDisplayRate != displayRate)
		{
			mDisplayRate = displayRate;
			ForceRecreateBuffers(mSwapChainBuffer[0]->GetWidth(), mSwapChainBuffer[0]->GetHeight());
			LOG_INFO << "Set Display Rate : " << displayRate;
		}
	}

	void FSwapChain::OnWindowResize(uint32_t displayWdith, uint32_t displayHeight)
	{
		if (mSwapChainBuffer[mCurrentBackBufferIndex]->GetWidth() != displayWdith || mSwapChainBuffer[mCurrentBackBufferIndex]->GetHeight() != displayHeight)
		{
			ForceRecreateBuffers(displayWdith, displayHeight);
		}
	}

	void FSwapChain::OnWindowMoved(uint32_t xPos, uint32_t yPos)
	{
		CheckDisplayHDRSupport();
	}

	void FSwapChain::SetWindowBounds(int left, int top, int right, int bottom)
	{
		mWindowRect.left = static_cast<LONG>(left);
		mWindowRect.top = static_cast<LONG>(top);
		mWindowRect.right = static_cast<LONG>(right);
		mWindowRect.bottom = static_cast<LONG>(bottom);
	}

	void FSwapChain::SetBackBufferBitDepth(ESwapChainBitDepth bitDepth)
	{
		mCurrentSwapChainBitDepth = bitDepth;
		mSwapChainFormat = BitDepthToSwapChainFormat(mCurrentSwapChainBitDepth);

		ForceRecreateBuffers(mSwapChainBuffer[0]->GetWidth(), mSwapChainBuffer[0]->GetHeight());
	}

	void FSwapChain::Present()
	{
		FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("Present");

		graphicsContext.TransitionBarrier(FGraphicsCore::SwapChain->GetCurrentBackBuffer(), EResourceState::Present);

		graphicsContext.Finish();
		
		UINT presentInterval = mVSyncEnable ? 1 : 0;
		UINT presentFlags = (FGraphicsCore::Device->SupportsTearing() && !mFullScreenMode) ? DXGI_PRESENT_ALLOW_TEARING : 0;

		mSwapChain->Present(presentInterval, presentFlags);

		mFenceValue[mCurrentBackBufferIndex] = FGraphicsCore::CommandQueueManager->GetGraphicsQueue().Signal();

		mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
		uint64_t nextBufferFenceValue = mFenceValue[mCurrentBackBufferIndex];
		FGraphicsCore::CommandQueueManager->GetGraphicsQueue().WaitForFence(nextBufferFenceValue);	

		FGraphicsCore::DescriptorAllocator->ReleaseStaleDescriptors();
	}

	FColorBufferRef FSwapChain::GetColorBuffer()
	{
		return mColorBuffer;
	}

	FDepthBufferRef FSwapChain::GetDepthBuffer()
	{
		return mDepthBuffer;
	}

	void FSwapChain::CreateDxgiFactory()
	{
		UINT dxgiFactoryFlags = 0;
#if DASH_DEBUG
		ComPtr<ID3D12Debug> DebugInterface;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugInterface))))
		{
			DebugInterface->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif // DASH_DEBUG

		DX_CALL(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mDxgiFactory)));
	}

	void FSwapChain::CreateSwapChain(uint32_t displayWdith, uint32_t displayHeight)
	{
		ASSERT_MSG(mSwapChain == nullptr, "Swap Chain Already Initialized");
		
		CreateDxgiFactory();

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.Width = displayWdith;
		swapChainDesc.Height = displayHeight;
		swapChainDesc.Format = D3DFormat(mSwapChainFormat);
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
		swapChainDesc.Flags = FGraphicsCore::Device->SupportsTearing() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING  : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc{};
		fullScreenDesc.Windowed = TRUE;

		ComPtr<IDXGISwapChain1> dxgiSwapChain1;
		DX_CALL(mDxgiFactory->CreateSwapChainForHwnd(FGraphicsCore::CommandQueueManager->GetGraphicsQueue().GetD3DCommandQueue(),
			IGameApp::GetInstance()->GetWindowHandle(),
			&swapChainDesc,
			&fullScreenDesc,
			nullptr,
			&dxgiSwapChain1));

		if (FGraphicsCore::Device->SupportsTearing())
		{
			mDxgiFactory->MakeWindowAssociation(IGameApp::GetInstance()->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);
		}

		DX_CALL(dxgiSwapChain1.As(&mSwapChain));

		CheckDisplayHDRSupport();
		EnsureSwapChainColorSpace(mCurrentSwapChainBitDepth, mHDRSupport);
		int32_t HDRMetadataPoolIndex = 0;
		SetHDRMetaData(HDRMetaDataPool[HDRMetadataPoolIndex][0], HDRMetaDataPool[HDRMetadataPoolIndex][1], HDRMetaDataPool[HDRMetadataPoolIndex][2], HDRMetaDataPool[HDRMetadataPoolIndex][3]);
	}

	void FSwapChain::CreateBuffers()
	{
		for (uint32_t index = 0; index < SWAP_CHAIN_BUFFER_COUNT; ++index)
		{
			ComPtr<ID3D12Resource> backBuffer;
			DX_CALL(mSwapChain->GetBuffer(index, IID_PPV_ARGS(&backBuffer)));
			mSwapChainBuffer[index] = FGraphicsCore::Device->CreateColorBuffer("Swap Chain Buffer[" + FStringUtility::ToString(index) + "]", backBuffer.Detach(), EResourceState::Common); // D3D12_RESOURCE_STATE_PRESENT ?
		}

		mDisplayWdith = static_cast<uint32_t>(FMath::AlignUp(mSwapChainBuffer[0]->GetWidth() * mDisplayRate, 2));
		mDisplayHeight = static_cast<uint32_t>(FMath::AlignUp(mSwapChainBuffer[0]->GetHeight() * mDisplayRate, 2));
		mColorBuffer = FGraphicsCore::Device->CreateColorBuffer("Color Buffer", mDisplayWdith, mDisplayHeight, 1, mColorBufferFormat);
		mDepthBuffer = FGraphicsCore::Device->CreateDepthBuffer("Depth Buffer", mDisplayWdith, mDisplayHeight, mDepthBufferFormat);
		LOG_INFO << "Set Display Width : " << mDisplayWdith << ", Display Height : " << mDisplayHeight;

		mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	}

	void FSwapChain::DestroyBuffers()
	{
		for (uint32_t index = 0; index < SWAP_CHAIN_BUFFER_COUNT; ++index)
		{
			mSwapChainBuffer[index]->Destroy();
			mSwapChainBuffer[index] = nullptr;
		}

		mColorBuffer->Destroy();
		mColorBuffer = nullptr;

		mDepthBuffer->Destroy();
		mDepthBuffer = nullptr;
	}

	void FSwapChain::ForceRecreateBuffers(uint32_t newWidth, uint32_t newHeight)
	{
		FGraphicsCore::CommandQueueManager->Flush();

		FGraphicsCore::ContextManager->ResetAllContext();

		DestroyBuffers();

		ASSERT(mSwapChain != nullptr);

		DXGI_SWAP_CHAIN_DESC desc{};
		DX_CALL(mSwapChain->GetDesc(&desc));
		DX_CALL(mSwapChain->ResizeBuffers(SWAP_CHAIN_BUFFER_COUNT, newWidth, newHeight, D3DFormat(mSwapChainFormat), desc.Flags));

		EnsureSwapChainColorSpace(mCurrentSwapChainBitDepth, mHDRSupport);

		CreateBuffers();

		int32_t HDRMetadataPoolIndex = 0;
		SetHDRMetaData(HDRMetaDataPool[HDRMetadataPoolIndex][0], HDRMetaDataPool[HDRMetadataPoolIndex][1], HDRMetaDataPool[HDRMetadataPoolIndex][2], HDRMetaDataPool[HDRMetadataPoolIndex][3]);
	}

	// To detect HDR support, we will need to check the color space in the primary DXGI output associated with the app at
	// this point in time (using window/display intersection). 

	// Compute the overlay area of two rectangles, A and B.
	// (ax1, ay1) = left-top coordinates of A; (ax2, ay2) = right-bottom coordinates of A
	// (bx1, by1) = left-top coordinates of B; (bx2, by2) = right-bottom coordinates of B
	inline int ComputeIntersectionArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
	{
		return FMath::Min(0, FMath::Min(ax2, bx2) - FMath::Max(ax1, bx1)) * FMath::Max(0, FMath::Min(ay2, by2) - FMath::Max(ay1, by1));
	}

	void Dash::FSwapChain::CheckDisplayHDRSupport()
	{
		// If the display's advanced color state has changed (e.g. HDR display plug/unplug, or OS HDR setting on/off), 
		// then this app's DXGI factory is invalidated and must be created anew in order to retrieve up-to-date display information. 
		if (mDxgiFactory->IsCurrent() == false)
		{
			CreateDxgiFactory();
		}

		// First, the method must determine the app's current display. 
		// We don't recommend using IDXGISwapChain::GetContainingOutput method to do that because of two reasons:
		//    1. Swap chains created with CreateSwapChainForComposition do not support this method.
		//    2. Swap chains will return a stale dxgi output once DXGIFactory::IsCurrent() is false. In addition, 
		//       we don't recommend re-creating swapchain to resolve the stale dxgi output because it will cause a short 
		//       period of black screen.
		// Instead, we suggest enumerating through the bounds of all dxgi outputs and determine which one has the greatest 
		// intersection with the app window bounds. Then, use the DXGI output found in previous step to determine if the 
		// app is on a HDR capable display. 

		// Retrieve the current default adapter.
		ComPtr<IDXGIAdapter1> dxgiAdapter;
		DX_CALL(mDxgiFactory->EnumAdapters1(0, &dxgiAdapter));

		// Iterate through the DXGI outputs associated with the DXGI adapter,
		// and find the output whose bounds have the greatest overlap with the
		// app window (i.e. the output for which the intersection area is the
		// greatest).

		UINT i = 0;
		ComPtr<IDXGIOutput> currentOutput;
		ComPtr<IDXGIOutput> bestOutput;
		float bestIntersectArea = -1;

		while (dxgiAdapter->EnumOutputs(i, &currentOutput) != DXGI_ERROR_NOT_FOUND)
		{
			// Get the retangle bounds of the app window
			int ax1 = mWindowRect.left;
			int ay1 = mWindowRect.top;
			int ax2 = mWindowRect.right;
			int ay2 = mWindowRect.bottom;

			// Get the rectangle bounds of current output
			DXGI_OUTPUT_DESC desc;
			DX_CALL(currentOutput->GetDesc(&desc));
			RECT r = desc.DesktopCoordinates;
			int bx1 = r.left;
			int by1 = r.top;
			int bx2 = r.right;
			int by2 = r.bottom;

			// Compute the intersection
			int intersectArea = ComputeIntersectionArea(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2);
			if (intersectArea > bestIntersectArea)
			{
				bestOutput = currentOutput;
				bestIntersectArea = static_cast<float>(intersectArea);
			}

			i++;
		}

		// Having determined the output (display) upon which the app is primarily being 
		// rendered, retrieve the HDR capabilities of that display by checking the color space.
		ComPtr<IDXGIOutput6> output6;
		DX_CALL(bestOutput.As(&output6));

		DXGI_OUTPUT_DESC1 desc1;
		DX_CALL(output6->GetDesc1(&desc1));

		mHDRSupport = (desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
	}

	void FSwapChain::EnsureSwapChainColorSpace(ESwapChainBitDepth swapChainBitDepth, bool enableST2084)
	{
		DXGI_COLOR_SPACE_TYPE colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
		switch (swapChainBitDepth)
		{
		case ESwapChainBitDepth::_8:
			break;

		case ESwapChainBitDepth::_10:
			colorSpace = enableST2084 ? DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 : DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
			break;

		case ESwapChainBitDepth::_16:
			colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;
			break;
		}

		if (mCurrentSwapChainColorSpace != colorSpace)
		{
			UINT colorSpaceSupport = 0;
			if (SUCCEEDED(mSwapChain->CheckColorSpaceSupport(colorSpace, &colorSpaceSupport)) &&
				((colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) == DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT))
			{
				DX_CALL(mSwapChain->SetColorSpace1(colorSpace));
				mCurrentSwapChainColorSpace = colorSpace;
			}
		}
	}

	void FSwapChain::SetHDRMetaData(float MaxOutputNits, float MinOutputNits, float MaxCLL, float MaxFALL)
	{
		if (!mSwapChain)
		{
			return;
		}

		// Clean the hdr metadata if the display doesn't support HDR
		if (!mHDRSupport)
		{
			DX_CALL(mSwapChain->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_NONE, 0, nullptr));
			return;
		}

		static const EDisplayChromaticities DisplayChromaticityList[] =
		{
			{ 0.64000f, 0.33000f, 0.30000f, 0.60000f, 0.15000f, 0.06000f, 0.31270f, 0.32900f }, // Display Gamut Rec709 
			{ 0.70800f, 0.29200f, 0.17000f, 0.79700f, 0.13100f, 0.04600f, 0.31270f, 0.32900f }, // Display Gamut Rec2020
		};

		// Select the chromaticity based on HDR format of the DWM.
		int selectedChroma = 0;
		if (mCurrentSwapChainBitDepth == ESwapChainBitDepth::_16 && mCurrentSwapChainColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709)
		{
			selectedChroma = 0;
		}
		else if (mCurrentSwapChainBitDepth == ESwapChainBitDepth::_10 && mCurrentSwapChainColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)
		{
			selectedChroma = 1;
		}
		else
		{
			// Reset the metadata since this is not a supported HDR format.
			DX_CALL(mSwapChain->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_NONE, 0, nullptr));
			return;
		}

		// Set HDR meta data
		const EDisplayChromaticities& Chroma = DisplayChromaticityList[selectedChroma];
		DXGI_HDR_METADATA_HDR10 HDR10MetaData = {};
		HDR10MetaData.RedPrimary[0] = static_cast<UINT16>(Chroma.RedX * 50000.0f);
		HDR10MetaData.RedPrimary[1] = static_cast<UINT16>(Chroma.RedY * 50000.0f);
		HDR10MetaData.GreenPrimary[0] = static_cast<UINT16>(Chroma.GreenX * 50000.0f);
		HDR10MetaData.GreenPrimary[1] = static_cast<UINT16>(Chroma.GreenY * 50000.0f);
		HDR10MetaData.BluePrimary[0] = static_cast<UINT16>(Chroma.BlueX * 50000.0f);
		HDR10MetaData.BluePrimary[1] = static_cast<UINT16>(Chroma.BlueY * 50000.0f);
		HDR10MetaData.WhitePoint[0] = static_cast<UINT16>(Chroma.WhiteX * 50000.0f);
		HDR10MetaData.WhitePoint[1] = static_cast<UINT16>(Chroma.WhiteY * 50000.0f);
		HDR10MetaData.MaxMasteringLuminance = static_cast<UINT>(MaxOutputNits * 10000.0f);
		HDR10MetaData.MinMasteringLuminance = static_cast<UINT>(MinOutputNits * 10000.0f);
		HDR10MetaData.MaxContentLightLevel = static_cast<UINT16>(MaxCLL);
		HDR10MetaData.MaxFrameAverageLightLevel = static_cast<UINT16>(MaxFALL);
		DX_CALL(mSwapChain->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_HDR10, sizeof(DXGI_HDR_METADATA_HDR10), &HDR10MetaData));
	}

	EResourceFormat FSwapChain::BitDepthToSwapChainFormat(ESwapChainBitDepth bitDepth)
	{
		switch (bitDepth)
		{
		case ESwapChainBitDepth::_8:
			return EResourceFormat::RGBA8_Unsigned_Norm;
		case ESwapChainBitDepth::_10:
			return EResourceFormat::RGB10A2_Unsigned_Norm;
		case ESwapChainBitDepth::_16:
			return EResourceFormat::RGBA16_Unsigned_Norm;
		default:
			ASSERT(false);
			break;
		}

		return EResourceFormat::Unknown;
	}

	FColorBufferRef FSwapChain::GetCurrentBackBuffer()
	{
		return mSwapChainBuffer[mCurrentBackBufferIndex];
	}

	EResourceFormat FSwapChain::GetBackBufferFormat() const
	{
		return mSwapChainFormat;
	}

	EResourceFormat FSwapChain::GetColorBufferFormat() const
	{
		return mColorBufferFormat;
	}

	EResourceFormat FSwapChain::GetDepthBufferFormat() const
	{
		return mDepthBufferFormat;
	}

	void FSwapChain::ToggleFullscreenMode()
	{
		if (mFullScreenMode)
		{
			// Restore window's attributes and size.
			SetWindowLong(IGameApp::GetInstance()->GetWindowHandle(), GWL_STYLE, mWindowStyle);

			SetWindowPos(
				IGameApp::GetInstance()->GetWindowHandle(),
				HWND_NOTOPMOST,
				mWindowRect.left,
				mWindowRect.top,
				mWindowRect.right - mWindowRect.left,
				mWindowRect.bottom - mWindowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			ShowWindow(IGameApp::GetInstance()->GetWindowHandle(), SW_NORMAL);
		}
		else
		{
			GetWindowRect(IGameApp::GetInstance()->GetWindowHandle(), &mWindowRect);

			SetWindowLong(IGameApp::GetInstance()->GetWindowHandle(), GWL_STYLE, mWindowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

			ComPtr<IDXGIOutput> pOutput;
			DX_CALL(mSwapChain->GetContainingOutput(&pOutput));
			DXGI_OUTPUT_DESC desc;
			DX_CALL(pOutput->GetDesc(&desc));

			RECT fullScreenWindowRect = desc.DesktopCoordinates;

			SetWindowPos(
				IGameApp::GetInstance()->GetWindowHandle(),
				HWND_NOTOPMOST,
				fullScreenWindowRect.left,
				fullScreenWindowRect.top,
				fullScreenWindowRect.right - fullScreenWindowRect.left,
				fullScreenWindowRect.bottom - fullScreenWindowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			ShowWindow(IGameApp::GetInstance()->GetWindowHandle(), SW_MAXIMIZE);
		}

		mFullScreenMode = !mFullScreenMode;
	}

	UINT FSwapChain::GetCurrentBackBufferIndex() const
	{
		return mCurrentBackBufferIndex;
	}

	void FSwapChain::OnKeyPressed(FKeyEventArgs& args)
	{
		if (args.Key == EKeyCode::F11)
		{
			ToggleFullscreenMode();
		}
	}
}