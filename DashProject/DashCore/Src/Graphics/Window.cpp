#include "PCH.h"

#include "Window.h"

#include <functional>

#include "../Utility/ThreadSafeQueue.h"
#include "../Utility/Keyboard.h"
#include "../Utility/Mouse.h"


namespace Dash
{
	using MessageFunc = std::function<void()>;
	TThreadSafeQueue<MessageFunc> MessageQueue;

	FWindow::FWindow(const std::string& name, const std::string title, size_t width, size_t height)
		: mWindowTitle(title)
		, mWindowWidth(width)
		, mWindowHeight(height)
		, mRequestQuit(false)
	{
		FWindow::FWindowClassRegister::Get(name, ::GetModuleHandle(NULL));
		
		RECT winRect;
		winRect.left = 100;
		winRect.top = 100;
		winRect.right = winRect.left + (UINT)width;
		winRect.bottom = winRect.top + (UINT)height;

		::AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);

		mWindowHandle = CreateWindow(
			GetWindowName().c_str(),
			title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<int>(winRect.right - winRect.left), 
			static_cast<int>(winRect.bottom - winRect.top),
			nullptr,      
			nullptr,       
			FWindow::FWindowClassRegister::Get()->GetWindowInstance(),
			this);

		ShowWindow();
	}

	FWindow::~FWindow()
	{
		::DestroyWindow(mWindowHandle);
	}

	void FWindow::ShowWindow() noexcept
	{
		ASSERT(mWindowHandle != nullptr);

		::ShowWindow(mWindowHandle, SW_SHOWDEFAULT);
	}

	void FWindow::HideWindow() noexcept
	{
		ASSERT(mWindowHandle != nullptr);

		::ShowWindow(mWindowHandle, SW_HIDE);
	}

	void FWindow::CloseWindow() noexcept
	{
		mRequestQuit = true;
	}

	void FWindow::SetTitle(const std::string& title) noexcept
	{
		ASSERT(mWindowHandle != nullptr);

		::SetWindowText(mWindowHandle, title.c_str());
	}

	void FWindow::SetFullScreen(bool fullScreen) noexcept
	{
		if (mFullScreen != fullScreen)
		{
			mFullScreen = fullScreen;
		}
	}

	bool FWindow::IsFullScreen() const noexcept
	{
		return mFullScreen;
	}

	int FWindow::WindowsMessageLoop() noexcept
	{
		MSG msg = {};

		while (true)
		{
			while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if ((msg.message == WM_QUIT) || (mRequestQuit == true))
				{
					mRequestQuit = false;
					return (int)msg.wParam;
				}

				::TranslateMessage(&msg);
				::DispatchMessageA(&msg);
			}

			std::this_thread::yield();
		}
	}

	void FWindow::ProcessMessage() noexcept
	{
		MessageFunc func;
		while (MessageQueue.TryPop(func))
		{
			func();
		}
	}

	// Convert the message ID into a EMouseButton ID
	static EMouseButton DecodeMouseButton(UINT messageID)
	{
		EMouseButton mouseButton = EMouseButton::None;
		switch (messageID)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		{
			mouseButton = EMouseButton::Left;
			break;
		}
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		{
			mouseButton = EMouseButton::Right;
			break;
		}
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		{
			mouseButton = EMouseButton::Middle;
			break;
		}
		}

		return mouseButton;
	}

	void FWindow::OnWindowResize(FResizeEventArgs& args)
	{
		if (args.mWidth != mWindowWidth || args.mHeight != mWindowHeight)
		{
			WindowResize(args);

			mWindowWidth = args.mWidth;
			mWindowHeight = args.mHeight;
		}
	}

	LRESULT FWindow::WinProcFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_CREATE)
		{
			// extract ptr to window class from creation data
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			FWindow* const pWnd = static_cast<FWindow*>(pCreate->lpCreateParams);
			// set WinAPI-managed user data to store ptr to window instance
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			// set message proc to normal (non-setup) handler now that setup is finished
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&FWindow::HandleMsgThunk));
			// forward message to window instance handler
			return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	LRESULT FWindow::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// retrieve ptr to window instance
		FWindow* const pWnd = reinterpret_cast<FWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		// forward message to window instance handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	LRESULT FWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		switch (msg)
		{
			// we don't want the DefProc to handle this message because
			// we want our destructor to destroy the window, so return 0 instead of break
		case WM_CLOSE:
			::PostQuitMessage(0);
			return 0;
			// clear keystate when window loses focus to prevent input getting "stuck"
		case WM_KILLFOCUS:
			MessageQueue.Push(std::bind(&FKeyboard::ClearStates, &FKeyboard::Get()));
			break;
		case WM_SIZE:
		{
			RECT clientRect = {};
			GetClientRect(hWnd, &clientRect);

			FResizeEventArgs windowResizeArgs{ clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED };

			MessageQueue.Push(std::bind(&FWindow::OnWindowResize, this, windowResizeArgs));
			break;
		}			
		/*********** KEYBOARD MESSAGES ***********/
		case WM_KEYDOWN:
			// syskey commands need to be handled to track ALT key (VK_MENU) and F10
		case WM_SYSKEYDOWN:
		{	
			MSG charMsg;

			// Get the Unicode character (UTF-16)
			unsigned int c = 0;
			// For printable characters, the next message will be WM_CHAR.
			// This message contains the character code we need to send the KeyPressed event.
			// Inspired by the SDL 1.2 implementation.
			if (PeekMessage(&charMsg, hWnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR)
			{
				GetMessage(&charMsg, hWnd, 0, 0);
				c = static_cast<unsigned int>(charMsg.wParam);
			}

			bool repeat = lParam & 0x40000000;

			bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
			bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

			EKeyCode key = (EKeyCode)wParam;
			unsigned int scanCode = (lParam & 0x00FF0000) >> 16;
			FKeyEventArgs keyEventArgs(key, c, EKeyState::Pressed, control, shift, alt, repeat);
			MessageQueue.Push(std::bind(&FKeyboard::OnKeyPressed, &FKeyboard::Get(), keyEventArgs));
			break;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
			bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

			EKeyCode key = (EKeyCode)wParam;

			FKeyEventArgs keyEventArgs(key, 0, EKeyState::Released, control, shift, alt, false);
			MessageQueue.Push(std::bind(&FKeyboard::OnKeyReleased, &FKeyboard::Get(), keyEventArgs));
			break;
		}
			/*********** END KEYBOARD MESSAGES ***********/

			/************* MOUSE MESSAGES ****************/
		case WM_MOUSEMOVE:
		{
			bool lButton = (wParam & MK_LBUTTON) != 0;
			bool rButton = (wParam & MK_RBUTTON) != 0;
			bool mButton = (wParam & MK_MBUTTON) != 0;
			bool shift = (wParam & MK_SHIFT) != 0;
			bool control = (wParam & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			FMouseMotionEventArgs mouseMotionEventArgs(lButton, mButton, rButton, control, shift, x, y);

			// in client region -> log move, and log enter + capture mouse (if not previously in window)
			if (x >= 0 && x < mWindowWidth && y >= 0 && y < mWindowHeight)
			{
				MessageQueue.Push(std::bind(&FMouse::OnMouseMove, &FMouse::Get(), mouseMotionEventArgs));

				if (!FMouse::Get().IsInWindow())
				{
					SetCapture(hWnd);
					MessageQueue.Push(std::bind(&FMouse::OnMouseEnter, &FMouse::Get(), mouseMotionEventArgs));
				}
			}
			// not in client -> log move / maintain capture if button down
			else
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					MessageQueue.Push(std::bind(&FMouse::OnMouseMove, &FMouse::Get(), mouseMotionEventArgs));
				}
				// button up -> release capture / log event for leaving
				else
				{
					ReleaseCapture();
					MessageQueue.Push(std::bind(&FMouse::OnMouseLeave, &FMouse::Get(), mouseMotionEventArgs));
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		{
			bool lButton = (wParam & MK_LBUTTON) != 0;
			bool rButton = (wParam & MK_RBUTTON) != 0;
			bool mButton = (wParam & MK_MBUTTON) != 0;
			bool shift = (wParam & MK_SHIFT) != 0;
			bool control = (wParam & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			FMouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(msg), EButtonState::Pressed, lButton, mButton, rButton, control, shift, x, y);
			MessageQueue.Push(std::bind(&FMouse::OnMouseButtonPressed, &FMouse::Get(), mouseButtonEventArgs));
			break;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		{
			bool lButton = (wParam & MK_LBUTTON) != 0;
			bool rButton = (wParam & MK_RBUTTON) != 0;
			bool mButton = (wParam & MK_MBUTTON) != 0;
			bool shift = (wParam & MK_SHIFT) != 0;
			bool control = (wParam & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			FMouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(msg), EButtonState::Released, lButton, mButton, rButton, control, shift, x, y);
			MessageQueue.Push(std::bind(&FMouse::OnMouseButtonReleased, &FMouse::Get(), mouseButtonEventArgs));
			break;
		}
		case WM_MOUSEWHEEL:
		{
			const int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			short keyStates = (short)LOWORD(wParam);

			bool lButton = (keyStates & MK_LBUTTON) != 0;
			bool rButton = (keyStates & MK_RBUTTON) != 0;
			bool mButton = (keyStates & MK_MBUTTON) != 0;
			bool shift = (keyStates & MK_SHIFT) != 0;
			bool control = (keyStates & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			// Convert the screen coordinates to client coordinates.
			POINT clientToScreenPoint;
			clientToScreenPoint.x = x;
			clientToScreenPoint.y = y;
			::ScreenToClient(hWnd, &clientToScreenPoint);

			FMouseWheelEventArgs mouseWheelEventArgs((float)zDelta, lButton, mButton, rButton, control, shift, (int)clientToScreenPoint.x, (int)clientToScreenPoint.y);
			MessageQueue.Push(std::bind(&FMouse::OnMouseWheel, &FMouse::Get(), mouseWheelEventArgs));
			break;
		}
		/************** END MOUSE MESSAGES **************/
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	HINSTANCE FWindow::FWindowClassRegister::GetWindowInstance() const noexcept
	{
		return mWindowInstance;
	}

	const std::string& FWindow::FWindowClassRegister::GetWindowClassName() const noexcept
	{
		return mWindowClassName;
	}

	FWindow::FWindowClassRegister::FWindowClassRegister()
		: mWindowClassName()
		, mWindowInstance(nullptr)
	{
	}

	FWindow::FWindowClassRegister::FWindowClassRegister(const std::string& windowName, HINSTANCE windowInstance)
		: mWindowClassName(windowName)
		, mWindowInstance(windowInstance)
	{
		WNDCLASSEX winClass;
		ZeroMemory(&winClass, sizeof(WNDCLASSEX));
		
		winClass.cbSize = sizeof(winClass);
		winClass.style = CS_HREDRAW | CS_VREDRAW;
		winClass.lpfnWndProc = FWindow::WinProcFunc;
		winClass.cbClsExtra = 0;
		winClass.cbWndExtra = 0;
		winClass.hInstance = mWindowInstance;
		winClass.hIcon = nullptr;
		winClass.hCursor = nullptr;
		//winClass.hbrBackground = nullptr;
		winClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		winClass.lpszMenuName = nullptr;
		winClass.lpszClassName = mWindowClassName.c_str();
		winClass.hIconSm = nullptr;

		RegisterClassEx(&winClass);
	}

	FWindow::FWindowClassRegister::~FWindowClassRegister()
	{
		UnregisterClass(mWindowClassName.c_str(), mWindowInstance);
	}
}