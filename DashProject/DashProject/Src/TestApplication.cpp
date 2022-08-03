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

		Camera = std::make_shared<FPerspectiveCamera>();
		Camera->SetCameraParams(aspect, fov, 0.1f, 100.0f);
		Camera->SetPosition(FVector3f{ 0.0f, 0.0f, -2.0f });

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
		Scalar Translate = Scalar(Speed * e.mElapsedTime);

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::A))
		{
			Camera->TranslateLeft(Translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::D))
		{
			Camera->TranslateRight(Translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::W))
		{
			Camera->TranslateForward(Translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::S))
		{
			Camera->TranslateBack(Translate);
		}
	}

	void TestApplication::OnRenderScene(const FRenderEventArgs& e)
	{
		FRenderEventArgs Args = e;
		Args.mCamera = Camera;
		Graphics::OnRender(Args);
	}

	void TestApplication::OnMouseWheelDown(FMouseWheelEventArgs& e)
	{
		Camera->TranslateForward(0.001f * e.mWheelDelta);
		LOG_INFO << "OnMouseWheelDown -- ";
	}

	void TestApplication::OnMouseWheelUp(FMouseWheelEventArgs& e)
	{
		Camera->TranslateForward(0.001f * e.mWheelDelta);
		LOG_INFO << "OnMouseWheelUp ++ ";
	}

	void TestApplication::OnMouseMove(FMouseMotionEventArgs& e)
	{
		float RotationSpeed = 0.1f;
		if (FMouse::Get().GetButtonState(EMouseButton::Left).Pressed)
		{
			Camera->AddPitch(e.mRelY * RotationSpeed);
			Camera->AddYaw(e.mRelX * RotationSpeed);
		}
	}
}