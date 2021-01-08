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
		LOG_INFO << "Startup";
	}

	void TestApplication::Cleanup(void)
	{
		LOG_INFO << "Cleanup";
	}

	void TestApplication::OnUpdate(const FUpdateEventArgs& e)
	{
		if (FKeyboard::Get().IsKeyPressed(EKeyCode::A))
		{
			this->SetWindowTitle("A Pressed");
		}
	}

	void TestApplication::OnRenderScene(const FRenderEventArgs& e)
	{
		Graphics::OnRender(e);
	}
}