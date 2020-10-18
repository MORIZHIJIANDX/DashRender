#include "PCH.h"
#include "EntryPoint.h"
#include "Utility/HighResolutionTimer.h"

namespace Dash
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void CreateApplicationWindow(IGameApp* app, HINSTANCE hInstance)
	{
		WNDCLASSEX windowClass;
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = hInstance;
		windowClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowClass.lpszMenuName = nullptr;
		windowClass.lpszClassName = "DashGameApp";
		windowClass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
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
		FLogManager::Get()->Init();
		FLogManager::Get()->RegisterLogStream(std::make_shared<FLogStreamConsole>());
		app->Startup();
	}

	void TerminateApplication(IGameApp* app)
	{
		app->Cleanup();
		FLogManager::Get()->Shutdown();
	}

	bool UpdateApplication(IGameApp* app, size_t& frameCount)
	{
		//app->OnUpdate();

		++frameCount;

		return !app->IsDone();
	}

	void RunApplication(IGameApp* app)
	{
		FHighResolutionTimer timer;

		size_t frameCount = 0;

		do
		{
			MSG msg = {};
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (msg.message == WM_QUIT)
				break;
		} while (UpdateApplication(app, frameCount));    // Returns false to quit loop
	}

	LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		return 0;
	}
}

int CALLBACK WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	Dash::IGameApp* app = CreateApplication();

	Dash::CreateApplicationWindow(app, hInstance);

	Dash::InitializeApplication(app);

	::ShowWindow(app->GetWindowHandle(), SW_SHOWDEFAULT);

	Dash::RunApplication(app);
	Dash::TerminateApplication(app);

	delete app;
}
