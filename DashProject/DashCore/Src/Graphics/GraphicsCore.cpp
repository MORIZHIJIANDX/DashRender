#include "PCH.h"
#include "GraphicsCore.h"
#include "../Utility/Exception.h"

#if defined(NTDDI_WIN10_RS2) && (NTDDI_VERSION >= NTDDI_WIN10_RS2)
#include <dxgi1_6.h>
#else
#include <dxgi1_4.h>    // For WARP
#endif

using namespace Microsoft::WRL;

namespace Dash
{
	ID3D12Device* Graphics::Device = nullptr;
    D3D_FEATURE_LEVEL DefaultD3DFeatureLevel = D3D_FEATURE_LEVEL_12_0;
    bool bTypedUAVLoadSupport_R11G11B10_FLOAT = false;
    bool bTypedUAVLoadSupport_R16G16B16A16_FLOAT = false;

	IDXGISwapChain1* SwapChain;

	void Graphics::Initialize()
	{
		ASSERT_MSG(SwapChain != nullptr, "Graphics has already been initialized!");

		UINT dxgiFactoryFlag = 0;

#if DASH_DEBUG
		ComPtr<ID3D12Debug> DebugInterface;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugInterface))))
		{
			DebugInterface->EnableDebugLayer();
			dxgiFactoryFlag |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif // DASH_DEBUG

		ComPtr<IDXGIFactory4> dxgiFactory;
		CreateDXGIFactory2(dxgiFactoryFlag, IID_PPV_ARGS(&dxgiFactory));

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
                        LOG_INFO << "Adapter Memory " << desc.DedicatedVideoMemory;

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
                        LOG_INFO << "Adapter Memory " << desc.DedicatedVideoMemory;
                        break;
                    }
                }
            }
		}

        if (Graphics::Device == nullptr)
        {
            LOG_ERROR << "Can't Create D3D12 Hardware Device!" ;
        }

#if DASH_DEBUG
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
        //Create swap chain
        //Create back buffer
	}


	void Graphics::Shutdown()
	{
	}
}