#include "PCH.h"
#include "GameApp.h"

#include "Graphics/GraphicsCore.h"
#include "Graphics/SwapChain.h"

#include "Utility/Keyboard.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"

#include "Framework/RenderLayer/SceneRenderLayer.h"
#include "Framework/RenderLayer/UIRenderLayer.h"
#include "Framework/RenderLayer/PostProcessRenderLayer.h"

#include "Graphics/GPUProfiler.h"

#include "Graphics/ShaderPreprocesser.h"
#include "Utility/FileUtility.h"

namespace Dash
{
	IGameApp* IGameApp::mAppInstance = nullptr;

	IGameApp::IGameApp(uint32 width, uint32 height, const std::string& title, const std::string& winClassName)
		: mWindowHandle(NULL)
		, mWindowWidth(width)
		, mWindowHeight(height)
		, mWindowTitle(title)
		, mWindowClassName(winClassName)
	{
	}

	void IGameApp::Startup()
	{
		{
			FShaderPreprocessdResult result = FShaderPreprocesser::Process(FFileUtility::GetEngineShaderDir("TestBindlessPS.hlsl"));

			std::string processedShaderPass = FFileUtility::GetEngineShaderDir("TestBindlessPSAfter.hlsl");

			if (!std::filesystem::exists(processedShaderPass))
			{
				std::filesystem::create_directory(std::filesystem::path(processedShaderPass).parent_path());
			}

			std::ofstream file(processedShaderPass);

			if (!file)
			{
				ASSERT(false);
			}

			file << result.ShaderCode;
			file.close();
		}

		AddRenderLayer(std::make_unique<FSceneRenderLayer>());
		AddRenderLayer(std::make_unique<FPostProcessRenderLayer>());
		AddRenderLayer(std::make_unique<FUIRenderLayer>());
	}

	void IGameApp::Cleanup()
	{
		for (uint32 i = 0; i < mRenderLayers.size(); i++)
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

		for (uint32 i = 0; i < mRenderLayers.size(); i++)
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
		for (uint32 i = 0; i < mRenderLayers.size(); i++)
		{
			mRenderLayers[i]->OnBeginFrame();
		}
	}

	void IGameApp::OnEndFrame()
	{
		for (uint32 i = 0; i < mRenderLayers.size(); i++)
		{
			mRenderLayers[i]->OnEndFrame();
		}

		FGraphicsCore::SwapChain->Present();
	}

	void IGameApp::OnUpdate(const FUpdateEventArgs& e)
	{
		FGraphicsCore::Profiler->NewFrame();

		for (uint32 i = 0; i < mRenderLayers.size(); i++)
		{
			mRenderLayers[i]->OnUpdate(e);
		}
	}

	void IGameApp::OnRender(const FRenderEventArgs& e)
	{
		for (uint32 i = 0; i < mRenderLayers.size(); i++)
		{
			mRenderLayers[i]->OnRender(e);
		}
	}

	void IGameApp::OnWindowResize(const FWindowResizeEventArgs& e)
	{
		mWindowWidth = e.Width;
		mWindowHeight = e.Height;

		FGraphicsCore::SwapChain->OnWindowResize(mWindowWidth, mWindowHeight);

		for (uint32 i = 0; i < mRenderLayers.size(); i++)
		{
			mRenderLayers[i]->OnWindowResize(e);
		}
	}

	void IGameApp::OnWindowMoved(const FWindowMoveEventArgs& e)
	{
		FGraphicsCore::SwapChain->OnWindowMoved(e.XPos, e.YPos);
	}

	bool IGameApp::ProcessWinMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	}

	bool IGameApp::IsDone(void)
	{
		return FKeyboard::Get().IsKeyPressed(EKeyCode::Escape);
	}

	void IGameApp::SetWindowBounds(int left, int top, int right, int bottom)
	{
		FGraphicsCore::SwapChain->SetWindowBounds(left, top, right, bottom);
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

