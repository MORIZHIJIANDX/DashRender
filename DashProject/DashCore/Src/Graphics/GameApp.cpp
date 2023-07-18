#include "PCH.h"
#include "GameApp.h"
#include "GraphicsCore.h"
#include "SwapChain.h"
#include "Utility/Keyboard.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"

#include "ModelLoader/StaticMeshLoader.h"
#include "Utility/FileUtility.h"

namespace Dash
{
	FGpuVertexBufferRef PositionVertexBuffer;
	FGpuVertexBufferRef NormalVertexBuffer;
	FGpuVertexBufferRef UVVertexBuffer;
	FGpuIndexBufferRef IndexBuffer;
	FShaderPassRef MeshDrawPass;
	FGraphicsPSORef MeshDrawPSO = FGraphicsPSO::MakeGraphicsPSO("MeshDrawPSO");;

	bool show_demo_window = false;
	bool show_another_window = true;

	IGameApp* IGameApp::mAppInstance = nullptr;

	IGameApp::IGameApp(UINT width, UINT height, const std::string& title, const std::string& winClassName)
		: mWindowHandle(NULL)
		, mWindowWidth(width)
		, mWindowHeight(height)
		, mWindowTitle(title)
		, mWindowClassName(winClassName)
	{
	}

	void CreateVertexBuffers(const FImportedStaticMeshData& meshData)
	{
		const uint32_t numVertexes = meshData.numVertexes;
		if (numVertexes > 0)
		{
			PositionVertexBuffer = FGraphicsCore::Device->CreateVertexBuffer("MeshPositionVertexBuffer", numVertexes, sizeof(meshData.PositionData[0]), meshData.PositionData.data());
			NormalVertexBuffer = FGraphicsCore::Device->CreateVertexBuffer("MeshNormalVertexBuffer", numVertexes, sizeof(meshData.NormalData[0]), meshData.NormalData.data());
			UVVertexBuffer = FGraphicsCore::Device->CreateVertexBuffer("MeshUVVertexBuffer", numVertexes, sizeof(meshData.UVData[0]), meshData.UVData.data());

			IndexBuffer = FGraphicsCore::Device->CreateIndexBuffer("MeshIndexBuffer", numVertexes, meshData.indices.data(), true);
		}
		
		FShaderCreationInfo psPresentInfo{ EShaderStage::Pixel, FFileUtility::GetEngineShaderDir("MeshShader.hlsl"),  "PS_Main" };

		FShaderCreationInfo vsInfo{ EShaderStage::Vertex,FFileUtility::GetEngineShaderDir("MeshShader.hlsl"),  "VS_Main" };

		FRasterizerState rasterizerDefault{ ERasterizerFillMode::Solid, ERasterizerCullMode::None };

		FBlendState blendDisable{ false, false };
		FDepthStencilState depthStateDisabled{ false, false };

		FShaderPassRef meshDrawPass = FShaderPass::MakeShaderPass("PresentPass", { vsInfo , psPresentInfo }, blendDisable, rasterizerDefault, depthStateDisabled);

		MeshDrawPSO->SetShaderPass(meshDrawPass);
		MeshDrawPSO->SetPrimitiveTopologyType(EPrimitiveTopology::TriangleList);
		MeshDrawPSO->SetSamplerMask(UINT_MAX);
		MeshDrawPSO->SetRenderTargetFormat(FGraphicsCore::SwapChain->GetColorBuffer()->GetFormat(), EResourceFormat::Depth32_Float);
		MeshDrawPSO->Finalize();
	}

	void RenderMesh(FGraphicsCommandContext& graphicsContext)
	{
		FResourceMagnitude RenderTargetMagnitude = FGraphicsCore::SwapChain->GetColorBuffer()->GetDesc().Magnitude;

		FGpuVertexBufferRef vertexBuffers[3] = { PositionVertexBuffer ,NormalVertexBuffer, UVVertexBuffer };

		graphicsContext.SetGraphicsPipelineState(MeshDrawPSO);

		//graphicsContext.SetRootConstantBufferView("PerObjectBuffer", vertex_constant_buffer);

		graphicsContext.SetViewportAndScissor(0, 0, RenderTargetMagnitude.Width, RenderTargetMagnitude.Height);

		graphicsContext.SetVertexBuffers(0, 3, vertexBuffers);

		graphicsContext.SetIndexBuffer(IndexBuffer);

		graphicsContext.DrawIndexed(IndexBuffer->GetElementCount());
	}

	void ReleaseVertexBuffers()
	{
		if (IndexBuffer)
		{
			IndexBuffer->Destroy();
		}

		if (PositionVertexBuffer)
		{
			PositionVertexBuffer->Destroy();
		}

		if(NormalVertexBuffer)
		{
			NormalVertexBuffer->Destroy();
		}

		if (UVVertexBuffer)
		{
			UVVertexBuffer->Destroy();
		}
	}

	void IGameApp::Startup()
	{
		FImportedStaticMeshData importedStaticMeshData;

		std::string fbxMeshPath = std::string(ENGINE_PATH) + "/Resource/Cyborg_Weapon.fbx";

		bool result = LoadStaticMeshFromFile(fbxMeshPath, importedStaticMeshData);

		if (result)
		{
			importedStaticMeshData.hasNormal;

			CreateVertexBuffers(importedStaticMeshData);

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

	void IGameApp::Cleanup()
	{
		ReleaseVertexBuffers();

		// Cleanup
		ImGui_ImplDX12_Shutdown_Refactoring();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void IGameApp::BeginFrame(FGraphicsCommandContext& graphicsContext)
	{
		graphicsContext.SetRenderTarget(FGraphicsCore::SwapChain->GetColorBuffer());
		graphicsContext.ClearColor(FGraphicsCore::SwapChain->GetColorBuffer());

		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame_Refactoring();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

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

	void IGameApp::EndFrame(FGraphicsCommandContext& graphicsContext)
	{
		Present(graphicsContext);
	}

	void IGameApp::OnRenderUI(const FRenderEventArgs& e, FGraphicsCommandContext& graphicsContext)
	{
		//return;

		// Our state
		bool show_demo_window = false;
		bool show_another_window = true;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		{
			// Rendering
			ImGui::Render();

			//FGraphicsCommandContext& graphicsContext = FGraphicsCommandContext::Begin("RenderUI");

			ImGui_ImplDX12_RenderDrawData_Refactoring(ImGui::GetDrawData(), graphicsContext);
		}
	}

	bool IGameApp::ProcessWinMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	}

	void IGameApp::Present(FGraphicsCommandContext& graphicsContext)
	{
		FGraphicsCore::SwapChain->Present(graphicsContext);
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

