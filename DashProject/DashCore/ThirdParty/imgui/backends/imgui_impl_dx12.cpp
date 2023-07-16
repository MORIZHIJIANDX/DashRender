// dear imgui: Renderer Backend for DirectX12
// This needs to be used along with a Platform Backend (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'D3D12_GPU_DESCRIPTOR_HANDLE' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bit indices.

// Important: to compile on 32-bit systems, this backend requires code to be compiled with '#define ImTextureID ImU64'.
// This is because we need ImTextureID to carry a 64-bit value and by default ImTextureID is defined as void*.
// To build this on 32-bit systems:
// - [Solution 1] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'ImTextureID=ImU64' (this is what we do in the 'example_win32_direct12/example_win32_direct12.vcxproj' project file)
// - [Solution 2] IDE/msbuild: in "Properties/C++/Preprocessor Definitions" add 'IMGUI_USER_CONFIG="my_imgui_config.h"' and inside 'my_imgui_config.h' add '#define ImTextureID ImU64' and as many other options as you like.
// - [Solution 3] IDE/msbuild: edit imconfig.h and add '#define ImTextureID ImU64' (prefer solution 2 to create your own config file!)
// - [Solution 4] command-line: add '/D ImTextureID=ImU64' to your cl.exe command-line (this is what we do in the example_win32_direct12/build_win32.bat file)

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2021-06-29: Reorganized backend to pull data from a single structure to facilitate usage with multiple-contexts (all g_XXXX access changed to bd->XXXX).
//  2021-05-19: DirectX12: Replaced direct access to ImDrawCmd::TextureId with a call to ImDrawCmd::GetTexID(). (will become a requirement)
//  2021-02-18: DirectX12: Change blending equation to preserve alpha in output buffer.
//  2021-01-11: DirectX12: Improve Windows 7 compatibility (for D3D12On7) by loading d3d12.dll dynamically.
//  2020-09-16: DirectX12: Avoid rendering calls with zero-sized scissor rectangle since it generates a validation layer warning.
//  2020-09-08: DirectX12: Clarified support for building on 32-bit systems by redefining ImTextureID.
//  2019-10-18: DirectX12: *BREAKING CHANGE* Added extra ID3D12DescriptorHeap parameter to ImGui_ImplDX12_Init() function.
//  2019-05-29: DirectX12: Added support for large mesh (64K+ vertices), enable ImGuiBackendFlags_RendererHasVtxOffset flag.
//  2019-04-30: DirectX12: Added support for special ImDrawCallback_ResetRenderState callback to reset render state.
//  2019-03-29: Misc: Various minor tidying up.
//  2018-12-03: Misc: Added #pragma comment statement to automatically link with d3dcompiler.lib when using D3DCompile().
//  2018-11-30: Misc: Setting up io.BackendRendererName so it can be displayed in the About Window.
//  2018-06-12: DirectX12: Moved the ID3D12GraphicsCommandList* parameter from NewFrame() to RenderDrawData().
//  2018-06-08: Misc: Extracted imgui_impl_dx12.cpp/.h away from the old combined DX12+Win32 example.
//  2018-06-08: DirectX12: Use draw_data->DisplayPos and draw_data->DisplaySize to setup projection matrix and clipping rectangle (to ease support for future multi-viewport).
//  2018-02-22: Merged into master with all Win32 code synchronized to other examples.


#include "PCH.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/PipelineStateObject.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/SwapChain.h"
#include "Utility/FileUtility.h"

//#include "imgui.h"
#include "imgui/imgui.h"
#include "imgui_impl_dx12.h"

// DirectX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler") // Automatically link with d3dcompiler.lib as we are using D3DCompile() below.
#endif

struct VERTEX_CONSTANT_BUFFER_DX12
{
    float   mvp[4][4];
};

// =======================================================================================================================

namespace Dash
{
    struct ImGui_VertexDataBuffer
    {
        std::vector<FVector2f> Positions;
        std::vector<FVector2f> UVs;
        std::vector<FColor> Colors;

        void Reserve(int32_t size)
        {
            Positions.reserve(size);
            UVs.reserve(size);
            Colors.reserve(size);
        }

        void AddVertex(const FVector2f& position, const FVector2f& uv, const FColor& color)
        {
            Positions.emplace_back(position);
            UVs.emplace_back(uv);
            Colors.emplace_back(color);
        }
    };

    struct ImGui_ImplDX12_RenderBuffers_Refactoring
    {
        FGpuDynamicIndexBufferRef IndexBuffer;

        FGpuDynamicVertexBufferRef VertexPositonBuffer;
        FGpuDynamicVertexBufferRef VertexUVBuffer;
        FGpuDynamicVertexBufferRef VertexColorBuffer;

        int                 IndexBufferSize;
        int                 VertexBufferSize;
    };

    struct ImGui_ImplDX12_Data_Refactoring
    {
        FGraphicsPSORef pPipelineState;
        FTextureBufferRef pFontTextureResource;
        ImGui_ImplDX12_RenderBuffers_Refactoring* pFrameResources;
        UINT                            numFramesInFlight;
        UINT                            frameIndex;

        ImGui_ImplDX12_Data_Refactoring() { memset((void*)this, 0, sizeof(*this)); frameIndex = UINT_MAX; }
    };

    static ImGui_ImplDX12_Data_Refactoring* ImGui_ImplDX12_GetBackendData_Refactoring()
    {
        return ImGui::GetCurrentContext() ? (ImGui_ImplDX12_Data_Refactoring*)ImGui::GetIO().BackendRendererUserData : NULL;
    }

    bool ImGui_ImplDX12_Init_Refactoring(int num_frames_in_flight)
    {
        ImGuiIO& io = ImGui::GetIO();
        IM_ASSERT(io.BackendRendererUserData == NULL && "Already initialized a renderer backend!");

        // Setup backend capabilities flags
        ImGui_ImplDX12_Data_Refactoring* bd = IM_NEW(ImGui_ImplDX12_Data_Refactoring)();
        io.BackendRendererUserData = (void*)bd;
        io.BackendRendererName = "imgui_impl_dx12";
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.

        bd->pFrameResources = new ImGui_ImplDX12_RenderBuffers_Refactoring[num_frames_in_flight];
        bd->numFramesInFlight = num_frames_in_flight;
        bd->frameIndex = UINT_MAX;

        // Create buffers with a default size (they will later be grown as needed)
        for (int i = 0; i < num_frames_in_flight; i++)
        {
            ImGui_ImplDX12_RenderBuffers_Refactoring* fr = &bd->pFrameResources[i];
            fr->IndexBuffer = nullptr;
            //fr->VertexBuffer = nullptr;
            fr->IndexBufferSize = 10000;
            fr->VertexBufferSize = 5000;

            fr->VertexPositonBuffer = nullptr;
            fr->VertexUVBuffer = nullptr;
            fr->VertexColorBuffer = nullptr;
        }

        return true;
    }

    void ImGui_ImplDX12_Shutdown_Refactoring()
    {
        ImGui_ImplDX12_Data_Refactoring* bd = ImGui_ImplDX12_GetBackendData_Refactoring();
        IM_ASSERT(bd != NULL && "No renderer backend to shutdown, or already shutdown?");
        ImGuiIO& io = ImGui::GetIO();

        ImGui_ImplDX12_InvalidateDeviceObjects_Refactoring();
        delete[] bd->pFrameResources;
        io.BackendRendererName = NULL;
        io.BackendRendererUserData = NULL;
        IM_DELETE(bd);
    }

    void ImGui_ImplDX12_NewFrame_Refactoring()
    {
        ImGui_ImplDX12_Data_Refactoring* bd = ImGui_ImplDX12_GetBackendData_Refactoring();
        IM_ASSERT(bd != NULL && "Did you call ImGui_ImplDX12_Init()?");

        if (!bd->pPipelineState)
            ImGui_ImplDX12_CreateDeviceObjects_Refactoring();
    }

    // Functions
    static void ImGui_ImplDX12_SetupRenderState_Refactoring(ImDrawData* draw_data, FGraphicsCommandContext& ctx, ImGui_ImplDX12_RenderBuffers_Refactoring* fr)
    {
        ImGui_ImplDX12_Data_Refactoring* bd = ImGui_ImplDX12_GetBackendData_Refactoring();

        // Setup orthographic projection matrix into our constant buffer
        // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
        VERTEX_CONSTANT_BUFFER_DX12 vertex_constant_buffer;
        {
            float L = draw_data->DisplayPos.x;
            float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
            float T = draw_data->DisplayPos.y;
            float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

            float mvp[4][4] =
            {
                { 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
                { 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
                { 0.0f,         0.0f,           0.5f,       0.0f },
                { (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
            };
            memcpy(&vertex_constant_buffer.mvp, mvp, sizeof(mvp));
        }
    
        ctx.SetViewport(FViewport{0.0f, 0.0f, draw_data->DisplaySize.x, draw_data->DisplaySize.y});

        FGpuVertexBufferRef vertexBuffers[3] = { fr->VertexPositonBuffer ,fr->VertexUVBuffer, fr->VertexColorBuffer };

        ctx.SetVertexBuffers(0, 3, vertexBuffers);

        ctx.SetIndexBuffer(fr->IndexBuffer);

        ctx.SetGraphicsPipelineState(bd->pPipelineState);
        ctx.SetRootConstantBufferView("constantBuffer", vertex_constant_buffer);

        ctx.SetBlendFactor(FLinearColor{0.0f, 0.0f, 0.0f, 0.0f});
    }

    void ImGui_CopyVertexData(ImGui_VertexDataBuffer& VertexDataBuffer, const ImVector<ImDrawVert>& ImguiVertexes)
    {
        VertexDataBuffer.Reserve((int32_t)VertexDataBuffer.Positions.size() + ImguiVertexes.Size);
        
        for (int32_t i = 0; i < ImguiVertexes.Size; i++)
        {
            const ImDrawVert& vertex = ImguiVertexes.Data[i];

            VertexDataBuffer.AddVertex(FVector2f{ vertex.pos.x, vertex.pos.y }, FVector2f{ vertex.uv.x, vertex.uv.y }, FColor{ vertex.col });
        }
    }

    void ImGui_ImplDX12_RenderDrawData_Refactoring(ImDrawData* draw_data, FGraphicsCommandContext& graphics_command_context)
    {
        // Avoid rendering when minimized
        if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
            return;

        // FIXME: I'm assuming that this only gets called once per frame!
        // If not, we can't just re-allocate the IB or VB, we'll have to do a proper allocator.
        ImGui_ImplDX12_Data_Refactoring* bd = ImGui_ImplDX12_GetBackendData_Refactoring();
        bd->frameIndex = bd->frameIndex + 1;
        ImGui_ImplDX12_RenderBuffers_Refactoring* fr = &bd->pFrameResources[bd->frameIndex % bd->numFramesInFlight];

        {
            // Create and grow vertex/index buffers if needed
            if (fr->VertexPositonBuffer == NULL || fr->VertexBufferSize < draw_data->TotalVtxCount)
            {
                fr->VertexPositonBuffer = nullptr;
                fr->VertexUVBuffer = nullptr;
                fr->VertexColorBuffer = nullptr;

                fr->VertexBufferSize = draw_data->TotalVtxCount + 5000;
                fr->VertexPositonBuffer = FGraphicsCore::Device->CreateDynamicVertexBuffer("IMGUI_VertexPositonBuffer", fr->VertexBufferSize, sizeof(FVector2f));
                fr->VertexUVBuffer = FGraphicsCore::Device->CreateDynamicVertexBuffer("IMGUI_VertexUVBuffer", fr->VertexBufferSize, sizeof(FVector2f));
                fr->VertexColorBuffer = FGraphicsCore::Device->CreateDynamicVertexBuffer("IMGUI_VertexColorBuffer", fr->VertexBufferSize, sizeof(FColor));
            }
            if (fr->IndexBuffer == NULL || fr->IndexBufferSize < draw_data->TotalIdxCount)
            {
                fr->IndexBuffer = nullptr;

                fr->IndexBufferSize = draw_data->TotalIdxCount + 10000;
                fr->IndexBuffer = FGraphicsCore::Device->CreateDynamicIndexBuffer("IMGUI_IndexBuffer", fr->IndexBufferSize, sizeof(ImDrawIdx) != 2);
            }

            ImGui_VertexDataBuffer vertexData;

            for (int n = 0; n < draw_data->CmdListsCount; n++)
            {
                const ImDrawList* cmd_list = draw_data->CmdLists[n];
                ImGui_CopyVertexData(vertexData, cmd_list->VtxBuffer);
            }

            if (vertexData.Positions.size() > 0)
            {
                fr->VertexPositonBuffer->UpdateData(vertexData.Positions.data(), vertexData.Positions.size() * sizeof(FVector2f));
                fr->VertexUVBuffer->UpdateData(vertexData.UVs.data(), vertexData.UVs.size() * sizeof(FVector2f));
                fr->VertexColorBuffer->UpdateData(vertexData.Colors.data(), vertexData.Colors.size() * sizeof(FColor));
            }

            void* idx_resource = fr->IndexBuffer->Map();
            ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource;

            for (int n = 0; n < draw_data->CmdListsCount; n++)
            {
                const ImDrawList* cmd_list = draw_data->CmdLists[n];
                memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
                idx_dst += cmd_list->IdxBuffer.Size;
            }
        }

        if (draw_data->CmdListsCount <= 0)
        {
            return;
        }

        graphics_command_context.SetRenderTarget(FGraphicsCore::SwapChain->GetDisplayBuffer());
        graphics_command_context.ClearColor(FGraphicsCore::SwapChain->GetDisplayBuffer());

        // Setup desired DX state
        ImGui_ImplDX12_SetupRenderState_Refactoring(draw_data, graphics_command_context, fr);

        graphics_command_context.SetShaderResourceView("texture0", bd->pFontTextureResource);

        // Render command lists
        // (Because we merged all buffers into a single one, we maintain our own offset into them)
        int global_vtx_offset = 0;
        int global_idx_offset = 0;
        ImVec2 clip_off = draw_data->DisplayPos;
        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback != NULL)
                {
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                    if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                        ImGui_ImplDX12_SetupRenderState_Refactoring(draw_data, graphics_command_context, fr);
                    else
                        pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    // Project scissor/clipping rectangles into framebuffer space
                    ImVec2 clip_min(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y);
                    ImVec2 clip_max(pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
                    if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                        continue;

                    // Apply Scissor/clipping rectangle, Bind texture, Draw
                    const D3D12_RECT r = { (LONG)clip_min.x, (LONG)clip_min.y, (LONG)clip_max.x, (LONG)clip_max.y };
                    D3D12_GPU_DESCRIPTOR_HANDLE texture_handle = {};
                    texture_handle.ptr = (UINT64)pcmd->GetTexID();

                    graphics_command_context.SetScissor(r);

                    graphics_command_context.DrawIndexedInstanced(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
                }
            }
            global_idx_offset += cmd_list->IdxBuffer.Size;
            global_vtx_offset += cmd_list->VtxBuffer.Size;
        }

        graphics_command_context.Flush();
    }

    void ImGui_ImplDX12_InvalidateDeviceObjects_Refactoring()
    {
        ImGui_ImplDX12_Data_Refactoring* bd = ImGui_ImplDX12_GetBackendData_Refactoring();
        if (!bd)
            return;
        ImGuiIO& io = ImGui::GetIO();

        bd->pPipelineState = nullptr;
        bd->pFontTextureResource = nullptr;

        io.Fonts->SetTexID(NULL); // We copied bd->pFontTextureView to io.Fonts->TexID so let's clear that as well.

        for (UINT i = 0; i < bd->numFramesInFlight; i++)
        {
            ImGui_ImplDX12_RenderBuffers_Refactoring* fr = &bd->pFrameResources[i];

            fr->IndexBuffer = nullptr;
            //fr->VertexBuffer = nullptr;

            fr->VertexPositonBuffer = nullptr;
            fr->VertexUVBuffer = nullptr;
            fr->VertexColorBuffer = nullptr;
        }
    }

    static void ImGui_ImplDX12_CreateFontsTexture_Refactoring()
    {
        // Build texture atlas
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplDX12_Data_Refactoring* bd = ImGui_ImplDX12_GetBackendData_Refactoring();
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        bd->pFontTextureResource = nullptr;

        FTextureBufferDescription textureDest = FTextureBufferDescription::Create2D(EResourceFormat::RGBA8_Unsigned_Norm, width, height, 1);
        bd->pFontTextureResource = FGraphicsCore::Device->CreateTextureBufferFromMemory("TestTexture", textureDest, pixels);

        io.Fonts->SetTexID((ImTextureID)bd->pFontTextureResource->GetShaderResourceView().ptr);
    }

    bool ImGui_ImplDX12_CreateDeviceObjects_Refactoring()
    {
        ImGui_ImplDX12_Data_Refactoring* bd = ImGui_ImplDX12_GetBackendData_Refactoring();
        if (!bd)
            return false;
        if (bd->pPipelineState)
            ImGui_ImplDX12_InvalidateDeviceObjects_Refactoring();
    
        {
            FShaderCreationInfo vsInfo{ EShaderStage::Vertex, FFileUtility::GetEngineShaderDir("IMGUI_shader.hlsl"),  "VS_Main" };
            vsInfo.Finalize();

            FShaderCreationInfo psInfo{ EShaderStage::Pixel, FFileUtility::GetEngineShaderDir("IMGUI_shader.hlsl"),  "PS_Main" };
            psInfo.Finalize();

            FRasterizerState rasterizerCullOff{ ERasterizerFillMode::Solid, ERasterizerCullMode::None };
            FBlendState blendDisable{ true, false };
            FDepthStencilState depthStateDisabled{ false, false };

            FShaderPassRef drawPass = FShaderPass::MakeShaderPass("IMGUI_DrawPass", { vsInfo , psInfo }, blendDisable, rasterizerCullOff, depthStateDisabled);

            FGraphicsPSORef drawPSO = FGraphicsPSO::MakeGraphicsPSO("IMGUI_PSO");

            drawPSO->SetShaderPass(drawPass);
            drawPSO->SetPrimitiveTopologyType(EPrimitiveTopology::TriangleList);
            drawPSO->SetSamplerMask(UINT_MAX);
            drawPSO->SetRenderTargetFormat(FGraphicsCore::SwapChain->GetBackBufferFormat(), EResourceFormat::Depth32_Float);
            drawPSO->Finalize();

            bd->pPipelineState = drawPSO;

            ImGui_ImplDX12_CreateFontsTexture_Refactoring();
        }

        return true;
    }

}
