#include "PCH.h"
#include "SwapChain.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"
#include "GameApp.h"
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
#include "TextureLoader/HDRTextureLoader.h"

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
	}

	void FSwapChain::SetVSyncEnable(bool enable)
	{
		mVSyncEnable = enable;
	}

	void FSwapChain::Destroy()
	{
		mSwapChain->SetFullscreenState(FALSE, nullptr);

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

	void FSwapChain::Present(FGraphicsCommandContext& graphicsContext)
	{
		graphicsContext.PIXBeginEvent("Present");

		{
			graphicsContext.SetRenderTarget(FGraphicsCore::SwapChain->GetCurrentBackBuffer());
			graphicsContext.SetGraphicsPipelineState(presentPSO);
			graphicsContext.SetViewportAndScissor(0, 0, FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetWidth(), FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetHeight());
			graphicsContext.SetShaderResourceView("DisplayTexture", mDisplayBuffer);
			graphicsContext.Draw(3);
		}	
		
		graphicsContext.TransitionBarrier(FGraphicsCore::SwapChain->GetCurrentBackBuffer(), EResourceState::Present);
		
		graphicsContext.PIXEndEvent();

		graphicsContext.Finish();
		
		UINT PresentInterval = mVSyncEnable ? 1 : 0;

		mSwapChain->Present(PresentInterval, 0);

		mFenceValue[mCurrentBackBufferIndex] = FGraphicsCore::CommandQueueManager->GetGraphicsQueue().Signal();

		mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
		uint64_t nextBufferFenceValue = mFenceValue[mCurrentBackBufferIndex];
		FGraphicsCore::CommandQueueManager->GetGraphicsQueue().WaitForFence(nextBufferFenceValue);	
	}

	FColorBufferRef FSwapChain::GetDisplayBuffer()
	{
		return mDisplayBuffer;
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
		mDisplayBuffer = FGraphicsCore::Device->CreateColorBuffer("Display Buffer", mDisplayWdith, mDisplayHeight, 1, mSwapChainFormat);
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

		mDisplayBuffer->Destroy();
		mDisplayBuffer = nullptr;
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
}