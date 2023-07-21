#include "PCH.h"
#include "EntryPoint.h"
#include "Utility/Events.h"
#include "Utility/Keyboard.h"
#include "Utility/Mouse.h"
#include "Utility/SystemTimer.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/CommandContext.h"

namespace Dash
{
	bool AppPaused = false;
	bool Minimized = false;
	bool Maximized = false;
	bool Resizing = false;

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void CreateApplicationWindow(IGameApp* app, HINSTANCE hInstance)
	{
		IGameApp::mAppInstance = app;
		std::string WindowClassName = app->GetWindowClassName();

		// Initialize the window class.
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = WindowClassName.c_str();
		RegisterClassEx(&windowClass);

		RECT windowRect = { 0, 0, static_cast<LONG>(app->GetWindowWidth()), static_cast<LONG>(app->GetWindowHeight()) };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		HWND hwnd = CreateWindow(
			windowClass.lpszClassName,
			app->GetWindowTitle().c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,        // We have no parent window.
			nullptr,        // We aren't using menus.
			hInstance,
			app);

		ASSERT(hwnd != 0);

		app->SetWindowHandle(hwnd);
	}

	void InitializeApplication(IGameApp* app)
	{
		::CoInitialize(nullptr);

		FLogManager::Get()->Init();

		FMouse::Get().Initialize(app->GetWindowHandle());
		FSystemTimer::Initialize();

		FGraphicsCore::Initialize();

		app->Startup();

		::ShowWindow(app->GetWindowHandle(), SW_SHOWDEFAULT);
		::UpdateWindow(app->GetWindowHandle());
	}

	void TerminateApplication(IGameApp* app)
	{
		FGraphicsCore::Shutdown();
		app->Cleanup();
		FLogManager::Get()->Shutdown();

		std::string WindowClassName = app->GetWindowClassName();
		::UnregisterClassA(WindowClassName.c_str(),
			(HINSTANCE)::GetModuleHandle(NULL));
	}

	void UpdateApplication(IGameApp* app, size_t& frameCount, FCpuTimer& timer)
	{
		timer.Tick();
		float deltaTime = timer.GetDeltaTime();
		float totalTime = timer.GetTotalTime();

		FUpdateEventArgs updateArgs{ deltaTime, totalTime, frameCount};
		FRenderEventArgs RenderArgs{ deltaTime, totalTime, frameCount };

		if (!Minimized)
		{
			FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("RenderFrame");

			app->BeginFrame(graphicsContext);

			app->OnUpdate(updateArgs);

			app->OnRenderScene(RenderArgs, graphicsContext);

			app->OnRenderUI(RenderArgs, graphicsContext);

			app->EndFrame(graphicsContext);
		}

		++frameCount;
	}

	int RunApplication(IGameApp* app)
	{
		FCpuTimer timer;
		timer.Reset();

		size_t frameCount = 0;

		MSG msg = {0};

		while (!(msg.message == WM_QUIT || app->IsDone()))
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				UpdateApplication(app, frameCount, timer);
			}
		}

		return (int)msg.wParam;
	}

	EMouseButton DecodeMouseButton(UINT messageID)
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

	LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		IGameApp* app = reinterpret_cast<IGameApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		if (app->ProcessWinMessage(hWnd, message, wParam, lParam))
		{
			return true;
		}

		switch (message)
		{
		case WM_CREATE:
		{
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_KILLFOCUS:
		{
			FKeyboard::Get().ClearStates();
			break;
		}
		case WM_SIZE:
		{
			if (app == nullptr)
			{
				return 0;
			}

			// Save the new client area dimensions.
			int ClientWidth = LOWORD(lParam);
			int ClientHeight = HIWORD(lParam);

			LOG_INFO << "Window Event, Wdith : " << ClientWidth << ", Height : " << ClientHeight;

			//if (app)
			{
				app->SetWindowWidth(ClientWidth);
				app->SetWindowHeight(ClientHeight);

				if (wParam == SIZE_MINIMIZED)
				{
					AppPaused = true;
					Minimized = true;
					Maximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					AppPaused = false;
					Minimized = false;
					Maximized = true;
					app->OnWindowResize(FResizeEventArgs{ app->GetWindowWidth(), app->GetWindowHeight(), Minimized });
				}
				else if (wParam == SIZE_RESTORED)
				{

					// Restoring from minimized state?
					if (Minimized)
					{
						AppPaused = false;
						Minimized = false;
						app->OnWindowResize(FResizeEventArgs{ app->GetWindowWidth(), app->GetWindowHeight(), Minimized });
					}

					// Restoring from maximized state?
					else if (Maximized)
					{
						AppPaused = false;
						Maximized = false;
						app->OnWindowResize(FResizeEventArgs{ app->GetWindowWidth(), app->GetWindowHeight(), Minimized });
					}
					else if (Resizing)
					{
						// If user is dragging the resize bars, we do not resize 
						// the buffers here because as the user continuously 
						// drags the resize bars, a stream of WM_SIZE messages are
						// sent to the window, and it would be pointless (and slow)
						// to resize for each WM_SIZE message received from dragging
						// the resize bars.  So instead, we reset after the user is 
						// done resizing the window and releases the resize bars, which 
						// sends a WM_EXITSIZEMOVE message.
					}
					else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
					{
						app->OnWindowResize(FResizeEventArgs{ app->GetWindowWidth(), app->GetWindowHeight(), Minimized });
					}
				}
			}
			//break;

			return 0;
		}
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
		{
			AppPaused = true;
			Resizing = true;
			//mTimer.Stop();
			//break;
			return 0;
		}
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
		{
			AppPaused = false;
			Resizing = false;
			//mTimer.Start();
			app->OnWindowResize(FResizeEventArgs{ app->GetWindowWidth(), app->GetWindowHeight(), Minimized });
			//break;
			return 0;
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
			FKeyboard::Get().OnKeyPressed(keyEventArgs);
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
			FKeyboard::Get().OnKeyReleased(keyEventArgs);
			break;
		}
		/************* MOUSE MESSAGES ****************/
		case WM_MOUSEMOVE:
		{
			if (app == nullptr)
			{
				return 0;
			}

			bool lButton = (wParam & MK_LBUTTON) != 0;
			bool rButton = (wParam & MK_RBUTTON) != 0;
			bool mButton = (wParam & MK_MBUTTON) != 0;
			bool shift = (wParam & MK_SHIFT) != 0;
			bool control = (wParam & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			FMouseMotionEventArgs mouseMotionEventArgs(lButton, mButton, rButton, control, shift, x, y);

			int windowWidth = static_cast<int>(app->GetWindowWidth());
			int windowHeight = static_cast<int>(app->GetWindowHeight());

			// in client region -> log move, and log enter + capture mouse (if not previously in window)
			if (x >= 0 && x < windowWidth && y >= 0 && y < windowHeight)
			{
				FMouse::Get().OnMouseMove(mouseMotionEventArgs);

				if (!FMouse::Get().IsInWindow())
				{
					SetCapture(hWnd);
					FMouse::Get().OnMouseEnter(mouseMotionEventArgs);
				}
			}
			// not in client -> log move / maintain capture if button down
			else
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					FMouse::Get().OnMouseMove(mouseMotionEventArgs);
				}
				// button up -> release capture / log event for leaving
				else
				{
					ReleaseCapture();
					FMouse::Get().OnMouseLeave(mouseMotionEventArgs);
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

			FMouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), EButtonState::Pressed, lButton, mButton, rButton, control, shift, x, y);
			FMouse::Get().OnMouseButtonPressed(mouseButtonEventArgs);
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

			FMouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(message), EButtonState::Released, lButton, mButton, rButton, control, shift, x, y);
			FMouse::Get().OnMouseButtonReleased(mouseButtonEventArgs);
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
			FMouse::Get().OnMouseWheel(mouseWheelEventArgs);
			break;
		}
		/************** END MOUSE MESSAGES **************/
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		return 0;
	}
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	Dash::IGameApp* app = CreateApplication();

	Dash::CreateApplicationWindow(app, hInstance);

	Dash::InitializeApplication(app);

	int retureCode = Dash::RunApplication(app);
	Dash::TerminateApplication(app);

	delete app;

	return retureCode;
}
