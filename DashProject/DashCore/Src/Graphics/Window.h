#pragma once

#include "../Utility/Events.h"
#include "../DesignPatterns/Singleton.h"

namespace Dash
{
	class FWindow
	{
	public:
		FWindow(const std::string& name, const std::string title, size_t width = 1080, size_t height = 720);
		virtual ~FWindow();

		const std::string& GetWindowName() const noexcept { return FWindowClassRegister::Get()->GetWindowClassName(); }

		size_t GetWindowWidth() const noexcept { return mWindowWidth; }
		size_t GetWindowHeight() const noexcept { return mWindowHeight; }

		virtual void ShowWindow() noexcept;
		virtual void HideWindow() noexcept;
		virtual void CloseWindow() noexcept;

		void SetTitle(const std::string& title) noexcept;
		virtual void SetFullScreen(bool fullScreen) noexcept;

		bool IsFullScreen() const noexcept;

		int WindowsMessageLoop() noexcept;

		void ProcessMessage() noexcept;

		HWND GetWindowHandle() noexcept { return mWindowHandle; }

	public:
		FResizeEvent WindowResize;

		void OnWindowResize(FResizeEventArgs& args);

	private:

		static LRESULT CALLBACK WinProcFunc(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK HandleMsgThunk(HWND, UINT, WPARAM, LPARAM);
		LRESULT HandleMsg(HWND, UINT, WPARAM, LPARAM) noexcept;

		class FWindowClassRegister : public TSingleton<FWindowClassRegister>
		{
		public:
			HINSTANCE GetWindowInstance() const noexcept;
			const std::string& GetWindowClassName() const noexcept;

			FWindowClassRegister();
			FWindowClassRegister(const std::string& windowName, HINSTANCE windowInstance);
			~FWindowClassRegister();
			
			FWindowClassRegister(FWindowClassRegister&) = delete;
			FWindowClassRegister& operator=(const FWindowClassRegister&) = delete;

		private:

			std::string mWindowClassName;
			HINSTANCE mWindowInstance;
		};

		HWND mWindowHandle;

		size_t mWindowWidth;
		size_t mWindowHeight;

		std::string mWindowTitle;

		bool mFullScreen;
		bool mRequestQuit;
	};
}
