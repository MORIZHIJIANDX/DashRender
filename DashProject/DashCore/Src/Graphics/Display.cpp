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

	//FShaderMap ShaderMap;
	FShaderResource PSShader;
	FShaderResource PSShaderPresent;
	FShaderResource VSShader;
	FRootSignature RootSignature{};
	FGraphicsPSO PSO{ "DisplayPSO" };
	FGraphicsPSO PresentPSO{ "PresentPSO" };
	FGpuVertexBuffer VertexBuffer;
	FShaderPass DrawPass;
	FShaderPass PresentPass;
	//FGpuIndexBuffer IndexBuffer;

	struct Vertex
	{
		FVector3f Pos;
		FVector2f UV;
		FVector4f Color;
	};

	struct ConstantParams
	{
		FVector4f TintColor;
		FVector4f Params;
	};

	std::vector<Vertex> VertexData;


	void FDisplay::Initialize()
	{
		CreateSwapChain(IGameApp::GetInstance()->GetWindowWidth(), IGameApp::GetInstance()->GetWindowHeight());
		CreateBuffers();

		for (uint32_t index = 0; index < SWAP_CHAIN_BUFFER_COUNT; ++index)
		{
			mFenceValue[index] = ((uint64_t)D3D12_COMMAND_LIST_TYPE_DIRECT) << COMMAND_TYPE_MASK;;
		}

		FShaderMap::Init();

		FShaderCreationInfo psInfo{ "..\\DashCore\\Src\\Shaders\\FullScreen_PS.hlsl" ,  "PS_Main" };
		psInfo.Finalize();
		PSShader = FShaderMap::LoadShader(psInfo);

		FShaderCreationInfo psPresentInfo{ "..\\DashCore\\Src\\Shaders\\FullScreen_PS.hlsl" ,  "PS_SampleColor" };
		psPresentInfo.Finalize();
		PSShaderPresent = FShaderMap::LoadShader(psPresentInfo);

		FShaderCreationInfo vsInfo{ "..\\DashCore\\Src\\Shaders\\FullScreen_PS.hlsl" ,  "VS_Main" };
		vsInfo.Finalize();
		VSShader = FShaderMap::LoadShader(vsInfo);
	
		DrawPass.SetShader(EShaderStage::Vertex, vsInfo);
		DrawPass.SetShader(EShaderStage::Pixel, psInfo);
		//DrawPass.Finalize("DrawPass");
		DrawPass.SetPassName("DrawPass");

		PresentPass.SetShader(EShaderStage::Vertex, vsInfo);
		PresentPass.SetShader(EShaderStage::Pixel, psPresentInfo);
		PresentPass.SetPassName("PresentPass");

		D3D12_SAMPLER_DESC sampler{};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;

		RootSignature.Reset(2, 1);
		RootSignature[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
		//RootSignature[1].InitAsRootConstantBufferView(0, D3D12_SHADER_VISIBILITY_PIXEL);
		RootSignature[1].InitAsRootConstants(0, sizeof(ConstantParams), D3D12_SHADER_VISIBILITY_ALL);
		RootSignature.InitStaticSampler(0, sampler, D3D12_SHADER_VISIBILITY_PIXEL);
		RootSignature.Finalize("DisplayRootSignature", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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
		inputLayout.AddPerVertexLayoutElement("COLOR", 0, EResourceFormat::RGBA32_Float, 0, 20);

		//PSO.SetRootSignature(RootSignature);
		PSO.SetBlendState(BlendDisable);
		PSO.SetDepthStencilState(DepthStateDisabled);
		//PSO.SetVertexShader(CD3DX12_SHADER_BYTECODE{ VSShader.GetCompiledShader().Data, VSShader.GetCompiledShader().Size});
		//PSO.SetPixelShader(CD3DX12_SHADER_BYTECODE{ PSShader.GetCompiledShader().Data, PSShader.GetCompiledShader().Size });
		PSO.SetShaderPass(DrawPass);
		PSO.SetRasterizerState(RasterizerTwoSided);
		PSO.SetInputLayout(inputLayout);
		PSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		PSO.SetSamplerMask(UINT_MAX);
		PSO.SetRenderTargetFormat(mSwapChainFormat, EResourceFormat::Depth32_Float);
		PSO.Finalize();

		//PresentPSO.SetRootSignature(RootSignature);
		PresentPSO.SetBlendState(BlendDisable);
		PresentPSO.SetDepthStencilState(DepthStateDisabled);
		//PresentPSO.SetVertexShader(CD3DX12_SHADER_BYTECODE{ VSShader.GetCompiledShader().Data, VSShader.GetCompiledShader().Size });
		//PresentPSO.SetPixelShader(CD3DX12_SHADER_BYTECODE{ PSShaderPresent.GetCompiledShader().Data, PSShaderPresent.GetCompiledShader().Size });
		PresentPSO.SetShaderPass(PresentPass);
		PresentPSO.SetRasterizerState(RasterizerTwoSided);
		PresentPSO.SetInputLayout(inputLayout);
		PresentPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		PresentPSO.SetSamplerMask(UINT_MAX);
		PresentPSO.SetRenderTargetFormat(mSwapChainFormat, EResourceFormat::Depth32_Float);
		PresentPSO.Finalize();
	
		VertexData.resize(3);
		VertexData[0].Pos = FVector3f{-1.0f, 3.0f, 0.5f};
		VertexData[0].UV = FVector2f{ 0.0f, -1.0f };
		VertexData[0].Color = FVector4f{ 1.0f, 0.0f, 0.0f, 1.0f };

		VertexData[1].Pos = FVector3f{ 3.0f, -1.0f, 0.5f };
		VertexData[1].UV = FVector2f{ 2.0f, 1.0f };
		VertexData[1].Color = FVector4f{ 0.0f, 1.0f, 0.0f, 1.0f };

		VertexData[2].Pos = FVector3f{ -1.0f, -1.0f, 0.5f };
		VertexData[2].UV = FVector2f{ 0.0f, 1.0f };
		VertexData[2].Color = FVector4f{ 0.0f, 0.0f, 1.0f, 1.0f };
		VertexBuffer.Create("DisplayVertexBuffer", 3, sizeof(Vertex), VertexData.data());	
	}

	void FDisplay::Destroy()
	{
		mSwapChain->SetFullscreenState(FALSE, nullptr);

		DestroyBuffers();

		mSwapChain = nullptr;


		VertexBuffer.Destroy();
		FShaderMap::Destroy();
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
			DX_CALL(mSwapChain->ResizeBuffers(SWAP_CHAIN_BUFFER_COUNT, displayWdith, displayHeight, D3DFormat(mSwapChainFormat), desc.Flags));

			CreateBuffers();
		}
	}

	void FDisplay::Present()
	{
		FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("Present");

		ConstantParams param;
		param.TintColor = FVector4f{ 1.0f, 1.0f, 0.0f, 1.0f };
		param.Params = FVector4f{ 1.0f, 1.0f, 0.5f, 1.0f };


		{
			graphicsContext.SetRenderTarget(mDisplayBuffer);
			graphicsContext.ClearColor(mDisplayBuffer, mDisplayBuffer.GetClearColor());
			//graphicsContext.SetRootSignature(RootSignature);
			graphicsContext.SetPipelineState(PSO);
			graphicsContext.SetViewportAndScissor(0, 0, IGameApp::GetInstance()->GetWindowWidth(), IGameApp::GetInstance()->GetWindowHeight());
			graphicsContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//graphicsContext.SetRootConstantBufferView<ConstantParams>("FrameBuffer", param);
			graphicsContext.SetVertexBuffer(0, VertexBuffer);
			graphicsContext.Draw(3);
		}

		{
			graphicsContext.SetRenderTarget(FGraphicsCore::Display->GetDisplayBuffer());
			graphicsContext.ClearColor(FGraphicsCore::Display->GetDisplayBuffer(), FLinearColor::Gray);
			//graphicsContext.SetRootSignature(RootSignature);
			graphicsContext.SetPipelineState(PresentPSO);
			graphicsContext.SetViewportAndScissor(0, 0, IGameApp::GetInstance()->GetWindowWidth(), IGameApp::GetInstance()->GetWindowHeight());
			graphicsContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			graphicsContext.SetShaderResourceView("DisplayTexture", mDisplayBuffer);
			//graphicsContext.SetRootConstantBufferView<ConstantParams>("FrameBuffer", param);
			//graphicsContext.SetRootConstantBufferView<ConstantParams>("AnotherBuffer", param);
			//graphicsContext.Set32BitConstants<ConstantParams>(1, param);
			graphicsContext.SetVertexBuffer(0, VertexBuffer);
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