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

	FGraphicsPSORef presentPSO = FGraphicsPSO::MakeGraphicsPSO("presentPSO");//{ "presentPSO" };
	FShaderPassRef presentPass = nullptr;

	void FSwapChain::Initialize()
	{
		mDisplayWdith = IGameApp::GetInstance()->GetWindowWidth();
		mDisplayHeight = IGameApp::GetInstance()->GetWindowHeight();

		CreateSwapChain(IGameApp::GetInstance()->GetWindowWidth(), IGameApp::GetInstance()->GetWindowHeight());
		CreateBuffers();

		for (uint32_t index = 0; index < SWAP_CHAIN_BUFFER_COUNT; ++index)
		{
			mFenceValue[index] = ((uint64_t)D3D12_COMMAND_LIST_TYPE_DIRECT) << COMMAND_TYPE_MASK;
		}

		FShaderCreationInfo psPresentInfo{ EShaderStage::Pixel, FFileUtility::GetEngineShaderDir("PresentShader.hlsl"),  "PS_Main" };
		 
		FShaderCreationInfo vsInfo{ EShaderStage::Vertex,FFileUtility::GetEngineShaderDir("PresentShader.hlsl"),  "VS_Main" };
	
		FRasterizerState rasterizerDefault{ ERasterizerFillMode::Solid, ERasterizerCullMode::None };

		FBlendState blendDisable{false, false};
		FDepthStencilState depthStateDisabled{false, false};

		presentPass = FShaderPass::MakeShaderPass("PresentPass", { vsInfo , psPresentInfo }, blendDisable, rasterizerDefault, depthStateDisabled);

		presentPSO->SetShaderPass(presentPass);
		presentPSO->SetPrimitiveTopologyType(EPrimitiveTopology::TriangleList);
		presentPSO->SetSamplerMask(UINT_MAX);
		presentPSO->SetRenderTargetFormat(mSwapChainFormat, EResourceFormat::Depth32_Float);
		presentPSO->Finalize();

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

	void FSwapChain::Present()
	{
		FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("Present");

		/*
		{
			graphicsContext.SetRenderTarget(FGraphicsCore::SwapChain->GetCurrentBackBuffer());
			graphicsContext.SetGraphicsPipelineState(presentPSO);
			graphicsContext.SetViewportAndScissor(0, 0, FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetWidth(), FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetHeight());
			graphicsContext.SetShaderResourceView("DisplayTexture", mColorBuffer);
			graphicsContext.Draw(3);
		}	
		*/
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

	void FSwapChain::CreateSwapChain(uint32_t displayWdith, uint32_t displayHeight)
	{
		ASSERT_MSG(mSwapChain == nullptr, "Swap Chain Already Initialized");

		UINT dxgiFactoryFlags = 0;
#if DASH_DEBUG
		ComPtr<ID3D12Debug> DebugInterface;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugInterface))))
		{
			DebugInterface->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif // DASH_DEBUG

		ComPtr<IDXGIFactory4> dxgiFactory;
		DX_CALL(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.Width = displayWdith;
		swapChainDesc.Height = displayHeight;
		swapChainDesc.Format = D3DFormat(mSwapChainFormat);
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc{};
		fullScreenDesc.Windowed = TRUE;

		ComPtr<IDXGISwapChain1> dxgiSwapChain1;
		DX_CALL(dxgiFactory->CreateSwapChainForHwnd(FGraphicsCore::CommandQueueManager->GetGraphicsQueue().GetD3DCommandQueue(),
			IGameApp::GetInstance()->GetWindowHandle(),
			&swapChainDesc,
			&fullScreenDesc,
			nullptr,
			&dxgiSwapChain1));

		if (FGraphicsCore::Device->SupportsTearing())
		{
			dxgiFactory->MakeWindowAssociation(IGameApp::GetInstance()->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);
		}

		DX_CALL(dxgiSwapChain1.As(&mSwapChain));
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

		CreateBuffers();
	}

	FColorBufferRef FSwapChain::GetCurrentBackBuffer()
	{
		return mSwapChainBuffer[mCurrentBackBufferIndex];
	}

	EResourceFormat FSwapChain::GetBackBufferFormat() const
	{
		return mSwapChainFormat;
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