#pragma once

//#include "CommandContext.h"

#include "Graphics/CommandContext.h"

class FRenderEventArgs;
class FUpdateEventArgs;

namespace Dash
{
	class IGameApp
	{
    friend void CreateApplicationWindow(IGameApp* app, HINSTANCE hInstance);

	public:
        IGameApp(UINT width = 1080, UINT height = 720, const std::string& title = "Sample", const std::string& winClassName = "DashGameApp");
        virtual ~IGameApp() {};

        static IGameApp* GetInstance() { return  mAppInstance; }

        virtual void Startup(void);
        virtual void Cleanup(void);
        
        virtual void BeginFrame(FGraphicsCommandContext& graphicsContext);
        virtual void EndFrame(FGraphicsCommandContext& graphicsContext);

        virtual void OnUpdate(const FUpdateEventArgs& e) = 0;

        virtual void OnRenderScene(const FRenderEventArgs& e, FGraphicsCommandContext& graphicsContext);

        virtual void OnRenderUI(const FRenderEventArgs& e, FGraphicsCommandContext& graphicsContext);

        virtual void OnWindowResize(const FResizeEventArgs& e) = 0;

        bool ProcessWinMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        void Present(FGraphicsCommandContext& graphicsContext);

        virtual bool IsDone(void);

        HWND GetWindowHandle() const { return mWindowHandle; }

		void SetWindowWidth(int width) { mWindowWidth = width; }
        void SetWindowHeight(int height) { mWindowHeight = height; }

        int GetWindowWidth() const { return mWindowWidth; }
        int GetWindowHeight() const { return mWindowHeight; }
        
        void SetWindowTitle(const std::string& title);
        std::string GetWindowTitle() const { return mWindowTitle; }

        std::string GetWindowClassName() const;

    private:
        void SetWindowHandle(HWND handle) { mWindowHandle = handle; }

    protected:
        static IGameApp* mAppInstance;

    //private:
        HWND mWindowHandle;
        int mWindowWidth;
        int mWindowHeight;
        std::string mWindowTitle;
        std::string mWindowClassName;
	};
}