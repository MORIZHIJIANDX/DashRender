#pragma once

#include "Graphics/CommandContext.h"
#include "RenderLayer/RenderLayer.h"

class FRenderEventArgs;
class FUpdateEventArgs;
//class IRenderLayer;

namespace Dash
{
	class IGameApp
	{
    friend void CreateApplicationWindow(IGameApp* app, HINSTANCE hInstance);

	public:
        IGameApp(UINT width = 1080, UINT height = 720, const std::string& title = "Sample", const std::string& winClassName = "DashGameApp");
        virtual ~IGameApp() {};

        static IGameApp* GetInstance() { return  mAppInstance; }

        virtual void Startup();
        virtual void Cleanup();
        
        bool AddRenderLayer(std::shared_ptr<IRenderLayer> layer);
        bool RemoveRenderLayer(const std::string& layerName);

        virtual void OnBeginFrame();
        virtual void OnEndFrame();

        virtual void OnUpdate(const FUpdateEventArgs& e);
        virtual void OnRender(const FRenderEventArgs& e);

        virtual void OnWindowResize(const FWindowResizeEventArgs& e);
        virtual void OnWindowMoved(const FWindowMoveEventArgs& e);

        bool ProcessWinMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

        virtual bool IsDone();

        HWND GetWindowHandle() const { return mWindowHandle; }

		void SetWindowWidth(int width) { mWindowWidth = width; }
        void SetWindowHeight(int height) { mWindowHeight = height; }

        int GetWindowWidth() const { return mWindowWidth; }
        int GetWindowHeight() const { return mWindowHeight; }
        
        void SetWindowBounds(int left, int top, int right, int bottom);

        void SetWindowTitle(const std::string& title);
        std::string GetWindowTitle() const { return mWindowTitle; }

        std::string GetWindowClassName() const;

    private:
        void SetWindowHandle(HWND handle) { mWindowHandle = handle; }

    protected:
        static IGameApp* mAppInstance;

        HWND mWindowHandle;
        int mWindowWidth;
        int mWindowHeight;
        std::string mWindowTitle;
        std::string mWindowClassName;

        std::vector<std::shared_ptr<IRenderLayer>> mRenderLayers;
	};
}