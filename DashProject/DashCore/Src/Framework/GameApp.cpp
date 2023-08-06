#include "PCH.h"
#include "GameApp.h"
//#include "GraphicsCore.h"
//#include "SwapChain.h"

#include "Graphics/GraphicsCore.h"
#include "Graphics/SwapChain.h"

#include "Utility/Keyboard.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"

#include "ModelLoader/StaticMeshLoader.h"
#include "Utility/FileUtility.h"
#include "Camera.h"

#include "SceneRenderLayer.h"
#include "UIRenderLayer.h"


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

	void IGameApp::Startup()
	{
		AddRenderLayer(std::make_unique<FSceneRenderLayer>());
		AddRenderLayer(std::make_unique<FUIRenderLayer>());
	}

	void IGameApp::Cleanup()
	{
		for (uint32_t i = 0; i < mRenderLayers.size(); i++)
		{
			mRenderLayers[i]->Shutdown();
		}

		mRenderLayers.clear();
	}

	bool IGameApp::AddRenderLayer(std::shared_ptr<IRenderLayer> layer)
	{
		if (layer == nullptr)
		{
			return false;
		}

		for (uint32_t i = 0; i < mRenderLayers.size(); i++)
		{
			if (mRenderLayers[i]->GetLayerId() == layer->GetLayerId() ||
				mRenderLayers[i]->GetLayerName() == layer->GetLayerName())
			{
				return false;
			}
		}

		layer->Init();
		mRenderLayers.push_back(layer);

		std::sort(mRenderLayers.begin(), mRenderLayers.end(), [](const std::shared_ptr<IRenderLayer>& a, const std::shared_ptr<IRenderLayer>& b){
			return a->GetLayerId() < b->GetLayerId();
		});

		return true;
	}

	bool IGameApp::RemoveRenderLayer(const std::string& layerName)
	{
		for (auto iter = mRenderLayers.begin(); iter < mRenderLayers.end(); ++iter)
		{
			if (iter->get()->GetLayerName() == layerName)
			{
				iter->get()->Shutdown();
				mRenderLayers.erase(iter);

				std::sort(mRenderLayers.begin(), mRenderLayers.end(), [](const std::shared_ptr<IRenderLayer>& a, const std::shared_ptr<IRenderLayer>& b) {
					return a->GetLayerId() < b->GetLayerId();
					});
				return true;
			}
		}

		return false;
	}

	void IGameApp::OnBeginFrame()
	{
		for (uint32_t i = 0; i < mRenderLayers.size(); i++)
		{
			mRenderLayers[i]->OnBeginFrame();
		}
	}

	void IGameApp::OnEndFrame()
	{
		for (uint32_t i = 0; i < mRenderLayers.size(); i++)
		{
			mRenderLayers[i]->OnEndFrame();
		}

		FGraphicsCore::SwapChain->Present();
	}

	void IGameApp::OnUpdate(const FUpdateEventArgs& e)
	{
		for (uint32_t i = 0; i < mRenderLayers.size(); i++)
		{
			mRenderLayers[i]->OnUpdate(e);
		}
	}

	void IGameApp::OnRender(const FRenderEventArgs& e)
	{
		for (uint32_t i = 0; i < mRenderLayers.size(); i++)
		{
			mRenderLayers[i]->OnRender(e);
		}
	}

	void IGameApp::OnWindowResize(const FResizeEventArgs& e)
	{
		mWindowWidth = e.Width;
		mWindowHeight = e.Height;

		for (uint32_t i = 0; i < mRenderLayers.size(); i++)
		{
			mRenderLayers[i]->OnWindowResize(e);
		}

		FGraphicsCore::SwapChain->OnWindowResize(mWindowWidth, mWindowHeight);
	}

	bool IGameApp::ProcessWinMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
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

