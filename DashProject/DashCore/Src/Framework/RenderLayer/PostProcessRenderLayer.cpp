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

	FComputePSORef ComputeGrayscalePSO = FComputePSO::MakeComputePSO("ComputeGrayscalePSO");
	FShaderPassRef ComputeGrayscalePass = nullptr;

	FPostProcessRenderLayer::FPostProcessRenderLayer()
		: IRenderLayer("PostProcessRenderLayer", 200)
	{
	}

	FPostProcessRenderLayer::~FPostProcessRenderLayer()
	{
		if (mTempRT)
		{
			mTempRT->Destroy();
			mTempRT = nullptr;
		}
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

		{
			FShaderCreationInfo csInfo{ EShaderStage::Compute, FFileUtility::GetEngineShaderDir("ComputeGrayscaleShader.hlsl"),  "CS_Main" };
			ComputeGrayscalePass = FShaderPass::MakeComputeShaderPass("ComputeGrayscalePass", csInfo);

			ComputeGrayscalePSO->SetShaderPass(ComputeGrayscalePass);
			ComputeGrayscalePSO->Finalize();

			mTempRT = FGraphicsCore::Device->CreateColorBuffer("Temp Buffer", FColorBufferDescription::Create2D(FGraphicsCore::SwapChain->GetColorBufferFormat(),
				FGraphicsCore::SwapChain->GetDisplayWidth(), FGraphicsCore::SwapChain->GetDisplayHeight(), FLinearColor::Black,
				1, EResourceState::UnorderedAccess));
		}
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
		if (mTempRT)
		{
			FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("GrayscalePostProcess");

			{
				FGPUProfilerScope profiler(graphicsContext, "GrayscalePostProcess");

				graphicsContext.SetComputePipelineState(ComputeGrayscalePSO);
				graphicsContext.SetUnorderAccessView("OutputTexture", mTempRT);
				graphicsContext.SetShaderResourceView("InputTexture", FGraphicsCore::SwapChain->GetColorBuffer());

				UINT numGroupsX = (UINT)ceilf(mTempRT->GetWidth() / 16.0f);
				UINT numGroupsY = (UINT)ceilf(mTempRT->GetHeight() / 16.0f);
				graphicsContext.Dispatch(numGroupsX, numGroupsY, 1);
			}

			graphicsContext.Finish();
		}
		
		{
			FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("BlitPostProcess");

			{
				FGPUProfilerScope profiler(graphicsContext, "BlitPostProcess");

				graphicsContext.SetRenderTarget(FGraphicsCore::SwapChain->GetCurrentBackBuffer());
				graphicsContext.SetGraphicsPipelineState(PostProcessPSO);
				graphicsContext.SetViewportAndScissor(0, 0, FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetWidth(), FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetHeight());
				//graphicsContext.SetShaderResourceView("ColorBuffer", FGraphicsCore::SwapChain->GetColorBuffer());
				graphicsContext.SetShaderResourceView("ColorBuffer", mTempRT);
				graphicsContext.Draw(3);
			}

			graphicsContext.Finish();
		}
	}

	void FPostProcessRenderLayer::OnWindowResize(const FWindowResizeEventArgs& e)
	{
		if (mTempRT)
		{
			mTempRT->Destroy();
			mTempRT = nullptr;
		}

		mTempRT = FGraphicsCore::Device->CreateColorBuffer("Temp Buffer", FColorBufferDescription::Create2D(FGraphicsCore::SwapChain->GetColorBufferFormat(),
			FGraphicsCore::SwapChain->GetDisplayWidth(), FGraphicsCore::SwapChain->GetDisplayHeight(), FLinearColor::Black,
			1, EResourceState::UnorderedAccess));
	}
}