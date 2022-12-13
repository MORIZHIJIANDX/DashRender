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

namespace Dash
{
	using namespace Microsoft::WRL;

	FGraphicsPSO PSO{ "DisplayPSO" };
	FGraphicsPSO PresentPSO{ "PresentPSO" };
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
			mFenceValue[index] = ((uint64_t)D3D12_COMMAND_LIST_TYPE_DIRECT) << COMMAND_TYPE_MASK;;
		}

		FShaderMap::Init();

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

		D3D12_RASTERIZER_DESC RasterizerDefault{};
		RasterizerDefault.FillMode = D3D12_FILL_MODE_SOLID;
		RasterizerDefault.CullMode = D3D12_CULL_MODE_BACK;
		RasterizerDefault.FrontCounterClockwise = TRUE;
		RasterizerDefault.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		RasterizerDefault.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		RasterizerDefault.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		RasterizerDefault.DepthClipEnable = TRUE;
		RasterizerDefault.MultisampleEnable = FALSE;
		RasterizerDefault.AntialiasedLineEnable = FALSE;
		RasterizerDefault.ForcedSampleCount = 0;
		RasterizerDefault.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;;

		D3D12_RASTERIZER_DESC RasterizerTwoSided{};
		RasterizerTwoSided = RasterizerDefault;
		RasterizerTwoSided.CullMode = D3D12_CULL_MODE_NONE;

		D3D12_BLEND_DESC alphaBlend{};
		alphaBlend.IndependentBlendEnable = FALSE;
		alphaBlend.RenderTarget[0].BlendEnable = FALSE;
		alphaBlend.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		alphaBlend.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		alphaBlend.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		alphaBlend.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		alphaBlend.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		alphaBlend.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		alphaBlend.RenderTarget[0].RenderTargetWriteMask = 0;

		alphaBlend.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		D3D12_BLEND_DESC BlendDisable = alphaBlend;

		CD3DX12_DEPTH_STENCIL_DESC DepthStateDisabled{};
		DepthStateDisabled.DepthEnable = FALSE;
		DepthStateDisabled.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		DepthStateDisabled.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		DepthStateDisabled.StencilEnable = FALSE;
		DepthStateDisabled.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		DepthStateDisabled.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		DepthStateDisabled.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		DepthStateDisabled.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		DepthStateDisabled.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		DepthStateDisabled.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		DepthStateDisabled.BackFace = DepthStateDisabled.FrontFace;

		FInputAssemblerLayout inputLayout;
		inputLayout.AddPerVertexLayoutElement("POSITION", 0, EResourceFormat::RGB32_Float, 0, 0);
		inputLayout.AddPerVertexLayoutElement("TEXCOORD", 0, EResourceFormat::RG32_Float, 0, 12);

		PSO.SetBlendState(BlendDisable);
		PSO.SetDepthStencilState(DepthStateDisabled);
		PSO.SetShaderPass(DrawPass);
		PSO.SetRasterizerState(RasterizerTwoSided);
		PSO.SetInputLayout(inputLayout);
		PSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		PSO.SetSamplerMask(UINT_MAX);
		PSO.SetRenderTargetFormat(mSwapChainFormat, EResourceFormat::Depth32_Float);
		PSO.Finalize();

		PresentPSO.SetBlendState(BlendDisable);
		PresentPSO.SetDepthStencilState(DepthStateDisabled);
		PresentPSO.SetShaderPass(PresentPass);
		PresentPSO.SetRasterizerState(RasterizerTwoSided);
		PresentPSO.SetInputLayout(inputLayout);
		PresentPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		PresentPSO.SetSamplerMask(UINT_MAX);
		PresentPSO.SetRenderTargetFormat(mSwapChainFormat, EResourceFormat::Depth32_Float);
		PresentPSO.Finalize();
	}

	void FDisplay::Destroy()
	{
		mSwapChain->SetFullscreenState(FALSE, nullptr);

		DestroyBuffers();

		mSwapChain = nullptr;

		FShaderMap::Destroy();
	}

	void FDisplay::SetDisplayRate(float displayRate)
	{
		mDisplayRate = displayRate;
		ForceRecreateBuffers(mSwapChainBuffer[0].GetWidth(), mSwapChainBuffer[0].GetHeight());
		LOG_INFO << "Set Display Rate : " << displayRate;
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
			graphicsContext.SetPipelineState(PSO);
			graphicsContext.SetViewportAndScissor(0, 0, mDisplayBuffer.GetWidth(), mDisplayBuffer.GetHeight());
			graphicsContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			graphicsContext.Draw(3);
		}

		{
			graphicsContext.SetRenderTarget(mSwapChainBuffer[mCurrentBackBufferIndex]);
			graphicsContext.ClearColor(FGraphicsCore::Display->GetDisplayBuffer(), FLinearColor::Gray);
			graphicsContext.SetPipelineState(PresentPSO);
			graphicsContext.SetViewportAndScissor(0, 0, mSwapChainBuffer[mCurrentBackBufferIndex].GetWidth(), mSwapChainBuffer[mCurrentBackBufferIndex].GetHeight());
			graphicsContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			graphicsContext.SetShaderResourceView("DisplayTexture", mDisplayBuffer);
			graphicsContext.Draw(3);
		}

		graphicsContext.TransitionBarrier(FGraphicsCore::Display->GetDisplayBuffer(), EResourceState::Present);

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

}