#pragma once

class FRenderEventArgs;
class FUpdateEventArgs;

namespace Dash
{
	class IGameApp
	{
    friend void CreateApplicationWindow(IGameApp* app, HINSTANCE hInstance);

	public:
        IGameApp(UINT width = 1080, UINT height = 720, const std::string& title = "Sample");
        virtual ~IGameApp() {};

        static IGameApp* GetInstance() { return  mAppInstance; }

        virtual void Startup(void) = 0;
        virtual void Cleanup(void) = 0;

        virtual void OnUpdate(const FUpdateEventArgs& e) = 0;

        virtual void OnRenderScene(const FRenderEventArgs& e) = 0;

        virtual void OnRenderUI(const FRenderEventArgs& e) {};

        virtual void OnWindowResize(const FResizeEventArgs& e) {};

        virtual bool IsDone(void);

        HWND GetWindowHandle() const { return mWindowHandle; }

        UINT GetWindowWidth() const { return mWindowWidth; }
        UINT GetWindowHeight() const { return mWindowHeight; }
        std::string GetWindowTitle() const { return mWindowTitle; }

        void SetWindowTitle(const std::string& title);

    private:
        void SetWindowHandle(HWND handle) { mWindowHandle = handle; }

    protected:
        static IGameApp* mAppInstance;

    private:
        HWND mWindowHandle;
        UINT mWindowWidth;
        UINT mWindowHeight;
        std::string mWindowTitle;
	};
}