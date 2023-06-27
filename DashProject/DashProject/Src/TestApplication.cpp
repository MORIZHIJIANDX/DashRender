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

		/*
		std::string path{ "Src\\ovra.png" };
		LOG_INFO << "Is Path : " << FFileUtility::IsPath(path);
		LOG_INFO << "Is File : " << FFileUtility::IsFile(path);
		LOG_INFO << "Is Path Exists : " << FFileUtility::IsPathExistent(path);
		LOG_INFO << "Absolute Path : " << FFileUtility::GetAbsolutePath(path);
		LOG_INFO << "RemoveBasePath Path : " << FFileUtility::RemoveBasePath(path);
		LOG_INFO << "BasePath : " << FFileUtility::GetBasePath(path);
		LOG_INFO << "Extension : " << FFileUtility::GetFileExtension(path);
		LOG_INFO << "Last write time : " << FFileUtility::GetFileLastWriteTime(path);
		LOG_INFO << "Current Path : " << FFileUtility::GetCurrentPath();
		LOG_INFO << "File Name : " << FFileUtility::GetFileName(path);
		LOG_INFO << "Relative Path : " << FFileUtility::GetRelativePath(FFileUtility::GetAbsolutePath(path));
		LOG_INFO << "Parent Path : " << FFileUtility::GetParentPath(FFileUtility::GetAbsolutePath(path));
		*/

		//std::string projectpath = std::string(PROJECT_PATH);

		//std::string projectpath = FFileUtility::GetCurrentPath();

		//LOG_INFO << "Project Path : " << projectpath;

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

	void TestApplication::OnRenderScene(const FRenderEventArgs& e)
	{
		FRenderEventArgs Args = e;
		Args.Camera = mCamera; 
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