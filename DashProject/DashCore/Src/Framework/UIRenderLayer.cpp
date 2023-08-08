#include "PCH.h"
#include "UIRenderLayer.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/SwapChain.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"
#include "GameApp.h"

namespace Dash
{
	FUIRenderLayer::FUIRenderLayer()
		: IRenderLayer("UIRenderLayer", 65535)
	{

	}

	FUIRenderLayer::~FUIRenderLayer()
	{
	}

	void FUIRenderLayer::Init()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(IGameApp::GetInstance()->GetWindowHandle());
		ImGui_ImplDX12_Init_Refactoring(3);
	}

	void FUIRenderLayer::Shutdown()
	{
		// Cleanup
		ImGui_ImplDX12_Shutdown_Refactoring();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void FUIRenderLayer::OnBeginFrame()
	{
		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame_Refactoring();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void FUIRenderLayer::OnEndFrame()
	{
	}

	void FUIRenderLayer::OnUpdate(const FUpdateEventArgs& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		ImGui::End();
	}

	void FUIRenderLayer::OnRender(const FRenderEventArgs& e)
	{
		ImGui::Render();

		FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("RenderUI");

		graphicsContext.SetRenderTarget(FGraphicsCore::SwapChain->GetCurrentBackBuffer());

		ImGui_ImplDX12_RenderDrawData_Refactoring(ImGui::GetDrawData(), graphicsContext);

		graphicsContext.Finish();
	}

	void FUIRenderLayer::OnWindowResize(const FResizeEventArgs& e)
	{
	}
}
