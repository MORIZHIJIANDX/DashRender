#include "PCH.h"
#include "PostProcessRenderLayer.h"
#include "Utility/FileUtility.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/SwapChain.h"
#include "Graphics/GPUProfiler.h"

namespace Dash
{
	FGraphicsPSORef PostProcessPSO = FGraphicsPSO::MakeGraphicsPSO("PostProcessPSO");
	FShaderPassRef PostProcessPass = nullptr;

	FPostProcessRenderLayer::FPostProcessRenderLayer()
		: IRenderLayer("PostProcessRenderLayer", 200)
	{
	}

	FPostProcessRenderLayer::~FPostProcessRenderLayer()
	{
	}

	void FPostProcessRenderLayer::Init()
	{
		FShaderCreationInfo psInfo{ EShaderStage::Pixel, FFileUtility::GetEngineShaderDir("PostProcessShader.hlsl"),  "PS_Main" };

		FShaderCreationInfo vsInfo{ EShaderStage::Vertex,FFileUtility::GetEngineShaderDir("PostProcessShader.hlsl"),  "VS_Main" };

		FRasterizerState rasterizerDefault{ ERasterizerFillMode::Solid, ERasterizerCullMode::None };

		FBlendState blendDisable{ false, false };
		FDepthStencilState depthStateDisabled{ false, false };

		PostProcessPass = FShaderPass::MakeGraphicShaderPass("PostProcessPass", { vsInfo , psInfo }, blendDisable, rasterizerDefault, depthStateDisabled);

		PostProcessPSO->SetShaderPass(PostProcessPass);
		PostProcessPSO->SetPrimitiveTopologyType(EPrimitiveTopology::TriangleList);
		PostProcessPSO->SetSamplerMask(UINT_MAX);
		PostProcessPSO->SetRenderTargetFormat(FGraphicsCore::SwapChain->GetBackBufferFormat(), FGraphicsCore::SwapChain->GetDepthBufferFormat());
		PostProcessPSO->Finalize();
	}

	void FPostProcessRenderLayer::Shutdown()
	{
	}

	void FPostProcessRenderLayer::OnBeginFrame()
	{
	}

	void FPostProcessRenderLayer::OnEndFrame()
	{
	}

	void FPostProcessRenderLayer::OnUpdate(const FUpdateEventArgs& e)
	{
	}

	void FPostProcessRenderLayer::OnRender(const FRenderEventArgs& e)
	{
		FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("PostProcess");

		{
			FGPUProfilerScope profiler(graphicsContext, "PostProcess");

			graphicsContext.SetRenderTarget(FGraphicsCore::SwapChain->GetCurrentBackBuffer());
			graphicsContext.SetGraphicsPipelineState(PostProcessPSO);
			graphicsContext.SetViewportAndScissor(0, 0, FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetWidth(), FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetHeight());
			graphicsContext.SetShaderResourceView("ColorBuffer", FGraphicsCore::SwapChain->GetColorBuffer());
			graphicsContext.Draw(3);
		}

		graphicsContext.Finish();
	}

	void FPostProcessRenderLayer::OnWindowResize(const FWindowResizeEventArgs& e)
	{
	}
}