#include "PCH.h"
#include "GameApp.h"

#include "Utility/Keyboard.h"

namespace Dash
{
	IGameApp* IGameApp::mAppInstance = nullptr;

	IGameApp::IGameApp(UINT width, UINT height, const std::string& title, const std::string& winClassName)
		: mWindowHandle(NULL)
		, mWindowWidth(width)
		, mWindowHeight(height)
		, mWindowTitle(title)
		, mWindowClassName(winClassName)
	{
	}

	bool IGameApp::IsDone(void)
	{
		return FKeyboard::Get().IsKeyPressed(EKeyCode::Escape);
	}

	void IGameApp::SetWindowTitle(const std::string& title)
	{
		mWindowTitle = title;
		ASSERT(mWindowHandle != nullptr);

		::SetWindowText(mWindowHandle, title.c_str());
	}

	std::string IGameApp::GetWindowClassName() const
	{
		return mWindowClassName;
	}

}

