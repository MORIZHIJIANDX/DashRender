#include "PCH/PCH.h"

#include "TestApplication.h"
#include "Utility/LogManager.h"
#include "Utility/Keyboard.h"

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

		LOG_INFO << "Startup";
	}

	void TestApplication::Cleanup(void)
	{
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
			Camera->TranslateUp(Translate);
		}

		if (FKeyboard::Get().IsKeyPressed(EKeyCode::S))
		{
			Camera->TranslateDown(Translate);
		}
	}

	void TestApplication::OnRenderScene(const FRenderEventArgs& e)
	{
		FRenderEventArgs Args = e;
		Args.mCamera = Camera;
		Graphics::OnRender(Args);
	}
}