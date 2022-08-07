#include "PCH.h"
#include "GraphicsCore.h"
#include "GameApp.h"
#include "../Utility/Exception.h"
#include "Camera.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <d3d12sdklayers.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;

#define CONDITIONALLY_ENABLE_HDR_OUTPUT 1

namespace Dash
{
    struct FFrameConstantBuffer
    {
        FMatrix4x4 ViewMatrix;
        FMatrix4x4 ProjectionMatrix;
        FMatrix4x4 WorldMatrix;
        FMatrix4x4 InversetransposedWorldMatrix;
        FVector4f LightDirection;
        FVector4f LightColor;
        float Time;
        FVector2f Speed;
    };

	ID3D12Device* Graphics::Device = nullptr;
    ID3D12CommandQueue* Graphics::CommandQueue = nullptr;
    IDXGISwapChain1* SwapChain = nullptr;

    ID3D12Resource* BackBuffers[Graphics::BackBufferCount];

    ID3D12Resource* DepthBuffer = nullptr;
    ID3D12Resource* ConstantBuffer = nullptr;

    ID3D12Resource* VertexBuffer = nullptr;
    ID3D12Resource* IndexBuffer = nullptr;

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
    D3D12_INDEX_BUFFER_VIEW IndexBufferView;

    UINT IndexCount = 0;

    ID3D12DescriptorHeap* RTVDescriptorHeap = nullptr;
    ID3D12DescriptorHeap* DSVDescriptorHeap = nullptr;
    ID3D12DescriptorHeap* SRVCBVDescriptorHeap = nullptr;

    UINT RTVDescriptorSize = 0;

    ID3D12RootSignature* RootSignature = nullptr;
    ID3D12PipelineState* PipelineState = nullptr;

    D3D_FEATURE_LEVEL DefaultD3DFeatureLevel = D3D_FEATURE_LEVEL_12_0;
    bool bTypedUAVLoadSupport_R11G11B10_FLOAT = false;
    bool bTypedUAVLoadSupport_R16G16B16A16_FLOAT = false;
    bool bEnableHDROutput = false;

    FFrameConstantBuffer* FrameConstantBuffer = nullptr;

    ID3DBlob* VertexShader = nullptr;
    ID3DBlob* PixelShader = nullptr;

    ID3D12CommandAllocator* CommandAllocator = nullptr;
    ID3D12GraphicsCommandList* CommandList = nullptr;

    ID3D12Fence* GPUFence = nullptr;
    UINT64 FenceValue = 0;
    HANDLE FenceEvent = nullptr;

    UINT64 FrameIndex = 0;

    DXGI_FORMAT BackBufferFormat = DXGI_FORMAT::DXGI_FORMAT_R10G10B10A2_UNORM;

    D3D12_VIEWPORT ViewPort;
    RECT ScissorRect;

    FTransform CubeTransform;

    struct Vertex
    {
        FVector3f Pos;
        FVector3f Normal;
        FVector4f Tangent;
        FVector4f Color;
        FVector2f UV;
    };

    struct MeshData
    {
        std::vector<Vertex> VertexData;    // 顶点数组
        std::vector<UINT> IndexData;    // 索引数组

        MeshData()
        {
        }
    };

    static MeshData CreateBoxMesh(Scalar width, Scalar height, Scalar depth, FVector4f color)
    {
        const Scalar halfWidth = width * 0.5f;
        const Scalar halfHeight = height * 0.5f;
        const Scalar halfDepth = depth * 0.5f;

        MeshData data;
        data.VertexData.resize(24);

        // 右面(+X面)
        data.VertexData[0].Pos = FVector3f(halfWidth, -halfHeight, -halfDepth);
        data.VertexData[1].Pos = FVector3f(halfWidth, halfHeight, -halfDepth);
        data.VertexData[2].Pos = FVector3f(halfWidth, halfHeight, halfDepth);
        data.VertexData[3].Pos = FVector3f(halfWidth, -halfHeight, halfDepth);
        // 左面(-X面)
        data.VertexData[4].Pos = FVector3f(-halfWidth, -halfHeight, halfDepth);
        data.VertexData[5].Pos = FVector3f(-halfWidth, halfHeight, halfDepth);
        data.VertexData[6].Pos = FVector3f(-halfWidth, halfHeight, -halfDepth);
        data.VertexData[7].Pos = FVector3f(-halfWidth, -halfHeight, -halfDepth);
        // 顶面(+Y面)
        data.VertexData[8].Pos = FVector3f(-halfWidth, halfHeight, -halfDepth);
        data.VertexData[9].Pos = FVector3f(-halfWidth, halfHeight, halfDepth);
        data.VertexData[10].Pos = FVector3f(halfWidth, halfHeight, halfDepth);
        data.VertexData[11].Pos = FVector3f(halfWidth, halfHeight, -halfDepth);
        // 底面(-Y面)
        data.VertexData[12].Pos = FVector3f(halfWidth, -halfHeight, -halfDepth);
        data.VertexData[13].Pos = FVector3f(halfWidth, -halfHeight, halfDepth);
        data.VertexData[14].Pos = FVector3f(-halfWidth, -halfHeight, halfDepth);
        data.VertexData[15].Pos = FVector3f(-halfWidth, -halfHeight, -halfDepth);
        // 背面(+Z面)
        data.VertexData[16].Pos = FVector3f(halfWidth, -halfHeight, halfDepth);
        data.VertexData[17].Pos = FVector3f(halfWidth, halfHeight, halfDepth);
        data.VertexData[18].Pos = FVector3f(-halfWidth, halfHeight, halfDepth);
        data.VertexData[19].Pos = FVector3f(-halfWidth, -halfHeight, halfDepth);
        // 正面(-Z面)
        data.VertexData[20].Pos = FVector3f(-halfWidth, -halfHeight, -halfDepth);
        data.VertexData[21].Pos = FVector3f(-halfWidth, halfHeight, -halfDepth);
        data.VertexData[22].Pos = FVector3f(halfWidth, halfHeight, -halfDepth);
        data.VertexData[23].Pos = FVector3f(halfWidth, -halfHeight, -halfDepth);

        for (size_t i = 0; i < 4; ++i)
        {
            // 右面(+X面)
            data.VertexData[i].Normal = FVector3f(1.0f, 0.0f, 0.0f);
            data.VertexData[i].Tangent = FVector4f(0.0f, 0.0f, 1.0f, 1.0f);
            data.VertexData[i].Color = color;
            // 左面(-X面)
            data.VertexData[i + 4].Normal = FVector3f(-1.0f, 0.0f, 0.0f);
            data.VertexData[i + 4].Tangent = FVector4f(0.0f, 0.0f, -1.0f, 1.0f);
            data.VertexData[i + 4].Color = color;
            // 顶面(+Y面)
            data.VertexData[i + 8].Normal = FVector3f(0.0f, 1.0f, 0.0f);
            data.VertexData[i + 8].Tangent = FVector4f(1.0f, 0.0f, 0.0f, 1.0f);
            data.VertexData[i + 8].Color = color;
            // 底面(-Y面)
            data.VertexData[i + 12].Normal = FVector3f(0.0f, -1.0f, 0.0f);
            data.VertexData[i + 12].Tangent = FVector4f(-1.0f, 0.0f, 0.0f, 1.0f);
            data.VertexData[i + 12].Color = color;
            // 背面(+Z面)
            data.VertexData[i + 16].Normal = FVector3f(0.0f, 0.0f, 1.0f);
            data.VertexData[i + 16].Tangent = FVector4f(-1.0f, 0.0f, 0.0f, 1.0f);
            data.VertexData[i + 16].Color = color;
            // 正面(-Z面)
            data.VertexData[i + 20].Normal = FVector3f(0.0f, 0.0f, -1.0f);
            data.VertexData[i + 20].Tangent = FVector4f(1.0f, 0.0f, 0.0f, 1.0f);
            data.VertexData[i + 20].Color = color;
        }

        for (size_t i = 0; i < 6; ++i)
        {
            data.VertexData[i * 4].UV = FVector2f(0.0f, 1.0f);
            data.VertexData[i * 4 + 1].UV = FVector2f(0.0f, 0.0f);
            data.VertexData[i * 4 + 2].UV = FVector2f(1.0f, 0.0f);
            data.VertexData[i * 4 + 3].UV = FVector2f(1.0f, 1.0f);
        }

        data.IndexData = {
            0, 1, 2, 2, 3, 0,        // 右面(+X面)
            4, 5, 6, 6, 7, 4,        // 左面(-X面)
            8, 9, 10, 10, 11, 8,    // 顶面(+Y面)
            12, 13, 14, 14, 15, 12,    // 底面(-Y面)
            16, 17, 18, 18, 19, 16, // 背面(+Z面)
            20, 21, 22, 22, 23, 20    // 正面(-Z面)
        };

        return data;
    }

    void WaitForGpu()
    {
        UINT64 valueToWait = FenceValue++;

        HR(Graphics::CommandQueue->Signal(GPUFence, valueToWait));

        if (GPUFence->GetCompletedValue() < valueToWait)
        {
            HR(GPUFence->SetEventOnCompletion(valueToWait, FenceEvent));
            WaitForSingleObject(FenceEvent, INFINITE);
        }
    }

    void PopulateCommandList(const FRenderEventArgs& e)
    {
        if (FrameConstantBuffer)
        {
            if (e.mCamera)
            {
                FrameConstantBuffer->ProjectionMatrix = e.mCamera->GetProjectionMatrix();
                FrameConstantBuffer->ViewMatrix = e.mCamera->GetViewMatrix();
            }
            FrameConstantBuffer->LightDirection = FMath::Normalize(FVector4f(1.0f, 1.0f, -1.0f, 0.0f));
            FrameConstantBuffer->LightColor = FVector4f(0.95f, 0.89f, 0.95f, 0.0f);

            FrameConstantBuffer->WorldMatrix = CubeTransform.GetMatrix();
            FrameConstantBuffer->InversetransposedWorldMatrix = FMath::Transpose(CubeTransform.GetInverseMatrix());

            FrameConstantBuffer->Time = static_cast<float>(e.mTotalTime);
        }

        HR(CommandAllocator->Reset());
        HR(CommandList->Reset(CommandAllocator, PipelineState));

        CommandList->SetGraphicsRootSignature(RootSignature);
        CommandList->SetPipelineState(PipelineState);

        ID3D12DescriptorHeap* descriptorHeaps[] = { SRVCBVDescriptorHeap };
        CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        CommandList->SetGraphicsRootDescriptorTable(0, SRVCBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

        CommandList->RSSetViewports(1, &ViewPort);
        CommandList->RSSetScissorRects(1, &ScissorRect);

        UINT64 backBufferIndex = FrameIndex % Graphics::BackBufferCount;
        CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(BackBuffers[backBufferIndex], 
            D3D12_RESOURCE_STATE_PRESENT, 
            D3D12_RESOURCE_STATE_RENDER_TARGET));

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtDescriptorHandle(RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), (INT)backBufferIndex, RTVDescriptorSize);
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsDescriptorHandle(DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
        CommandList->OMSetRenderTargets(1, &rtDescriptorHandle, false, &dsDescriptorHandle);

        FVector4f clearColor{0.5f, 0.5f, 0.5f, 1.0f};
        CommandList->ClearRenderTargetView(rtDescriptorHandle, clearColor, 0, nullptr);
        CommandList->ClearDepthStencilView(dsDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
        CommandList->IASetIndexBuffer(&IndexBufferView);

        CommandList->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);

        CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(BackBuffers[backBufferIndex],
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT));

        HR(CommandList->Close());
    }

	void Graphics::Initialize()
	{
		ASSERT_MSG(SwapChain == nullptr, "Graphics has already been initialized!");

		UINT dxgiFactoryFlags = 0;

#if DASH_DEBUG
		ComPtr<ID3D12Debug> DebugInterface;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugInterface))))
		{
			DebugInterface->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif // DASH_DEBUG

		ComPtr<IDXGIFactory4> dxgiFactory;
		CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory));

		ComPtr<IDXGIAdapter1> dxgiAdapter;

		{
            Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
            if (SUCCEEDED(dxgiFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
            {
                for (
                    UINT adapterIndex = 0;
                    DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
                        adapterIndex,
                        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                        IID_PPV_ARGS(&dxgiAdapter));
                    ++adapterIndex)
                {
                    DXGI_ADAPTER_DESC1 desc;
                    dxgiAdapter->GetDesc1(&desc);

                    // Skip the Basic Render Driver adapter.
                    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                    {
                        continue;
                    }

                    if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(), DefaultD3DFeatureLevel, IID_PPV_ARGS(&Graphics::Device))))
                    {
                        LOG_INFO << "Create Device With Adapter : " << desc.Description;
                        LOG_INFO << "Adapter Memory " << desc.DedicatedVideoMemory / (1024 * 1024) << " MB";

                        break;
                    }
                }
            }
            else
            {
                for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(adapterIndex, &dxgiAdapter); ++adapterIndex)
                {
                    DXGI_ADAPTER_DESC1 desc;
                    dxgiAdapter->GetDesc1(&desc);

                    // Skip the Basic Render Driver adapter.
                    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                    {
                        continue;
                    }

                    if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(), DefaultD3DFeatureLevel, IID_PPV_ARGS(&Graphics::Device))))
                    {
                        LOG_INFO << "Create Device With Adapter : " << desc.Description;
                        LOG_INFO << "Adapter Memory " << desc.DedicatedVideoMemory / (1024 * 1024) << " MB";
                        break;
                    }
                }
            }
		}

        if (Graphics::Device == nullptr)
        {
            LOG_ERROR << "Can't Create D3D12 Hardware Device!" ;
        }

#if !DASH_DEBUG
        {
            bool DeveloperModeEnabled = false;

            // Look in the Windows Registry to determine if Developer Mode is enabled
            HKEY hKey;
            LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock", 0, KEY_READ, &hKey);
            if (result == ERROR_SUCCESS)
            {
                DWORD keyValue, keySize = sizeof(DWORD);
                result = RegQueryValueEx(hKey, "AllowDevelopmentWithoutDevLicense", 0, NULL, (byte*)&keyValue, &keySize);
                if (result == ERROR_SUCCESS && keyValue == 1)
                    DeveloperModeEnabled = true;
                RegCloseKey(hKey);
            }

            ASSERT_MSG(DeveloperModeEnabled, "Enable Developer Mode on Windows 10 to get consistent profiling results");

            // Prevent the GPU from overclocking or underclocking to get consistent timings
            if (DeveloperModeEnabled)
                Graphics::Device->SetStablePowerState(TRUE);
        }
#endif // DASH_DEBUG

#if DASH_DEBUG
        {
            ID3D12InfoQueue* pInfoQueue = nullptr;
            if (SUCCEEDED(Graphics::Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
            {
                // Suppress whole categories of messages
                //D3D12_MESSAGE_CATEGORY Categories[] = {};

                // Suppress messages based on their severity level
                D3D12_MESSAGE_SEVERITY Severities[] =
                {
                    D3D12_MESSAGE_SEVERITY_INFO
                };

                // Suppress individual messages by their ID
                D3D12_MESSAGE_ID DenyIds[] =
                {
                    // This occurs when there are uninitialized descriptors in a descriptor table, even when a
                    // shader does not access the missing descriptors.  I find this is common when switching
                    // shader permutations and not wanting to change much code to reorder resources.
                    D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

                    // Triggered when a shader does not export all color components of a render target, such as
                    // when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
                    D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,

                    // This occurs when a descriptor table is unbound even when a shader does not access the missing
                    // descriptors.  This is common with a root signature shared between disparate shaders that
                    // don't all need the same types of resources.
                    D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

                    // RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS
                    (D3D12_MESSAGE_ID)1008,
                };

                D3D12_INFO_QUEUE_FILTER NewFilter = {};
                NewFilter.DenyList.NumSeverities = _countof(Severities);
                NewFilter.DenyList.pSeverityList = Severities;
                NewFilter.DenyList.NumIDs = _countof(DenyIds);
                NewFilter.DenyList.pIDList = DenyIds;

                pInfoQueue->PushStorageFilter(&NewFilter);
                pInfoQueue->Release();
            }
        }
#endif // DASH_DEBUG

        // We like to do read-modify-write operations on UAVs during post processing.  To support that, we
        // need to either have the hardware do typed UAV loads of R11G11B10_FLOAT or we need to manually
        // decode an R32_UINT representation of the same buffer.  This code determines if we get the hardware
        // load support.
        D3D12_FEATURE_DATA_D3D12_OPTIONS FeatureData = {};
        if (SUCCEEDED(Graphics::Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &FeatureData, sizeof(FeatureData))))
        {
            if (FeatureData.TypedUAVLoadAdditionalFormats)
            {
                D3D12_FEATURE_DATA_FORMAT_SUPPORT Support =
                {
                    DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE
                };

                if (SUCCEEDED(Graphics::Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
                    (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
                {
                    bTypedUAVLoadSupport_R11G11B10_FLOAT = true;
                }

                Support.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

                if (SUCCEEDED(Graphics::Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
                    (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
                {
                    bTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
                }
            }
        }

        //Create command queue
        D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
        commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        HR(Graphics::Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&CommandQueue)));

        //Create swap chain
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
        swapChainDesc.Width = IGameApp::GetInstance()->GetWindowWidth();
        swapChainDesc.Height = IGameApp::GetInstance()->GetWindowHeight();
        swapChainDesc.Format = BackBufferFormat;
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = Graphics::BackBufferCount;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

        HR(dxgiFactory->CreateSwapChainForHwnd(Graphics::CommandQueue, IGameApp::GetInstance()->GetWindowHandle(), &swapChainDesc, nullptr, nullptr, &SwapChain));

#if CONDITIONALLY_ENABLE_HDR_OUTPUT && defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
        {
            IDXGISwapChain4* swapChain = (IDXGISwapChain4*)SwapChain;
            ComPtr<IDXGIOutput> output;
            ComPtr<IDXGIOutput6> output6;
            DXGI_OUTPUT_DESC1 outputDesc;
            UINT colorSpaceSupport;

            // Query support for ST.2084 on the display and set the color space accordingly
            if (SUCCEEDED(swapChain->GetContainingOutput(&output)) &&
                SUCCEEDED(output.As(&output6)) &&
                SUCCEEDED(output6->GetDesc1(&outputDesc)) &&
                outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 &&
                SUCCEEDED(swapChain->CheckColorSpaceSupport(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020, &colorSpaceSupport)) &&
                (colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) &&
                SUCCEEDED(swapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)))
            {
                bEnableHDROutput = true;
            }
        }
#endif

        //Create Render Targe View
        {
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
            rtvHeapDesc.NumDescriptors = Graphics::BackBufferCount;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            HR(Graphics::Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&RTVDescriptorHeap)));

            RTVDescriptorSize = Graphics::Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

            CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            for (UINT i = 0; i < Graphics::BackBufferCount; i++)
            {
                SwapChain->GetBuffer(i, IID_PPV_ARGS(&BackBuffers[i]));
                Graphics::Device->CreateRenderTargetView(BackBuffers[i], nullptr, descriptorHandle);
                descriptorHandle.Offset(1, RTVDescriptorSize);
            }
        }       

        //Create Depth Buffer
        {
            D3D12_RESOURCE_DESC depthStencilDesc{};
            depthStencilDesc.MipLevels = 1;
            depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
            depthStencilDesc.Width = IGameApp::GetInstance()->GetWindowWidth();
            depthStencilDesc.Height = IGameApp::GetInstance()->GetWindowHeight();
            depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            depthStencilDesc.DepthOrArraySize = 1;
            depthStencilDesc.MipLevels = 0;
            depthStencilDesc.SampleDesc.Count = 1;
            depthStencilDesc.SampleDesc.Quality = 0;
            depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            D3D12_CLEAR_VALUE depthClearValue;
            depthClearValue.DepthStencil.Depth = 1.0f;
            depthClearValue.DepthStencil.Stencil = 0;
            depthClearValue.Format = depthStencilDesc.Format;

            Graphics::Device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &depthStencilDesc,
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &depthClearValue,
                IID_PPV_ARGS(&DepthBuffer)
            );
        }

        //Create Depth Descriptor Heap & Depth Stencil View
        {
            D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
            dsvHeapDesc.NumDescriptors = 1;
            dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            HR(Graphics::Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&DSVDescriptorHeap)));

            CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            Graphics::Device->CreateDepthStencilView(DepthBuffer, nullptr, descriptorHandle);
        }

        //Create SRV & CBV Heap
        {
            D3D12_DESCRIPTOR_HEAP_DESC srvCBVHeapDesc{};
            srvCBVHeapDesc.NumDescriptors = 1;
            srvCBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            srvCBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

            HR(Graphics::Device->CreateDescriptorHeap(&srvCBVHeapDesc, IID_PPV_ARGS(&SRVCBVDescriptorHeap)));
        }

        //Create Constant Buffer
        {
            UINT constantBufferSize = UPPER_ALIGNMENT(sizeof(FFrameConstantBuffer), 256);

            HR(Graphics::Device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&ConstantBuffer)
            ));

            D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc;
            constantBufferViewDesc.SizeInBytes = constantBufferSize;
            constantBufferViewDesc.BufferLocation = ConstantBuffer->GetGPUVirtualAddress();

            CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(SRVCBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
            Graphics::Device->CreateConstantBufferView(&constantBufferViewDesc, descriptorHandle);

            //Map constant buffer
            CD3DX12_RANGE readRange(0, 0);
            ConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&FrameConstantBuffer));
        }

        //Create Vertex Buffer & Index Buffer
        {
            MeshData boxMesh = CreateBoxMesh(1, 1, 1, FVector4f{ 0.8f, 0.5f, 1.0f, 1.0f });

            const UINT vertexBufferSize = (UINT)boxMesh.VertexData.size() * sizeof(Vertex);
            const UINT indexBufferSize = (UINT)boxMesh.IndexData.size() * sizeof(UINT);

            HR(Graphics::Device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&VertexBuffer)
            ));

            CD3DX12_RANGE readRange(0, 0);

            //Copy Vertex Data
            UINT8* vertexDataBegin = nullptr;
            HR(VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vertexDataBegin)));
            memcpy(vertexDataBegin, boxMesh.VertexData.data(), vertexBufferSize);
            VertexBuffer->Unmap(0, nullptr);

            //Create Vertex Buffer View
            VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
            VertexBufferView.SizeInBytes = vertexBufferSize;
            VertexBufferView.StrideInBytes = sizeof(Vertex);

            HR(Graphics::Device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
                D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&IndexBuffer)
            ));

            //Copy Index Data
            UINT8* indexDataBegin = nullptr;
            HR(IndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&indexDataBegin)));
            memcpy(indexDataBegin, boxMesh.IndexData.data(), indexBufferSize);
            IndexBuffer->Unmap(0, nullptr);

            //Create Index Buffer View
            IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
            IndexBufferView.SizeInBytes = indexBufferSize;
            IndexBufferView.Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

            IndexCount = (UINT)boxMesh.IndexData.size();
        }

        //Create Root Signature
        {
            CD3DX12_DESCRIPTOR_RANGE1 descriptorRanges[1] = {};
            descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

            CD3DX12_ROOT_PARAMETER1 rootParameters[1] = {};
            rootParameters[0].InitAsDescriptorTable(1, &descriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL);

            //CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
            rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

            //rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

            ComPtr<ID3DBlob> blob;
            ComPtr<ID3DBlob> errorBlob;
            HR(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &errorBlob));
            //HR(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &errorBlob));
            HR(Graphics::Device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&RootSignature)));
        }

        //Compile Shader
        {
            UINT shaderCompileFlags = 0;
            
#if DASH_DEBUG
            shaderCompileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // DASH_DEBUG

            shaderCompileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

            std::filesystem::path currentPath = std::filesystem::current_path();
            std::filesystem::path shaderPath = currentPath.parent_path() / "DashCore\\Src\\Resources\\shader.hlsl";

            HR(D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", shaderCompileFlags, 0, &VertexShader, nullptr));
            HR(D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", shaderCompileFlags, 0, &PixelShader, nullptr));
        }

        //Create PSO
        {
            D3D12_INPUT_ELEMENT_DESC inputElements[] = 
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc = {};
            PipelineStateDesc.pRootSignature = RootSignature;
            PipelineStateDesc.InputLayout = { inputElements , _countof(inputElements) };
            PipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE{ VertexShader };
            PipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE{ PixelShader };
            PipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC{D3D12_DEFAULT};
            //PipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
            PipelineStateDesc.BlendState = CD3DX12_BLEND_DESC{ D3D12_DEFAULT };
            PipelineStateDesc.DepthStencilState.DepthEnable = TRUE;
            PipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
            PipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            PipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
            PipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
            PipelineStateDesc.SampleMask = UINT_MAX;
            PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            PipelineStateDesc.NumRenderTargets = 1;
            PipelineStateDesc.RTVFormats[0] = BackBufferFormat;
            PipelineStateDesc.SampleDesc.Count = 1;

            HR(Graphics::Device->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&PipelineState)));
        }

        //Create Command Allocator
        {
            HR(Graphics::Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
        }

        //Create Command List
        {
            HR(Graphics::Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator, PipelineState, IID_PPV_ARGS(&CommandList)));
            HR(CommandList->Close());
        }
        
        //Create Fence
        {
            HR(Graphics::Device->CreateFence(FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GPUFence)));
            ++FenceValue;
        }

        //Create Event
        {
            FenceEvent = CreateEvent(nullptr, false, false, nullptr);
            if (FenceEvent == nullptr)
            {
                HR(HRESULT_FROM_WIN32(GetLastError()));
            }
        }

        WaitForGpu();

        //Create Camera &&  ViewPort
        {
            ViewPort.Width = (FLOAT)IGameApp::GetInstance()->GetWindowWidth();
            ViewPort.Height = (FLOAT)IGameApp::GetInstance()->GetWindowHeight();
            ViewPort.TopLeftX = 0;
            ViewPort.TopLeftY = 0;
            ViewPort.MinDepth = 0;
            ViewPort.MaxDepth = 1;
            
            ScissorRect.left = 0;
            ScissorRect.right = (LONG)IGameApp::GetInstance()->GetWindowWidth();
            ScissorRect.top = 0;
            ScissorRect.bottom = (LONG)IGameApp::GetInstance()->GetWindowHeight();
        }

        //Init Cube Transform
        {
            CubeTransform = FTransform(FIdentity());
        }
	}


	void Graphics::Shutdown()
	{
        WaitForGpu();

        //Unmap constant buffer
        ConstantBuffer->Unmap(0, nullptr);

        CommandList->Release();
        GPUFence->Release();
        CommandQueue->Release();
        SwapChain->Release();

        CommandAllocator->Release();

        PipelineState->Release();
        RootSignature->Release();

        RTVDescriptorHeap->Release();
        DSVDescriptorHeap->Release();
        SRVCBVDescriptorHeap->Release();

        DepthBuffer->Release();
        ConstantBuffer->Release();

        VertexBuffer->Release();
        IndexBuffer->Release();

        for (UINT i = 0; i < Graphics::BackBufferCount; i++)
        {
            BackBuffers[i]->Release();
        }

#if defined(DASH_DEBUG)
        ID3D12DebugDevice* debugInterface;
        if (SUCCEEDED(Graphics::Device->QueryInterface(&debugInterface)))
        {
            debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
            debugInterface->Release();
        }
#endif

        if (Graphics::Device != nullptr)
        {
            Graphics::Device->Release();
            Graphics::Device = nullptr;
        }

        CloseHandle(FenceEvent);
	}

    void Graphics::OnRender(const FRenderEventArgs& e)
    {
        PopulateCommandList(e);

        ID3D12CommandList* commands[] = { CommandList };
        CommandQueue->ExecuteCommandLists(_countof(commands), commands);

        HR(SwapChain->Present(1, 0));

        WaitForGpu();

        ++FrameIndex;
    }
}