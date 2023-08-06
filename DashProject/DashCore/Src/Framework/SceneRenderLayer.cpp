#include "PCH.h"
#include "SceneRenderLayer.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/ShaderPass.h"
#include "Graphics/PipelineStateObject.h"
#include "Graphics/SwapChain.h"
#include "Utility/FileUtility.h"
#include "Utility/Keyboard.h"
#include "Utility/Mouse.h"
#include "ModelLoader/StaticMeshLoader.h"
#include "GameApp.h"
#include "imgui/imgui.h"

namespace Dash
{
	struct FMeshConstantBuffer
	{
		FMatrix4x4 ModelViewProjectionMatrix{ FIdentity{} };
	};

	FMeshConstantBuffer MeshConstantBuffer;

	FGpuVertexBufferRef PositionVertexBuffer;
	FGpuVertexBufferRef NormalVertexBuffer;
	FGpuVertexBufferRef UVVertexBuffer;
	FGpuIndexBufferRef IndexBuffer;
	FShaderPassRef MeshDrawPass;
	FGraphicsPSORef MeshDrawPSO = FGraphicsPSO::MakeGraphicsPSO("MeshDrawPSO");;

	uint32_t VertexCount;

	void CreateVertexBuffers(const FImportedStaticMeshData& meshData)
	{
		VertexCount = meshData.numVertexes;
		if (VertexCount > 0)
		{
			PositionVertexBuffer = FGraphicsCore::Device->CreateVertexBuffer("MeshPositionVertexBuffer", VertexCount, sizeof(meshData.PositionData[0]), meshData.PositionData.data());
			NormalVertexBuffer = FGraphicsCore::Device->CreateVertexBuffer("MeshNormalVertexBuffer", VertexCount, sizeof(meshData.NormalData[0]), meshData.NormalData.data());
			UVVertexBuffer = FGraphicsCore::Device->CreateVertexBuffer("MeshUVVertexBuffer", VertexCount, sizeof(meshData.UVData[0]), meshData.UVData.data());

			IndexBuffer = FGraphicsCore::Device->CreateIndexBuffer("MeshIndexBuffer", VertexCount, meshData.indices.data(), true);
		}

		FShaderCreationInfo psPresentInfo{ EShaderStage::Pixel, FFileUtility::GetEngineShaderDir("MeshShader.hlsl"),  "PS_Main" };

		FShaderCreationInfo vsInfo{ EShaderStage::Vertex,FFileUtility::GetEngineShaderDir("MeshShader.hlsl"),  "VS_Main" };

		FRasterizerState rasterizerDefault{ ERasterizerFillMode::Solid, ERasterizerCullMode::None };

		FBlendState blendDisable{ false, false };
		FDepthStencilState depthStateDisabled{ true };

		FShaderPassRef meshDrawPass = FShaderPass::MakeShaderPass("PresentPass", { vsInfo , psPresentInfo }, blendDisable, rasterizerDefault, depthStateDisabled);

		MeshDrawPSO->SetShaderPass(meshDrawPass);
		MeshDrawPSO->SetPrimitiveTopologyType(EPrimitiveTopology::TriangleList);
		MeshDrawPSO->SetSamplerMask(UINT_MAX);
		MeshDrawPSO->SetRenderTargetFormat(FGraphicsCore::SwapChain->GetBackBufferFormat(), FGraphicsCore::SwapChain->GetDepthBuffer()->GetFormat());
		MeshDrawPSO->Finalize();
	}

	void RenderMesh(FGraphicsCommandContext& graphicsContext)
	{
		FResourceMagnitude renderTargetMagnitude = FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetDesc().Magnitude;

		FGpuVertexBufferRef vertexBuffers[3] = { PositionVertexBuffer ,NormalVertexBuffer, UVVertexBuffer };

		graphicsContext.SetGraphicsPipelineState(MeshDrawPSO);

		graphicsContext.SetRootConstantBufferView("ConstantBuffer", MeshConstantBuffer);

		graphicsContext.SetViewportAndScissor(0, 0, renderTargetMagnitude.Width, renderTargetMagnitude.Height);

		graphicsContext.SetVertexBuffers(0, 3, vertexBuffers);

		graphicsContext.SetIndexBuffer(IndexBuffer);

		graphicsContext.DrawIndexed(VertexCount);
	}

	FSceneRenderLayer::FSceneRenderLayer()
		: IRenderLayer("SceneRenderLayer", 100)
	{
	}

	FSceneRenderLayer::~FSceneRenderLayer()
	{
	}

	void FSceneRenderLayer::Init()
	{
		FImportedStaticMeshData importedStaticMeshData;

		std::string fbxMeshPath = std::string(ENGINE_PATH) + "/Resource/Cyborg_Weapon.fbx";

		bool result = LoadStaticMeshFromFile(fbxMeshPath, importedStaticMeshData);

		if (result)
		{
			importedStaticMeshData.hasNormal;

			CreateVertexBuffers(importedStaticMeshData);

			LOG_INFO << "Load mesh succeed!";
		}

		float fov = 45.0f;
		float aspect = IGameApp::GetInstance()->GetWindowWidth() / (float)IGameApp::GetInstance()->GetWindowHeight();

		mCamera = std::make_shared<FPerspectiveCamera>();
		mCamera->SetCameraParams(aspect, fov, 0.1f, 100.0f);
		mCamera->SetPosition(FVector3f{ 0.0f, 0.0f, -2.0f });

		OnMouseWheelDownDelegate = FMouseWheelEventDelegate::Create<FSceneRenderLayer, &FSceneRenderLayer::OnMouseWheelDown>(this);
		OnMouseWheelUpDelegate = FMouseWheelEventDelegate::Create<FSceneRenderLayer, &FSceneRenderLayer::OnMouseWheelUp>(this);
		OnMouseMoveDelegate = FMouseMotionEventDelegate::Create<FSceneRenderLayer, &FSceneRenderLayer::OnMouseMove>(this);

		FMouse::Get().MouseWheelDown += OnMouseWheelDownDelegate;
		FMouse::Get().MouseWheelUp += OnMouseWheelUpDelegate;
		FMouse::Get().MouseMoved += OnMouseMoveDelegate;
	}

	void FSceneRenderLayer::Shutdown()
	{
		if (IndexBuffer)
		{
			IndexBuffer->Destroy();
		}

		if (PositionVertexBuffer)
		{
			PositionVertexBuffer->Destroy();
		}

		if (NormalVertexBuffer)
		{
			NormalVertexBuffer->Destroy();
		}

		if (UVVertexBuffer)
		{
			UVVertexBuffer->Destroy();
		}

		FMouse::Get().MouseWheelDown -= OnMouseWheelDownDelegate;
		FMouse::Get().MouseWheelUp -= OnMouseWheelUpDelegate;
		FMouse::Get().MouseMoved -= OnMouseMoveDelegate;
	}

	void FSceneRenderLayer::OnBeginFrame()
	{
	}

	void FSceneRenderLayer::OnEndFrame()
	{
	}

	void FSceneRenderLayer::OnUpdate(const FUpdateEventArgs& e)
	{
		Scalar speed = 1.0f;

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::ShiftKey))
		{
			speed *= 2.0f;
		}

		Scalar translate = Scalar(speed * e.ElapsedTime);

		UpdateCamera(translate);
		
		MeshConstantBuffer.ModelViewProjectionMatrix = mCamera->GetViewProjectionMatrix();
	}

	void FSceneRenderLayer::OnRender(const FRenderEventArgs& e)
	{
		FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("RenderScene");

		graphicsContext.SetRenderTarget(FGraphicsCore::SwapChain->GetCurrentBackBuffer(), FGraphicsCore::SwapChain->GetDepthBuffer());
		graphicsContext.ClearColor(FGraphicsCore::SwapChain->GetCurrentBackBuffer());
		graphicsContext.ClearDepth(FGraphicsCore::SwapChain->GetDepthBuffer());

		RenderMesh(graphicsContext);

		graphicsContext.Finish();
	}

	void FSceneRenderLayer::OnWindowResize(const FResizeEventArgs& e)
	{
		float aspect = e.Width / (float)e.Height;
		float fov = 45.0f;
		mCamera->SetCameraParams(aspect, fov, 0.1f, 100.0f);
	}

	void FSceneRenderLayer::OnMouseWheelDown(FMouseWheelEventArgs& e)
	{
		mCamera->TranslateForward(0.001f * e.WheelDelta);
	}

	void FSceneRenderLayer::OnMouseWheelUp(FMouseWheelEventArgs& e)
	{
		mCamera->TranslateForward(0.001f * e.WheelDelta);
	}

	void FSceneRenderLayer::OnMouseMove(FMouseMotionEventArgs& e)
	{
		float RotationSpeed = 0.1f;
		if (FMouse::Get().GetButtonState(EMouseButton::Right).Pressed)
		{
			mCamera->AddPitch(e.mRelY * RotationSpeed);
			mCamera->AddYaw(e.mRelX * RotationSpeed);
		}
	}

	void FSceneRenderLayer::UpdateCamera(Scalar translate)
	{
		if (FKeyboard::Get().IsKeyPressed(EKeyCode::A))
		{
			mCamera->TranslateLeft(translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::D))
		{
			mCamera->TranslateRight(translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::W))
		{
			mCamera->TranslateForward(translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::S))
		{
			mCamera->TranslateBack(translate);
		}
	}
}