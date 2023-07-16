#include "PCH/PCH.h"

#include "TestApplication.h"
#include "Utility/LogManager.h"
#include "Utility/Keyboard.h"
#include "Utility/Mouse.h"

#include "Graphics/GraphicsCore.h"


#include <string>
#include "Graphics/SwapChain.h"
#include "Graphics/CommandContext.h"

#include "Utility/FileUtility.h"
#include "Graphics/ShaderMap.h"

namespace Dash
{
	TestApplication::TestApplication()
		: IGameApp()
	{
	}

	TestApplication::~TestApplication()
	{
	}

	void TestApplication::Startup(void)
	{
		IGameApp::Startup();

		float fov = 45.0f;
		float aspect = IGameApp::GetInstance()->GetWindowWidth() / (float)IGameApp::GetInstance()->GetWindowHeight();

		mCamera = std::make_shared<FPerspectiveCamera>();
		mCamera->SetCameraParams(aspect, fov, 0.1f, 100.0f);
		mCamera->SetPosition(FVector3f{ 0.0f, 0.0f, -2.0f });

		OnMouseWheelDownDelegate = FMouseWheelEventDelegate::Create<TestApplication, &TestApplication::OnMouseWheelDown>(this);
		OnMouseWheelUpDelegate = FMouseWheelEventDelegate::Create<TestApplication, &TestApplication::OnMouseWheelUp>(this);
		OnMouseMoveDelegate = FMouseMotionEventDelegate::Create<TestApplication, &TestApplication::OnMouseMove>(this);

		FMouse::Get().MouseWheelDown += OnMouseWheelDownDelegate;
		FMouse::Get().MouseWheelUp += OnMouseWheelUpDelegate;
		FMouse::Get().MouseMoved += OnMouseMoveDelegate;

		LOG_INFO << "Startup";
	}

	void TestApplication::Cleanup(void)
	{
		IGameApp::Cleanup();

		FMouse::Get().MouseWheelDown -= OnMouseWheelDownDelegate;
		FMouse::Get().MouseWheelUp -= OnMouseWheelUpDelegate;
		FMouse::Get().MouseMoved -= OnMouseMoveDelegate;

		LOG_INFO << "Cleanup";
	}
	 
	void TestApplication::OnUpdate(const FUpdateEventArgs& e)
	{
		Scalar Speed = 1.0f;

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::ShiftKey))
		{
			Speed *= 2.0f;
		}

		Scalar Translate = Scalar(Speed * e.ElapsedTime);

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::A))
		{
			mCamera->TranslateLeft(Translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::D))
		{
			mCamera->TranslateRight(Translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::W))
		{
			mCamera->TranslateForward(Translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::S))
		{
			mCamera->TranslateBack(Translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::K))
		{
			FGraphicsCore::SwapChain->SetDisplayRate(0.05f);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::L))
		{
			FGraphicsCore::SwapChain->SetDisplayRate(2.0f);
		}
	}

	void TestApplication::OnRenderScene(const FRenderEventArgs& e, FGraphicsCommandContext& graphicsContext)
	{
		//IGameApp::OnRenderScene(e, graphicsContext);
	}

	void TestApplication::OnWindowResize(const FResizeEventArgs& e)
	{
		mWindowWidth = e.Width;
		mWindowHeight = e.Height;

		float aspect = mWindowWidth / (float)mWindowHeight;
		float fov = 45.0f;
		mCamera->SetCameraParams(aspect, fov, 0.1f, 100.0f);

		FGraphicsCore::SwapChain->OnWindowResize(mWindowWidth, mWindowHeight);

		LOG_INFO << "Window Resized, width : " << e.Width << ", Height : " << e.Height << " , Minimized : " << e.Minimized;
	}

	void TestApplication::OnMouseWheelDown(FMouseWheelEventArgs& e)
	{
		mCamera->TranslateForward(0.001f * e.WheelDelta);
	}

	void TestApplication::OnMouseWheelUp(FMouseWheelEventArgs& e)
	{
		mCamera->TranslateForward(0.001f * e.WheelDelta);
	}

	void TestApplication::OnMouseMove(FMouseMotionEventArgs& e)
	{
		float RotationSpeed = 0.1f;
		if (FMouse::Get().GetButtonState(EMouseButton::Left).Pressed)
		{
			mCamera->AddPitch(e.mRelY * RotationSpeed);
			mCamera->AddYaw(e.mRelX * RotationSpeed);
		}
	}
}