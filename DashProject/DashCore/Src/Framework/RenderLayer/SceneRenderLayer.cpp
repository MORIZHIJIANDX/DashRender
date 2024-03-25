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
#include "Asset/AssetManager.h"

#include "Graphics/GPUProfiler.h"

namespace Dash
{
	struct FMeshConstantBuffer
	{
		FMatrix4x4 ModelViewProjectionMatrix{ FIdentity{} };
	};

	struct FInstanceDataType
	{
		FVector4f Color;
		FMatrix4x4 ModelMatrix{ FIdentity{} };
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

		std::string fbxMeshPath = std::string(ENGINE_PATH) + "/Resource/M4A1_1P_Main.FBX";

		float fov = 45.0f;
		float aspect = IGameApp::GetInstance()->GetWindowWidth() / (float)IGameApp::GetInstance()->GetWindowHeight();

		mCameraActor = std::make_shared<TCameraActor>("PerspectiveCameraActor", ECameraType::Perspective);
		mPerspectiveCamera = dynamic_cast<TPerspectiveCameraComponent*>(mCameraActor->GetCameraComponent());
		mPerspectiveCamera->SetCameraParams(aspect, fov, 0.1f, 100.0f);
		mPerspectiveCamera->SetWorldPosition(FVector3f{ 0.0f, 0.0f, -5.0f });

		FStaticMeshRef staticMesh = FAssetManager::Get().MakeStaticMesh(fbxMeshPath);

		FShaderTechniqueRef shaderTech = FShaderTechnique::MakeShaderTechnique("default");
		shaderTech->AddShaderPass("opaque",{ 
		{ EShaderStage::Vertex,FFileUtility::GetEngineShaderDir("MeshShader.hlsl"),  "VS_Main" } , 
		{ EShaderStage::Pixel, FFileUtility::GetEngineShaderDir("MeshShader.hlsl"),  "PS_Main" } 
		},
		FBlendState{ false, false },
		FRasterizerState{ ERasterizerFillMode::Solid, ERasterizerCullMode::None },
		FDepthStencilState{true});

		FMaterialRef material = FAssetManager::Get().MakeMaterial("material", shaderTech);
		staticMesh->SetMaterial("MI_M4A1", material);
		mStaticMeshActor = std::make_shared<TStaticMeshActor>("StaticMesh");
		mStaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(staticMesh);
		mStaticMeshActor->SetActorWorldScale(FVector3f{0.01f, 0.01f, 0.01f});
		mStaticMeshActor->SetActorWorldPosition(FVector3f{ 0.0f, 0.0f, 0.0f });
		mStaticMeshActor->SetActorWorldRotation(FQuaternion{ FVector3f{ 1.0f, 0.0f, 0.0f }, 90.0f });

		FTextureRef baseColorTexture = FAssetManager::Get().MakeTexture(std::string(ENGINE_PATH) + "/Resource/T_M4A1_D.TGA");

		material->SetVector4Parameter("Color", FVector4f{1.0f, 0.f, 0.0f, 0.0f});
		material->SetTextureParameter("BaseColorTexture", baseColorTexture);

		FTransform Instance1Transform{ FIdentity{} };
		Instance1Transform.SetScale(FVector3f{ 0.01f, 0.01f, 0.01f });
		Instance1Transform.SetPosition(FVector3f{ 0.0f, 0.0f, 0.0f });

		FTransform Instance2Transform { FIdentity{} };
		Instance2Transform.SetScale(FVector3f{ 0.01f, 0.01f, 0.01f });
		Instance2Transform.SetPosition(FVector3f{1.0f, 1.0f, 1.0f});

		std::vector<FInstanceDataType> InstanceData;
		InstanceData.emplace_back(FVector4f{ 1.0f, 0.0f, 0.0f, 1.0f }, Instance1Transform.GetMatrix());
		InstanceData.emplace_back(FVector4f{ 0.0f, 1.0f, 0.0f, 1.0f }, Instance2Transform.GetMatrix());

		mInstanceBuffer = FGraphicsCore::Device->CreateStructuredBuffer<FInstanceDataType>("InstanceBuffer", 2, InstanceData.data());

		std::vector<uint32_t> MatrixInstanceIDBuffer;
		MatrixInstanceIDBuffer.emplace_back(0);
		MatrixInstanceIDBuffer.emplace_back(1);
		mMatrixInstanceBuffer = FGraphicsCore::Device->CreateVertexBuffer<uint32_t>("MatrixInstanceIDBuffer", (uint32_t)MatrixInstanceIDBuffer.size(), MatrixInstanceIDBuffer.data());

		std::vector<uint32_t> ColorInstanceIDBuffer;
		ColorInstanceIDBuffer.emplace_back(1);
		ColorInstanceIDBuffer.emplace_back(0);
		mColorInstanceBuffer = FGraphicsCore::Device->CreateVertexBuffer<uint32_t>("ColorInstanceIDBuffer", (uint32_t)ColorInstanceIDBuffer.size(), ColorInstanceIDBuffer.data());

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
		
		//MeshConstantBuffer.ModelViewProjectionMatrix = mStaticMeshActor->GetActorWorldTransform().GetMatrix() * mPerspectiveCamera->GetViewProjectionMatrix();

		TStaticMeshComponent* staticMeshComponent = mStaticMeshActor->GetStaticMeshComponent();
		if (staticMeshComponent)
		{
			Scalar timeSin = static_cast<Scalar>(FMath::Abs(FMath::Sin(e.TotalTime)));
			staticMeshComponent->GetMaterial("MI_M4A1")->SetVector4Parameter("Color", FVector4f{ 1.0f, 1.0f, 1.0f, 1.0f });
		}
	}

	void FSceneRenderLayer::OnRender(const FRenderEventArgs& e)
	{
		FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("RenderScene");

		graphicsContext.SetRenderTarget(FGraphicsCore::SwapChain->GetCurrentBackBuffer(), FGraphicsCore::SwapChain->GetDepthBuffer());
		graphicsContext.ClearColor(FGraphicsCore::SwapChain->GetCurrentBackBuffer());
		graphicsContext.ClearDepth(FGraphicsCore::SwapChain->GetDepthBuffer());

		{
			FGPUProfilerScope testProfile(graphicsContext, "RenderScene");

			FResourceMagnitude renderTargetMagnitude = FGraphicsCore::SwapChain->GetCurrentBackBuffer()->GetDesc().Magnitude;

			graphicsContext.SetViewportAndScissor(0, 0, renderTargetMagnitude.Width, renderTargetMagnitude.Height);

			TStaticMeshComponent* staticMeshComponent = mStaticMeshActor->GetStaticMeshComponent();
			if (staticMeshComponent)
			{
				const std::vector<FMeshDrawCommand>& meshDrawCommands = staticMeshComponent->GetMeshDrawCommands();
				for (auto& drawCommand : meshDrawCommands)
				{
					graphicsContext.SetGraphicsPipelineState(drawCommand.PSO);

					graphicsContext.SetRootConstantBufferView("FrameConstantBuffer", mPerspectiveCamera->GetViewProjectionMatrix());
					 //graphicsContext.SetRootConstantBufferView("ObjectConstantBuffer", mStaticMeshActor->GetActorWorldTransform().GetMatrix());

					for (auto& cbvParameter : *drawCommand.ConstantBufferMapPtr)
					{
						graphicsContext.SetRootConstantBufferView(cbvParameter.first, cbvParameter.second.size(), cbvParameter.second.data());
					}

					for (auto& srvParameter : *drawCommand.TextureBufferMapPtr)
					{
						graphicsContext.SetShaderResourceView(srvParameter.first, srvParameter.second->GetTextureBuffer());
					}

					graphicsContext.SetShaderResourceView("InstanceData", mInstanceBuffer);

					std::vector<FGpuVertexBufferRef> RealVertexBuffer = drawCommand.VertexBuffers;
					RealVertexBuffer.push_back(mMatrixInstanceBuffer);
					//RealVertexBuffer.push_back(mColorInstanceBuffer);

					//graphicsContext.SetVertexBuffers(0, static_cast<UINT>(drawCommand.VertexBuffers.size()), drawCommand.VertexBuffers.data());

					graphicsContext.SetVertexBuffers(0, static_cast<UINT>(RealVertexBuffer.size()), RealVertexBuffer.data());

					graphicsContext.SetIndexBuffer(drawCommand.IndexBuffer);

					//graphicsContext.DrawIndexed(drawCommand.IndexCount, drawCommand.IndexStart, drawCommand.VertexStart);
					graphicsContext.DrawIndexedInstanced(drawCommand.IndexCount, 2, drawCommand.IndexStart, drawCommand.VertexStart, 0);
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