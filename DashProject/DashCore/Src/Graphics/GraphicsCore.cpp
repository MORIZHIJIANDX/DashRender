#include "PCH.h"
#include "GraphicsCore.h"
#include "CommandQueue.h"
#include "CpuDescriptorAllocator.h"
#include <wrl.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include "DX12Helper.h"
#include "CommandContext.h"
#include "RootSignature.h"
#include "SamplerDesc.h"
#include "Display.h"
#include "GameApp.h"
#include "Utility/StringUtility.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
//#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;

namespace Dash
{ 
	static const uint32_t VendorID_Nvidia = 0x10DE;
	static const uint32_t VendorID_AMD = 0x1002;
	static const uint32_t VendorID_Intel = 0x8086;

	ID3D12Device* FGraphicsCore::Device = nullptr;
	FCommandQueueManager* FGraphicsCore::CommandQueueManager = nullptr;
	FCommandListManager* FGraphicsCore::CommandListManager = nullptr;
	FCpuDescriptorAllocatorManager* FGraphicsCore::DescriptorAllocator = nullptr;
	FCommandContextManager* FGraphicsCore::ContextManager = nullptr;
	FDisplay* FGraphicsCore::Display = nullptr;

	bool FGraphicsCore::mTypedUAVLoadSupport_R11G11B10_FLOAT = false;
	bool FGraphicsCore::mTypedUAVLoadSupport_R16G16B16A16_FLOAT = false;

	D3D_ROOT_SIGNATURE_VERSION FGraphicsCore::mHighestRootSignatureVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	void FGraphicsCore::Initialize()
	{
		LOG_INFO << "FGraphicsCore::Initialize Begin.";

		FGraphicsCore::InitD3DDevice();

		CommandQueueManager = new FCommandQueueManager();
		CommandListManager = new FCommandListManager();
		DescriptorAllocator = new FCpuDescriptorAllocatorManager();
		ContextManager = new FCommandContextManager();
		Display = new FDisplay(IGameApp::GetInstance()->GetWindowWidth(), IGameApp::GetInstance()->GetWindowHeight());

		LOG_INFO << "FGraphicsCore::Initialize End.";
	}

	void FGraphicsCore::Shutdown()
	{
		LOG_INFO << "FGraphicsCore::Shutdown Begin.";

		if (CommandQueueManager)
		{
			CommandQueueManager->Flush();

			LOG_INFO << "Flush Command Queue.";
		}

		if (ContextManager)
		{
			ContextManager->Destroy();
			delete ContextManager;

			LOG_INFO << "Destroy Command Context Manager.";
		}

		if (CommandQueueManager)
		{
			CommandQueueManager->Destroy();
			delete CommandQueueManager;

			LOG_INFO << "Destroy Command Queue Manager.";
		}

		if (CommandListManager)
		{
			CommandListManager->Destroy();
			delete CommandListManager;

			LOG_INFO << "Destroy Command List Manager.";
		}

		if (DescriptorAllocator)
		{
			DescriptorAllocator->Destroy();
			delete DescriptorAllocator;

			LOG_INFO << "Destroy CPU Descriptor Allocator.";
		}

		if(Display)
		{
			Display->Destroy();
			delete Display;

			LOG_INFO << "Destroy Swap Chain.";
		}

		FPipelineStateObject::DestroyAll();
		FRootSignature::DestroyAll();
		FSamplerDesc::DestroyAll();

		DestroyD3Device();
	
		LOG_INFO << "FGraphicsCore::Shutdown End.";
	}

	void FGraphicsCore::InitD3DDevice()
	{
		ASSERT_MSG(FGraphicsCore::Device == nullptr, "FGraphicsCore Has Already Been Initialized!");

		UINT dxgiFactoryFlags = 0;

#if DASH_DEBUG
		//Enable debug layer
		{
			ComPtr<ID3D12Debug> debugInterface;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
			{
				debugInterface->EnableDebugLayer();

				ComPtr<ID3D12Debug1> debugInterface1;
				if (SUCCEEDED(debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1))))
				{
					debugInterface1->SetEnableGPUBasedValidation(TRUE);
				}
			}
			else
			{
				LOG_WARNING << "Unable To Enable D3D12 Debug Layer!";
			}

			ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
			{
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY::DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
				dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY::DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

				DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
				{
					//IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides.
					80,
				};

				DXGI_INFO_QUEUE_FILTER infoFiter = {};
				infoFiter.DenyList.NumIDs = _countof(hide);
				infoFiter.DenyList.pIDList = hide;
				dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &infoFiter);
			}
		}
#endif // DASH_DEBUG

		//Create DXGI Factory
		ComPtr<IDXGIFactory6> dxgiFactory;
		DX_CALL(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

		//Enumerate adapter and create device
		ComPtr<IDXGIAdapter1> dxgiAdapter;
		for (UINT adapterIndex = 0;
			DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&dxgiAdapter));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			dxgiAdapter->GetDesc1(&desc);

			// Skip the basic render driver adapter.
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}

			if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&FGraphicsCore::Device))))
			{
				LOG_INFO << "Create Device With Adapter : " << FStringUtility::WideStringToUTF8(desc.Description);
				LOG_INFO << "Adapter Memory " << desc.DedicatedVideoMemory / (1024 * 1024) << " MB";

				std::string vendorType = "Adapter Type : ";
				switch (desc.VendorId)
				{
				case VendorID_Nvidia:
					vendorType += "Nvidia";
					break;
				case VendorID_AMD:
					vendorType += "AMD";
					break;
				case VendorID_Intel:
					vendorType += "Intel";
					break;
				default:
					vendorType += "Unknown";
					break;
				}

				LOG_INFO << vendorType;
				break;
			}
		}

		if (FGraphicsCore::Device == nullptr)
		{
			LOG_ERROR << "Failed To Create D3D12 Device!";
		}

#ifndef DASH_RELEASE
		//Enable stable power state
		{
			bool developerModeEnabled = false;

			// Look in the Windows Registry to determine if Developer Mode is enabled
			HKEY hKey;
			LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock", 0, KEY_READ, &hKey);
			if (result == ERROR_SUCCESS)
			{
				DWORD keyValue, keySize = sizeof(DWORD);
				result = RegQueryValueEx(hKey, "AllowDevelopmentWithoutDevLicense", 0, NULL, (byte*)&keyValue, &keySize);
				if (result == ERROR_SUCCESS && keyValue == 1)
					developerModeEnabled = true;
				RegCloseKey(hKey);
			}

			if (developerModeEnabled == false)
			{
				LOG_INFO << "Enable Developer Mode on Windows 10 to get consistent profiling results";
			}

			// Prevent the GPU from overclocking or underclocking to get consistent timings
			if (developerModeEnabled)
				FGraphicsCore::Device->SetStablePowerState(TRUE);
		}
#endif // !DASH_RELEASE

#if DASH_DEBUG
		// Enable debug messages (only works if the debug layer has already been enabled).
		ID3D12InfoQueue* d3dInfoQueue = nullptr;
		if (SUCCEEDED(FGraphicsCore::Device->QueryInterface(IID_PPV_ARGS(&d3dInfoQueue))))
		{
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_ERROR, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_WARNING, true);

			// Suppress messages based on their severity level
			D3D12_MESSAGE_SEVERITY severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID denyIds[] =
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

			D3D12_INFO_QUEUE_FILTER newFilter = {};
			newFilter.DenyList.NumSeverities = _countof(severities);
			newFilter.DenyList.pSeverityList = severities;
			newFilter.DenyList.NumIDs = _countof(denyIds);
			newFilter.DenyList.pIDList = denyIds;

			d3dInfoQueue->PushStorageFilter(&newFilter);
			d3dInfoQueue->Release();
		}
#endif // DASH_DEBUG

		// Check features
		{
			// We like to do read-modify-write operations on UAVs during post processing.  To support that, we
			// need to either have the hardware do typed UAV loads of R11G11B10_FLOAT or we need to manually
			// decode an R32_UINT representation of the same buffer.  This code determines if we get the hardware
			// load support.
			D3D12_FEATURE_DATA_D3D12_OPTIONS featureDataOptions = {};
			if (SUCCEEDED(FGraphicsCore::Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &featureDataOptions, sizeof(featureDataOptions))))
			{
				if (featureDataOptions.TypedUAVLoadAdditionalFormats)
				{
					D3D12_FEATURE_DATA_FORMAT_SUPPORT Support =
					{
						DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE
					};

					if (SUCCEEDED(FGraphicsCore::Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
						(Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
					{
						mTypedUAVLoadSupport_R11G11B10_FLOAT = true;
					}

					Support.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

					if (SUCCEEDED(FGraphicsCore::Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
						(Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
					{
						mTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
					}
				}
			}

			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureDataSignature;
			featureDataSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
			if (FAILED(FGraphicsCore::Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureDataSignature,
				sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE))))
			{
				featureDataSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}
			mHighestRootSignatureVersion = featureDataSignature.HighestVersion;
		}
	}

	void FGraphicsCore::DestroyD3Device()
	{
#if DASH_DEBUG
		ID3D12InfoQueue* d3dInfoQueue = nullptr;
		if (SUCCEEDED(Device->QueryInterface(IID_PPV_ARGS(&d3dInfoQueue))))
		{
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_ERROR, false);
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_WARNING, false);
			d3dInfoQueue->Release();
		}

		ID3D12DebugDevice* debugInterface;
		if (SUCCEEDED(Device->QueryInterface(&debugInterface)))
		{
			debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_FLAGS(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
			debugInterface->Release();
		}
#endif

		if (Device != nullptr)
		{
			Device->Release();
			Device = nullptr;

			LOG_INFO << "Destroy D3D Device.";
		}

#if DASH_DEBUG
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
#endif
	}
}