#include "PCH/PCH.h"

#include "TestApplication.h"
#include "Utility/LogManager.h"
#include "Utility/Keyboard.h"
#include "Utility/Mouse.h"

#include "Graphics/GraphicsCore.h"


#include <string>

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

		Scalar Translate = Scalar(Speed * e.mElapsedTime);

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
	}

	void TestApplication::OnRenderScene(const FRenderEventArgs& e)
	{
		FRenderEventArgs Args = e;
		Args.mCamera = mCamera;
		//Graphics::OnRender(Args);
	}

	void TestApplication::OnWindowResize(const FResizeEventArgs& e)
	{
		mWindowWidth = e.mWidth;
		mWindowHeight = e.mHeight;

		float aspect = mWindowWidth / (float)mWindowHeight;
		float fov = 45.0f;
		mCamera->SetCameraParams(aspect, fov, 0.1f, 100.0f);

		//Graphics::OnResize(e);

		LOG_INFO << "Window Resized, Wdith : " << e.mWidth << ", Height : " << e.mHeight << " , Minimized : " << e.mMinimized;
	}

	void TestApplication::OnMouseWheelDown(FMouseWheelEventArgs& e)
	{
		mCamera->TranslateForward(0.001f * e.mWheelDelta);
	}

	void TestApplication::OnMouseWheelUp(FMouseWheelEventArgs& e)
	{
		mCamera->TranslateForward(0.001f * e.mWheelDelta);
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