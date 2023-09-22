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
#include "MeshLoader/StaticMeshLoader.h"
#include "Framework/GameApp.h"
#include "imgui/imgui.h"

#include "Asset/StaticMesh.h"
#include "Framework/Component/StaticMeshComponent.h"

namespace Dash
{
	struct FMeshConstantBuffer
	{
		FMatrix4x4 ModelViewProjectionMatrix{ FIdentity{} };
	};

	FMeshConstantBuffer MeshConstantBuffer;

	FSceneRenderLayer::FSceneRenderLayer()
		: IRenderLayer("SceneRenderLayer", 100)
		, mCameraActor(nullptr)
		, mPerspectiveCamera(nullptr)
	{
	}

	FSceneRenderLayer::~FSceneRenderLayer()
	{
	}

	void FSceneRenderLayer::Init()
	{
		FImportedStaticMeshData importedStaticMeshData;

		std::string fbxMeshPath = std::string(ENGINE_PATH) + "/Resource/Cyborg_Weapon.fbx";

		float fov = 45.0f;
		float aspect = IGameApp::GetInstance()->GetWindowWidth() / (float)IGameApp::GetInstance()->GetWindowHeight();

		mCameraActor = std::make_shared<TCameraActor>("PerspectiveCameraActor", ECameraType::Perspective);
		mPerspectiveCamera = dynamic_cast<TPerspectiveCameraComponent*>(mCameraActor->GetCameraComponent());
		mPerspectiveCamera->SetCameraParams(aspect, fov, 0.1f, 100.0f);
		mPerspectiveCamera->SetWorldPosition(FVector3f{ 0.0f, 0.0f, -2.0f });

		FStaticMeshRef staticMesh = FStaticMesh::MakeStaticMesh(fbxMeshPath);

		FShaderTechniqueRef shaderTech = FShaderTechnique::MakeShaderTechnique("default");
		shaderTech->AddShaderPass("opaque",{ 
		{ EShaderStage::Vertex,FFileUtility::GetEngineShaderDir("MeshShader.hlsl"),  "VS_Main" } , 
		{ EShaderStage::Pixel, FFileUtility::GetEngineShaderDir("MeshShader.hlsl"),  "PS_Main" } 
		},
		FBlendState{ false, false },
		FRasterizerState{ ERasterizerFillMode::Solid, ERasterizerCullMode::None },
		FDepthStencilState{true});

		FMaterialRef material = FMaterial::MakeMaterial("material", shaderTech);
		staticMesh->SetMaterial("Cyborg_Weapon_mat", material);
		mStaticMeshActor = std::make_shared<TStaticMeshActor>("StaticMesh");
		mStaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(staticMesh);

		material->SetVector4Parameter("Color", FVector4f{1.0f, 0.f, 0.0f, 0.0f});

		OnMouseWheelDownDelegate = FMouseWheelEventDelegate::Create<FSceneRenderLayer, &FSceneRenderLayer::OnMouseWheelDown>(this);
		OnMouseWheelUpDelegate = FMouseWheelEventDelegate::Create<FSceneRenderLayer, &FSceneRenderLayer::OnMouseWheelUp>(this);
		OnMouseMoveDelegate = FMouseMotionEventDelegate::Create<FSceneRenderLayer, &FSceneRenderLayer::OnMouseMove>(this);

		FMouse::Get().MouseWheelDown += OnMouseWheelDownDelegate;
		FMouse::Get().MouseWheelUp += OnMouseWheelUpDelegate;
		FMouse::Get().MouseMoved += OnMouseMoveDelegate;
	}

	void FSceneRenderLayer::Shutdown()
	{
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
		
		MeshConstantBuffer.ModelViewProjectionMatrix = mPerspectiveCamera->GetViewProjectionMatrix();

		TStaticMeshComponent* staticMeshComponent = mStaticMeshActor->GetStaticMeshComponent();
		if (staticMeshComponent)
		{
			Scalar timeSin = FMath::Abs(FMath::Sin(e.TotalTime));
			staticMeshComponent->GetMaterial("Cyborg_Weapon_mat")->SetVector4Parameter("Color", FVector4f{ timeSin, timeSin, timeSin, 1.0f });
		}
	}

	void FSceneRenderLayer::OnRender(const FRenderEventArgs& e)
	{
		FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("RenderScene");

		graphicsContext.SetRenderTarget(FGraphicsCore::SwapChain->GetCurrentBackBuffer(), FGraphicsCore::SwapChain->GetDepthBuffer());
		graphicsContext.ClearColor(FGraphicsCore::SwapChain->GetCurrentBackBuffer());
		graphicsContext.ClearDepth(FGraphicsCore::SwapChain->GetDepthBuffer());

		{
			TStaticMeshComponent* staticMeshComponent = mStaticMeshActor->GetStaticMeshComponent();
			if (staticMeshComponent)
			{
				const std::vector<FMeshDrawCommand>& meshDrawCommands = staticMeshComponent->GetMeshDrawCommands();
				for (auto& drawCommand : meshDrawCommands)
				{
					FResourceMagnitude renderTargetMagnitude = FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetDesc().Magnitude;

					graphicsContext.SetGraphicsPipelineState(drawCommand.PSO);

					graphicsContext.SetRootConstantBufferView("FrameConstantBuffer", MeshConstantBuffer);

					for (auto& cbvParameter : drawCommand.ConstantBufferMap)
					{
						graphicsContext.SetRootConstantBufferView(cbvParameter.first, cbvParameter.second.get()->size(), cbvParameter.second.get()->data());
					}

					for (auto& srvParameter : drawCommand.TextureBufferMap)
					{
						graphicsContext.SetShaderResourceView(srvParameter.first, srvParameter.second->GetTextureBuffer());
					}

					graphicsContext.SetViewportAndScissor(0, 0, renderTargetMagnitude.Width, renderTargetMagnitude.Height);

					graphicsContext.SetVertexBuffers(0, 3, drawCommand.VertexBuffers.data());

					graphicsContext.SetIndexBuffer(drawCommand.IndexBuffer);

					graphicsContext.DrawIndexed(drawCommand.vertexCount);
				}
			}
		}

		graphicsContext.Finish();
	}

	void FSceneRenderLayer::OnWindowResize(const FResizeEventArgs& e)
	{
		float aspect = e.Width / (float)e.Height;
		float fov = 45.0f;
		mPerspectiveCamera->SetCameraParams(aspect, fov, 0.1f, 100.0f);
	}

	void FSceneRenderLayer::OnMouseWheelDown(FMouseWheelEventArgs& e)
	{
		mPerspectiveCamera->TranslateForward(0.001f * e.WheelDelta);
	}

	void FSceneRenderLayer::OnMouseWheelUp(FMouseWheelEventArgs& e)
	{
		mPerspectiveCamera->TranslateForward(0.001f * e.WheelDelta);
	}

	void FSceneRenderLayer::OnMouseMove(FMouseMotionEventArgs& e)
	{
		float RotationSpeed = 0.1f;
		if (FMouse::Get().GetButtonState(EMouseButton::Right).Pressed)
		{
			mPerspectiveCamera->AddPitch(e.mRelY * RotationSpeed);
			mPerspectiveCamera->AddYaw(e.mRelX * RotationSpeed);
		}
	}

	void FSceneRenderLayer::UpdateCamera(Scalar translate)
	{
		if (FKeyboard::Get().IsKeyPressed(EKeyCode::A))
		{
			mPerspectiveCamera->TranslateLeft(translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::D))
		{
			mPerspectiveCamera->TranslateRight(translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::W))
		{
			mPerspectiveCamera->TranslateForward(translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::S))
		{
			mPerspectiveCamera->TranslateBack(translate);
		}
	}
}