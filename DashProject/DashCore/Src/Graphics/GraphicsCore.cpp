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
#include "SwapChain.h"
#include "GameApp.h"
#include "Utility/StringUtility.h"
#include "ShaderMap.h"
#include "RenderDevice.h"
#include <shlobj.h>

//#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;

namespace Dash
{ 
	static const uint32_t VendorID_Nvidia = 0x10DE;
	static const uint32_t VendorID_AMD = 0x1002;
	static const uint32_t VendorID_Intel = 0x8086;

	FRenderDevice* FGraphicsCore::Device = nullptr;
	FCommandQueueManager* FGraphicsCore::CommandQueueManager = nullptr;
	FCommandListManager* FGraphicsCore::CommandListManager = nullptr;
	FCpuDescriptorAllocatorManager* FGraphicsCore::DescriptorAllocator = nullptr;
	FCommandContextManager* FGraphicsCore::ContextManager = nullptr;
	FSwapChain* FGraphicsCore::SwapChain = nullptr;

	bool FGraphicsCore::mTypedUAVLoadSupport_R11G11B10_FLOAT = false;
	bool FGraphicsCore::mTypedUAVLoadSupport_R16G16B16A16_FLOAT = false;
	bool FGraphicsCore::mSupportsUniversalHeaps = false;

	D3D_ROOT_SIGNATURE_VERSION FGraphicsCore::mHighestRootSignatureVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	static std::wstring GetLatestWinPixGpuCapturerPath()
	{
		LPWSTR programFilesPath = nullptr;
		SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

		std::filesystem::path pixInstallationPath = programFilesPath;
		pixInstallationPath /= "Microsoft PIX";

		std::wstring newestVersionFound;

		for (auto const& directory_entry : std::filesystem::directory_iterator(pixInstallationPath))
		{
			if (directory_entry.is_directory())
			{
				if (newestVersionFound.empty() || newestVersionFound < directory_entry.path().filename().c_str())
				{
					newestVersionFound = directory_entry.path().filename().c_str();
				}
			}
		}

		if (newestVersionFound.empty())
		{
			// TODO: Error, no PIX installation found
			LOG_ERROR << "No PIX installation found";
		}

		return pixInstallationPath / newestVersionFound / L"WinPixGpuCapturer.dll";
	}

	void FGraphicsCore::Initialize()
	{
		LOG_INFO << "FGraphicsCore::Initialize Begin.";

		EnablePixCapture();

		FShaderMap::Init();
		
		Device = new FRenderDevice();
		CommandQueueManager = new FCommandQueueManager();
		CommandListManager = new FCommandListManager();
		DescriptorAllocator = new FCpuDescriptorAllocatorManager();
		ContextManager = new FCommandContextManager();
		SwapChain = new FSwapChain(IGameApp::GetInstance()->GetWindowWidth(), IGameApp::GetInstance()->GetWindowHeight());

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

		if(SwapChain)
		{
			SwapChain->Destroy();
			delete SwapChain;

			LOG_INFO << "Destroy Swap Chain.";
		}

		FPipelineStateObject::DestroyAll();
		FRootSignature::DestroyAll();
		FSamplerDesc::DestroyAll();

		FShaderMap::Destroy();

		if (Device)
		{
			Device->Destroy();
			delete Device;

			LOG_INFO << "Destroy Render Device.";
		}
	
		LOG_INFO << "FGraphicsCore::Shutdown End.";
	}

	D3D_ROOT_SIGNATURE_VERSION FGraphicsCore::GetRootSignatureVersion()
	{
		return FGraphicsCore::mHighestRootSignatureVersion;
	}
	
	void FGraphicsCore::EnablePixCapture()
	{
		// Check to see if a copy of WinPixGpuCapturer.dll has already been injected into the application.
		// This may happen if the application is launched through the PIX UI. 
		if (GetModuleHandleW(L"WinPixGpuCapturer.dll") == 0)
		{
			LoadLibraryW(GetLatestWinPixGpuCapturerPath().c_str());
		}
	}
}