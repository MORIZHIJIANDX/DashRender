#include "PCH.h"
#include "Display.h"
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

namespace Dash
{
	using namespace Microsoft::WRL;

	FGraphicsPSORef DrawPSO = std::make_shared<FGraphicsPSO>("DisplayPSO");//{ "DisplayPSO" };
	FGraphicsPSORef PresentPSO = std::make_shared<FGraphicsPSO>("PresentPSO");;//{ "PresentPSO" };
	FShaderPass DrawPass;
	FShaderPass PresentPass;

	void FDisplay::Initialize()
	{
		mDisplayWdith = IGameApp::GetInstance()->GetWindowWidth();
		mDisplayHeight = IGameApp::GetInstance()->GetWindowHeight();

		CreateSwapChain(IGameApp::GetInstance()->GetWindowWidth(), IGameApp::GetInstance()->GetWindowHeight());
		CreateBuffers();

		for (uint32_t index = 0; index < SWAP_CHAIN_BUFFER_COUNT; ++index)
		{
			mFenceValue[index] = ((uint64_t)D3D12_COMMAND_LIST_TYPE_DIRECT) << COMMAND_TYPE_MASK;
		}

		FShaderCreationInfo psInfo{ "..\\DashCore\\Src\\Shaders\\FullScreen_PS.hlsl" ,  "PS_Main" };
		psInfo.Finalize();

		FShaderCreationInfo psPresentInfo{ "..\\DashCore\\Src\\Shaders\\FullScreen_PS.hlsl" ,  "PS_SampleColor" };
		psPresentInfo.Finalize();

		FShaderCreationInfo vsInfo{ "..\\DashCore\\Src\\Shaders\\FullScreen_PS.hlsl" ,  "VS_Main" };
		vsInfo.Finalize();
	
		DrawPass.SetShader(EShaderStage::Vertex, vsInfo);
		DrawPass.SetShader(EShaderStage::Pixel, psInfo);
		DrawPass.SetPassName("DrawPass");

		PresentPass.SetShader(EShaderStage::Vertex, vsInfo);
		PresentPass.SetShader(EShaderStage::Pixel, psPresentInfo);
		PresentPass.SetPassName("PresentPass");

		FRasterizerState rasterizerDefault{ ERasterizerFillMode::Solid, ERasterizerCullMode::Back };

		FBlendState BlendDisable{false, false};
		FDepthStencilState DepthStateDisabled{false, false};

		FInputAssemblerLayout inputLayout;
		inputLayout.AddPerVertexLayoutElement("POSITION", 0, EResourceFormat::RGB32_Float, 0, 0);
		inputLayout.AddPerVertexLayoutElement("TEXCOORD", 0, EResourceFormat::RG32_Float, 0, 12);

		DrawPSO->SetBlendState(BlendDisable);
		DrawPSO->SetDepthStencilState(DepthStateDisabled);
		DrawPSO->SetShaderPass(DrawPass);
		DrawPSO->SetRasterizerState(rasterizerDefault);
		DrawPSO->SetInputLayout(inputLayout);
		DrawPSO->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		DrawPSO->SetSamplerMask(UINT_MAX);
		DrawPSO->SetRenderTargetFormat(mSwapChainFormat, EResourceFormat::Depth32_Float);
		DrawPSO->Finalize();

		PresentPSO->SetBlendState(BlendDisable);
		PresentPSO->SetDepthStencilState(DepthStateDisabled);
		PresentPSO->SetShaderPass(PresentPass);
		PresentPSO->SetRasterizerState(rasterizerDefault);
		PresentPSO->SetInputLayout(inputLayout);
		PresentPSO->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		PresentPSO->SetSamplerMask(UINT_MAX);
		PresentPSO->SetRenderTargetFormat(mSwapChainFormat, EResourceFormat::Depth32_Float);
		PresentPSO->Finalize();
	}

	void FDisplay::Destroy()
	{
		mSwapChain->SetFullscreenState(FALSE, nullptr);

		DestroyBuffers();

		mSwapChain = nullptr;
	}

	void FDisplay::SetDisplayRate(float displayRate)
	{
		if (mDisplayRate != displayRate)
		{
			mDisplayRate = displayRate;
			ForceRecreateBuffers(mSwapChainBuffer[0].GetWidth(), mSwapChainBuffer[0].GetHeight());
			LOG_INFO << "Set Display Rate : " << displayRate;
		}
	}

	void FDisplay::OnWindowResize(uint32_t displayWdith, uint32_t displayHeight)
	{
		if (mSwapChainBuffer[mCurrentBackBufferIndex].GetWidth() != displayWdith || mSwapChainBuffer[mCurrentBackBufferIndex].GetHeight() != displayHeight)
		{
			ForceRecreateBuffers(displayWdith, displayHeight);
		}
	}

	void FDisplay::Present()
	{
	
		FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("Present");

		{
			graphicsContext.SetRenderTarget(mDisplayBuffer);
			graphicsContext.ClearColor(mDisplayBuffer, mDisplayBuffer.GetClearColor());
			graphicsContext.SetPipelineState(DrawPSO);
			graphicsContext.SetViewportAndScissor(0, 0, mDisplayBuffer.GetWidth(), mDisplayBuffer.GetHeight());
			graphicsContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			graphicsContext.Draw(3);
		}
		
		
		{
			graphicsContext.SetRenderTarget(FGraphicsCore::Display->GetCurrentBackBuffer());
			graphicsContext.ClearColor(FGraphicsCore::Display->GetCurrentBackBuffer(), FLinearColor::Gray);
			graphicsContext.SetPipelineState(PresentPSO);
			graphicsContext.SetViewportAndScissor(0, 0, FGraphicsCore::Display->GetCurrentBackBuffer().GetWidth(), FGraphicsCore::Display->GetCurrentBackBuffer().GetHeight());
			graphicsContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			graphicsContext.SetShaderResourceView("DisplayTexture", mDisplayBuffer);
			graphicsContext.Draw(3);
		}

		graphicsContext.TransitionBarrier(FGraphicsCore::Display->GetCurrentBackBuffer(), EResourceState::Present);
		
		graphicsContext.Finish();
		
		mSwapChain->Present(1, 0);

		mFenceValue[mCurrentBackBufferIndex] = FGraphicsCore::CommandQueueManager->GetGraphicsQueue().Signal();

		mCurrentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();
		uint64_t nextBufferFenceValue = mFenceValue[mCurrentBackBufferIndex];
		FGraphicsCore::CommandQueueManager->GetGraphicsQueue().WaitForFence(nextBufferFenceValue);
		
	}

	FColorBuffer& FDisplay::GetDisplayBuffer()
	{
		return mDisplayBuffer;
	}

	void FDisplay::CreateSwapChain(uint32_t displayWdith, uint32_t displayHeight)
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

	void FDisplay::CreateBuffers()
	{
		for (uint32_t index = 0; index < SWAP_CHAIN_BUFFER_COUNT; ++index)
		{
			ComPtr<ID3D12Resource> backBuffer;
			DX_CALL(mSwapChain->GetBuffer(index, IID_PPV_ARGS(&backBuffer)));
			mSwapChainBuffer[index].Create("Swap Chain Buffer[" + FStringUtility::ToString(index) + "]", backBuffer.Detach(), EResourceState::Common); // D3D12_RESOURCE_STATE_PRESENT ?
		}

		mDisplayWdith = static_cast<uint32_t>(FMath::AlignUp(mSwapChainBuffer[0].GetWidth() * mDisplayRate, 2));
		mDisplayHeight = static_cast<uint32_t>(FMath::AlignUp(mSwapChainBuffer[0].GetHeight() * mDisplayRate, 2));
		mDisplayBuffer.Create("Display Buffer", mDisplayWdith, mDisplayHeight, 1, mSwapChainFormat);
		LOG_INFO << "Set Display Width : " << mDisplayWdith << ", Display Height : " << mDisplayHeight;

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

	void FDisplay::ForceRecreateBuffers(uint32_t newWidth, uint32_t newHeight)
	{
		FGraphicsCore::CommandQueueManager->Flush();

		FGraphicsCore::ContextManager->ReleaseAllTrackObjects();

		DestroyBuffers();

		ASSERT(mSwapChain != nullptr);

		DXGI_SWAP_CHAIN_DESC desc{};
		DX_CALL(mSwapChain->GetDesc(&desc));
		DX_CALL(mSwapChain->ResizeBuffers(SWAP_CHAIN_BUFFER_COUNT, newWidth, newHeight, D3DFormat(mSwapChainFormat), desc.Flags));

		CreateBuffers();
	}

	FColorBuffer& FDisplay::GetCurrentBackBuffer()
	{
		return mSwapChainBuffer[mCurrentBackBufferIndex];
	}

}