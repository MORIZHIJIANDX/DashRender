// dear imgui: Renderer Backend for DirectX12
// This needs to be used along with a Platform Backend (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'D3D12_GPU_DESCRIPTOR_HANDLE' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bit indices.

// Important: to compile on 32-bit systems, this backend requires code to be compiled with '#define ImTextureID ImU64'.
// See imgui_impl_dx12.cpp file for details.

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#pragma once
//#include "imgui.h"      // IMGUI_IMPL_API
#include "ThirdParty/imgui/imgui.h"
#include <dxgiformat.h> // DXGI_FORMAT

#include "Graphics/CommandContext.h"

// refactory

namespace Dash
{
    IMGUI_IMPL_API bool     ImGui_ImplDX12_Init_Refactoring(int num_frames_in_flight);
    IMGUI_IMPL_API void     ImGui_ImplDX12_Shutdown_Refactoring();

    IMGUI_IMPL_API void     ImGui_ImplDX12_NewFrame_Refactoring();

    IMGUI_IMPL_API void     ImGui_ImplDX12_RenderDrawData_Refactoring(ImDrawData* draw_data, FGraphicsCommandContext& graphics_command_list);

    IMGUI_IMPL_API void     ImGui_ImplDX12_InvalidateDeviceObjects_Refactoring();

    IMGUI_IMPL_API bool     ImGui_ImplDX12_CreateDeviceObjects_Refactoring();
}

