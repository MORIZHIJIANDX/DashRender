#include "PCH.h"
#include "Display.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"
#include "GameApp.h"
#include "CommandQueue.h"
#include "CommandContext.h"

namespace Dash
{
	using namespace Microsoft::WRL;

	void FDisplay::Initialize()
	{
		CreateSwapChain(IGameApp::GetInstance()->GetWindowWidth(), IGameApp::GetInstance()->GetWindowHeight());
		CreateBuffers();

		for (uint32_t index = 0; index < SWAP_CHAIN_BUFFER_COUNT; ++index)
		{
			mFenceValue[index] = ((uint64_t)D3D12_COMMAND_LIST_TYPE_DIRECT) << COMMAND_TYPE_MASK;;
		}
	}

	void FDisplay::Destroy()
	{
		mSwapChain->SetFullscreenState(FALSE, nullptr);

		DestroyBuffers();

		mSwapChain = nullptr;
	}

	void FDisplay::Resize(uint32_t displayWdith, uint32_t displayHeight)
	{
		if (mDisplayWdith != displayWdith || mDisplayHeight != displayHeight)
		{
			FGraphicsCore::CommandQueueManager->Flush();
			
			FGraphicsCore::ContextManager->ReleaseAllTrackObjects();

			DestroyBuffers();

			ASSERT(mSwapChain != nullptr);

			mDisplayWdith = displayWdith;
			mDisplayHeight = displayHeight;

			DXGI_SWAP_CHAIN_DESC desc{};
			DX_CALL(mSwapChain->GetDesc(&desc));
			DX_CALL(mSwapChain->ResizeBuffers(SWAP_CHAIN_BUFFER_COUNT, displayWdith, displayHeight, mSwapChainFormat, desc.Flags));

			CreateBuffers();
		}
	}

	void FDisplay::Present()
	{
		FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("Present");

		graphicsContext.ClearColor(FGraphicsCore::Display->GetDisplayBuffer(), FLinearColor::Gray);

		graphicsContext.TransitionBarrier(FGraphicsCore::Display->GetDisplayBuffer(), D3D12_RESOURCE_STATE_PRESENT);

		graphicsContext.Finish();

		mSwapChain->Present(1, 0);

		mFenceValue[mCurrentBackBufferIndex] = FGraphicsCore::CommandQueueManager->GetGraphicsQueue().Signal();

		mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
		uint64_t nextBufferFenceValue = mFenceValue[mCurrentBackBufferIndex];
		FGraphicsCore::CommandQueueManager->GetGraphicsQueue().WaitForFence(nextBufferFenceValue);
	}

	FColorBuffer& FDisplay::GetDisplayBuffer()
	{
		return mSwapChainBuffer[mCurrentBackBufferIndex];
	}

	void FDisplay::CreateSwapChain(uint32_t displayWdith, uint32_t displayHeight)
	{
		ASSERT_MSG(mSwapChain == nullptr, "Swap Chain Already Initialized");

		mDisplayWdith = displayWdith;
		mDisplayHeight = displayHeight;

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
		swapChainDesc.Format = mSwapChainFormat;
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

		DX_CALL(dxgiSwapChain1.As(&mSwapChain));
	}

	void FDisplay::CreateBuffers()
	{
		for (uint32_t index = 0; index < SWAP_CHAIN_BUFFER_COUNT; ++index)
		{
			ComPtr<ID3D12Resource> backBuffer;
			DX_CALL(mSwapChain->GetBuffer(index, IID_PPV_ARGS(&backBuffer)));
			mSwapChainBuffer[index].Create("Swap Chain Buffer[" + FStringUtility::ToString(index) + "]", backBuffer.Detach(), D3D12_RESOURCE_STATE_COMMON); // D3D12_RESOURCE_STATE_PRESENT ?
		}

		mDisplayBuffer.Create("Display Buffer", mDisplayWdith, mDisplayHeight, 1, mSwapChainFormat);

		mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
	}

	void FDisplay::DestroyBuffers()
	{
		for (uint32_t index = 0; index < SWAP_CHAIN_BUFFER_COUNT; ++index)
		{
			mSwapChainBuffer[index].Destroy();
		}

		mDisplayBuffer.Destroy();
	}

}