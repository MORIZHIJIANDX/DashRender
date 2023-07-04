#include "PCH.h"
#include "GameApp.h"
#include "GraphicsCore.h"
#include "SwapChain.h"
#include "Utility/Keyboard.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"

#include "ModelLoader/StaticMeshLoader.h"

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

	void IGameApp::Startup(void)
	{
		FImportedStaticMeshData importedStaticMeshData;

		std::string fbxMeshPath = std::string(ENGINE_PATH) + "/Resource/Cyborg_Weapon.fbx";

		bool result = LoadStaticMeshFromFile(fbxMeshPath, importedStaticMeshData);

		if (result)
		{
			FStaticMeshData<FMeshVertexTypePNTU> meshData = importedStaticMeshData.GetMeshData<FMeshVertexTypePNTU>();

			LOG_INFO << "Load mesh succeed!";
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(mWindowHandle);
		ImGui_ImplDX12_Init_Refactoring(3);
	}

	void IGameApp::Cleanup(void)
	{
		// Cleanup
		ImGui_ImplDX12_Shutdown_Refactoring();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void IGameApp::BeginFrame()
	{
		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame_Refactoring();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		bool show_demo_window = false;
		bool show_another_window = true;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
	}

	void IGameApp::EndFrame()
	{
		
	}

	void IGameApp::OnRenderUI(const FRenderEventArgs& e)
	{
		// Our state
		bool show_demo_window = false;
		bool show_another_window = true;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		{
			// Rendering
			ImGui::Render();

			FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("RenderUI");

			graphicsContext.SetRenderTarget(FGraphicsCore::SwapChain->GetCurrentBackBuffer());
			graphicsContext.ClearColor(FGraphicsCore::SwapChain->GetCurrentBackBuffer());

			ImGui_ImplDX12_RenderDrawData_Refactoring(ImGui::GetDrawData(), graphicsContext);

			graphicsContext.Finish();
		}
	}

	bool IGameApp::ProcessWinMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	}

	void IGameApp::Present()
	{
		FGraphicsCore::SwapChain->Present();
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

