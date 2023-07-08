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

	FGraphicsPSORef drawPSO = FGraphicsPSO::MakeGraphicsPSO("DisplayPSO");//{ "DisplayPSO" };
	FGraphicsPSORef presentPSO = FGraphicsPSO::MakeGraphicsPSO("presentPSO");//{ "presentPSO" };
	FShaderPassRef drawPass = nullptr;
	FShaderPassRef PresentPass = nullptr;

	FGpuDynamicVertexBufferRef vertexBuffer;

	FGpuDynamicVertexBufferRef PositionVertexBuffer;
	FGpuDynamicVertexBufferRef UVVertexBuffer;
	FGpuDynamicVertexBufferRef ColorVertexBuffer;


	struct ConstantParams
	{
		FVector4f TintColor;
		FVector4f Params;
	};

	struct Vertex
	{
		FVector3f Pos;
		FVector2f UV;
		FVector4f Color;
	};

	std::vector<Vertex> vertexData;

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

		FShaderCreationInfo psInfo{ EShaderStage::Pixel, FFileUtility::GetEngineShaderDir("FullScreen_PS.hlsl"),  "PS_Main"};

		FShaderCreationInfo psPresentInfo{ EShaderStage::Pixel, FFileUtility::GetEngineShaderDir("FullScreen_PS.hlsl"),  "PS_SampleColor" };
		 
		FShaderCreationInfo vsInfo{ EShaderStage::Vertex,FFileUtility::GetEngineShaderDir("FullScreen_PS.hlsl"),  "VS_Main" };
	
		FRasterizerState rasterizerDefault{ ERasterizerFillMode::Solid, ERasterizerCullMode::None };

		FBlendState blendDisable{false, false};
		FDepthStencilState depthStateDisabled{false, false};

		drawPass = FShaderPass::MakeShaderPass("DrawPass", { vsInfo , psInfo }, blendDisable, rasterizerDefault, depthStateDisabled);
		PresentPass = FShaderPass::MakeShaderPass("PresentPass", { vsInfo , psPresentInfo }, blendDisable, rasterizerDefault, depthStateDisabled);

		FInputAssemblerLayout inputLayout;
		inputLayout.AddPerVertexLayoutElement("POSITION", 0, EResourceFormat::RGB32_Float, 0);
		inputLayout.AddPerVertexLayoutElement("TEXCOORD", 0, EResourceFormat::RG32_Float, 1);
		inputLayout.AddPerVertexLayoutElement("TEXCOORD", 1, EResourceFormat::RG32_Float, 1, sizeof(FVector2f));
		inputLayout.AddPerVertexLayoutElement("COLOR", 0, EResourceFormat::RGBA32_Float, 2);

		// drawPSO->SetBlendState(blendDisable);
		// drawPSO->SetDepthStencilState(depthStateDisabled);
		drawPSO->SetShaderPass(drawPass);
		// drawPSO->SetRasterizerState(rasterizerDefault);
		drawPSO->SetInputLayout(inputLayout);
		drawPSO->SetPrimitiveTopologyType(EPrimitiveTopology::TriangleList);
		drawPSO->SetSamplerMask(UINT_MAX);
		drawPSO->SetRenderTargetFormat(mSwapChainFormat, EResourceFormat::Depth32_Float);
		drawPSO->Finalize();

		// presentPSO->SetBlendState(blendDisable);
		// presentPSO->SetDepthStencilState(depthStateDisabled);
		presentPSO->SetShaderPass(PresentPass);
		// presentPSO->SetRasterizerState(rasterizerDefault);
		presentPSO->SetInputLayout(inputLayout);
		presentPSO->SetPrimitiveTopologyType(EPrimitiveTopology::TriangleList);
		presentPSO->SetSamplerMask(UINT_MAX);
		presentPSO->SetRenderTargetFormat(mSwapChainFormat, EResourceFormat::Depth32_Float);
		presentPSO->Finalize();
		 
		vertexData.resize(3);
		vertexData[0].Pos = FVector3f{ -1.0f, 3.0f, 0.5f };
		vertexData[0].UV = FVector2f{ 0.0f, -1.0f };
		vertexData[0].Color = FVector4f{ 1.0f, 0.0f, 0.0f, 1.0f };

		vertexData[1].Pos = FVector3f{ 3.0f, -1.0f, 0.5f };
		vertexData[1].UV = FVector2f{ 2.0f, 1.0f };
		vertexData[1].Color = FVector4f{ 0.0f, 1.0f, 0.0f, 1.0f };

		vertexData[2].Pos = FVector3f{ -1.0f, -1.0f, 0.5f };
		vertexData[2].UV = FVector2f{ 0.0f, 1.0f };
		vertexData[2].Color = FVector4f{ 0.0f, 0.0f, 1.0f, 1.0f };
		vertexBuffer = FGraphicsCore::Device->CreateDynamicVertexBuffer("DisplayVertexBuffer", 3, sizeof(Vertex));

		vertexBuffer->UpdateData(vertexData.data(), vertexData.size() * sizeof(Vertex));

		std::vector<FVector3f> vertexPositionData;
		vertexPositionData.reserve(3);
		vertexPositionData.push_back(FVector3f{ -1.0f, 3.0f, 0.5f });
		vertexPositionData.push_back(FVector3f{ 3.0f, -1.0f, 0.5f });
		vertexPositionData.push_back(FVector3f{ -1.0f, -1.0f, 0.5f });

		std::vector<FVector2f> vertexUVData;
		vertexUVData.reserve(3);
		vertexUVData.push_back(FVector2f{ 0.0f, -1.0f });
		vertexUVData.push_back(FVector2f{ 1.0f, 0.5f });

		vertexUVData.push_back(FVector2f{ 2.0f, 1.0f });
		vertexUVData.push_back(FVector2f{ 0.5f, 0.5f });

		vertexUVData.push_back(FVector2f{ 0.0f, 1.0f });
		vertexUVData.push_back(FVector2f{ 1.0f, 0.5f });

		std::vector<FVector4f> vertexColorData;
		vertexColorData.reserve(3);
		vertexColorData.push_back(FVector4f{ 1.0f, 0.0f, 0.0f, 1.0f });
		vertexColorData.push_back(FVector4f{ 0.0f, 1.0f, 0.0f, 1.0f });
		vertexColorData.push_back(FVector4f{ 0.0f, 0.0f, 1.0f, 1.0f });

		uint32_t UVCount = 2;

		PositionVertexBuffer = FGraphicsCore::Device->CreateDynamicVertexBuffer("PositionVertexBuffer", 3, sizeof(FVector3f));
		UVVertexBuffer = FGraphicsCore::Device->CreateDynamicVertexBuffer("UVVertexBuffer", 3, sizeof(FVector2f) * UVCount);
		ColorVertexBuffer = FGraphicsCore::Device->CreateDynamicVertexBuffer("ColorVertexBuffer", 3, sizeof(FVector4f));

		PositionVertexBuffer->UpdateData(vertexPositionData.data(), vertexPositionData.size() * sizeof(FVector3f));
		UVVertexBuffer->UpdateData(vertexUVData.data(), vertexUVData.size() * sizeof(FVector2f));
		ColorVertexBuffer->UpdateData(vertexColorData.data(), vertexColorData.size() * sizeof(FVector4f));

		
		const int32_t textureWidth = 512;
		std::vector<FColor> colorData;
		colorData.reserve(textureWidth * textureWidth);
		for (int32_t x = 0; x < textureWidth; ++ x)
		{
			for (int32_t y = 0; y < textureWidth; y++)
			{
				Scalar Size = 64;
				FVector2f Pos= FMath::Floor(FVector2f(x, y) / Size);
				uint8_t PatternMask = static_cast<uint8_t>(FMath::Fmod(Pos.X + FMath::Fmod(Pos.Y, 2.0f), 2.0f) * 125);

				colorData.push_back(FColor{PatternMask, PatternMask, PatternMask});
			}	
		}

		FTextureBufferDescription textureDest = FTextureBufferDescription::Create2D(EResourceFormat::RGBA8_Unsigned_Norm, textureWidth, textureWidth, 1);
		mTexture = FGraphicsCore::Device->CreateTextureBufferFromMemory("TestTexture", textureDest, colorData.data());
		
		
		//std::string pngTexturePath = std::string(ENGINE_PATH) + "/Resource/AssaultRifle_BaseColor.png";
		//std::string tgaTexturePath = std::string(ENGINE_PATH) + "/Resource/TestTGA.tga";
		//std::string hdrTexturePath = std::string(ENGINE_PATH) + "/Resource/Newport_Loft_Ref.hdr";
		//std::string ddsTexturePath = std::string(ENGINE_PATH) + "/Resource/earth.dds";

		//mTexture = FGraphicsCore::Device->CreateTextureBufferFromFile("WIC_Texture", pngTexturePath);
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

		vertexBuffer->Destroy();

		PositionVertexBuffer->Destroy();
		UVVertexBuffer->Destroy();
		ColorVertexBuffer->Destroy();
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
		ConstantParams param;
		param.TintColor = FVector4f{ 1.0f, 1.0f, 0.0f, 1.0f };
		param.Params = FVector4f{ 1.0f, 1.0f, 0.5f, 1.0f };
	
		FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("Present");

		FGpuVertexBufferRef vertexBuffers[3] = { PositionVertexBuffer ,UVVertexBuffer, ColorVertexBuffer };

		{
			graphicsContext.SetRenderTarget(mDisplayBuffer);
			graphicsContext.SetGraphicsPipelineState(drawPSO);
			graphicsContext.SetViewportAndScissor(0, 0, mDisplayBuffer->GetWidth(), mDisplayBuffer->GetHeight());
			//graphicsContext.SetVertexBuffer(0, vertexBuffer);
			graphicsContext.SetVertexBuffers(0, 3, vertexBuffers);
			graphicsContext.Draw(3);
		}

		{
			graphicsContext.SetRenderTarget(FGraphicsCore::SwapChain->GetCurrentBackBuffer());
			graphicsContext.SetGraphicsPipelineState(presentPSO);
			graphicsContext.SetViewportAndScissor(0, 0, FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetWidth(), FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetHeight());
			graphicsContext.SetShaderResourceView("DisplayTexture", mDisplayBuffer);
			//graphicsContext.SetVertexBuffer(0, vertexBuffer);
			graphicsContext.SetVertexBuffers(0, 3, vertexBuffers);
			graphicsContext.Draw(3);
		}

		graphicsContext.TransitionBarrier(FGraphicsCore::SwapChain->GetCurrentBackBuffer(), EResourceState::Present);
		
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