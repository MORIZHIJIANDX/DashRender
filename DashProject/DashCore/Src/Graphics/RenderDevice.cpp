#include "PCH.h"
#include "RenderDevice.h"
#include "DX12Helper.h"
#include <wrl.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "TextureBuffer.h"
#include "GpuBuffer.h"
#include "CommandContext.h"
#include "SubResourceData.h"
#include "Utility/FileUtility.h"
#include "TextureLoader/TextureLoaderManager.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

using namespace Microsoft::WRL;

namespace Dash
{
	static const uint32_t VendorID_Nvidia = 0x10DE;
	static const uint32_t VendorID_AMD = 0x1002;
	static const uint32_t VendorID_Intel = 0x8086;

	class FMakeColorBuffer : public FColorBuffer
	{
	public:
		FMakeColorBuffer(const std::string& name, ID3D12Resource* resource, EResourceState initStates = EResourceState::Common)
		{	
			Create(name, resource, initStates);
		}

		FMakeColorBuffer(const std::string& name, const FColorBufferDescription& desc, const FLinearColor& clearColor = FLinearColor{})
			: FColorBuffer(clearColor)
		{
			Create(name, desc, clearColor);
		}

		FMakeColorBuffer(const std::string& name, uint32_t width, uint32_t height, uint32_t numMips, EResourceFormat format)
		{
			Create(name, width, height, numMips, format);
		}

		FMakeColorBuffer(const std::string& name, uint32_t width, uint32_t height, uint32_t arrayCount, uint32_t numMips, EResourceFormat format)
		{
			CreateArray(name, width, height, arrayCount, numMips, format);
		}

		virtual ~FMakeColorBuffer() {}
	};

	class FMakeDepthBuffer : public FDepthBuffer
	{
	public:
		FMakeDepthBuffer(const std::string& name, const FDepthBufferDescription& desc)
		{
			Create(name, desc);
		}

		FMakeDepthBuffer(const std::string& name, uint32_t width, uint32_t height, EResourceFormat format)
		{
			Create(name, width, height, 1, 0, format);
		}

		FMakeDepthBuffer(const std::string& name, uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, EResourceFormat format)
		{
			Create(name, width, height, sampleCount, sampleQuality, format);
		}

		virtual ~FMakeDepthBuffer() {}
	};

	class FMakeTextureBuffer : public FTextureBuffer
	{
	public:
		FMakeTextureBuffer(const std::string& name, const FTextureBufferDescription& desc)
		{
			Create(name, desc);
		}

		FMakeTextureBuffer(const std::string& name, uint32_t width, uint32_t height, uint32_t numMips, EResourceFormat format)
		{
			Create(name, width, height, numMips, format);
		}

		FMakeTextureBuffer(const std::string& name, uint32_t width, uint32_t height, uint32_t arrayCount, uint32_t numMips, EResourceFormat format)
		{
			Create(name, width, height, arrayCount, numMips, format);
		}

		virtual ~FMakeTextureBuffer() {}
	};

	class FMakeVertexBuffer : public FGpuVertexBuffer
	{
	public:
		FMakeVertexBuffer(const std::string& name, uint32_t numElements, uint32_t elementSize)
		{
			Create(name, numElements, elementSize);
		}

		virtual ~FMakeVertexBuffer() {}
	};

	class FMakeIndexBuffer : public FGpuIndexBuffer
	{
	public:
		FMakeIndexBuffer(const std::string& name, uint32_t numElements, bool is32Bit = false)
		{
			Create(name, numElements, is32Bit ? sizeof(uint32_t) : sizeof(uint16_t));
			mIs32Bit = is32Bit;
		}

		virtual ~FMakeIndexBuffer() {}
	};

	class FMakeDynamicVertexBuffer : public FGpuDynamicVertexBuffer
	{
	public:
		FMakeDynamicVertexBuffer(const std::string& name, uint32_t numElements, uint32_t elementSize)
		{
			Create(name, numElements, elementSize);
		}

		virtual ~FMakeDynamicVertexBuffer() {}
	};

	class FMakeDynamicIndexBuffer : public FGpuDynamicIndexBuffer
	{
	public:
		FMakeDynamicIndexBuffer(const std::string& name, uint32_t numElements, bool is32Bit = false)
		{
			Create(name, numElements, is32Bit ? sizeof(uint32_t) : sizeof(uint16_t));
			mIs32Bit = is32Bit;
		}

		virtual ~FMakeDynamicIndexBuffer() {}
	};

	class FMakeConstantBuffer : public FGpuConstantBuffer
	{
	public:
		FMakeConstantBuffer(const std::string& name, uint32_t sizeInBytes)
		{
			Create(name, 1, sizeInBytes);
		}

		virtual ~FMakeConstantBuffer() {}
	};

	class FMakeStructuredBuffer : public FStructuredBuffer
	{
	public:
		FMakeStructuredBuffer(const std::string& name, uint32_t numElements, uint32_t elementSize)
		{
			Create(name, numElements, elementSize);
		}

		virtual ~FMakeStructuredBuffer() {}
	};

	FRenderDevice::FRenderDevice()
	{
		if(mDevice == nullptr)
		{
			Init();
		}
	}

	FRenderDevice::~FRenderDevice()
	{
		if (mDevice != nullptr)
		{	
			Destroy();
		}
	}

	void FRenderDevice::Init()
	{
		ASSERT_MSG(mDevice == nullptr, "FGraphicsCore Has Already Been Initialized!");

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

			if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mDevice))))
			{
				LOG_INFO << "Create Device With Adapter : " << FStringUtility::WideStringToUTF8(desc.Description);
				LOG_INFO << "Adapter Memory " << desc.DedicatedVideoMemory / static_cast<size_t>(1024 * 1024) << " MB";

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

		if (mDevice == nullptr)
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
				DWORD keyValue = 0;
				DWORD keySize = sizeof(DWORD);
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
				mDevice->SetStablePowerState(TRUE);
		}
#endif // !DASH_RELEASE

#if DASH_DEBUG
		// Enable debug messages (only works if the debug layer has already been enabled).
		ID3D12InfoQueue* d3dInfoQueue = nullptr;
		if (SUCCEEDED(mDevice->QueryInterface(IID_PPV_ARGS(&d3dInfoQueue))))
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
				//D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

				// Triggered when a shader does not export all color components of a render target, such as
				// when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
				//D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,

				// This occurs when a descriptor table is unbound even when a shader does not access the missing
				// descriptors.  This is common with a root signature shared between disparate shaders that
				// don't all need the same types of resources.
				//D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

				// Support half type input in vertex shader.
				D3D12_MESSAGE_ID_CREATEINPUTLAYOUT_TYPE_MISMATCH,

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
			if (SUCCEEDED(mDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &featureDataOptions, sizeof(featureDataOptions))))
			{
				if (featureDataOptions.TypedUAVLoadAdditionalFormats)
				{
					D3D12_FEATURE_DATA_FORMAT_SUPPORT Support =
					{
						DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE
					};

					if (SUCCEEDED(mDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
						(Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
					{
						mTypedUAVLoadSupport_R11G11B10_FLOAT = true;
					}

					Support.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

					if (SUCCEEDED(mDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
						(Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
					{
						mTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
					}
				}

				if (featureDataOptions.ResourceHeapTier != D3D12_RESOURCE_HEAP_TIER_1)
				{
					mSupportsUniversalHeaps = true;
				}
			}

			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureDataSignature{};
			featureDataSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
			if (FAILED(mDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureDataSignature,
				sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE))))
			{
				featureDataSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}
			mHighestRootSignatureVersion = featureDataSignature.HighestVersion;

			bool allowTearing = false;
			if(SUCCEEDED(dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))))
			{
				mSupportsTearing = allowTearing;
			}
		}
	}

	void FRenderDevice::Destroy()
	{
#if DASH_DEBUG
		ID3D12InfoQueue* d3dInfoQueue = nullptr;
		if (SUCCEEDED(mDevice->QueryInterface(IID_PPV_ARGS(&d3dInfoQueue))))
		{
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_ERROR, false);
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY::D3D12_MESSAGE_SEVERITY_WARNING, false);
			d3dInfoQueue->Release();
		}

		ID3D12DebugDevice* debugInterface;
		if (SUCCEEDED(mDevice->QueryInterface(&debugInterface)))
		{
			debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_FLAGS(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
			debugInterface->Release();
		}
#endif

		if (mDevice != nullptr)
		{
			mDevice = nullptr;
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

	HRESULT FRenderDevice::CheckFeatureSupport(D3D12_FEATURE feature, void* pFeatureSupportData, UINT featureSupportDataSize)
	{
		return mDevice->CheckFeatureSupport(feature, pFeatureSupportData, featureSupportDataSize);
	}

	void FRenderDevice::CopyDescriptors(UINT numDestDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* pDestDescriptorRangeStarts, const UINT* pDestDescriptorRangeSizes, UINT numSrcDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorRangeStarts, const UINT* pSrcDescriptorRangeSizes, D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapsType)
	{
		mDevice->CopyDescriptors(numDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes, numSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes, descriptorHeapsType);
	}

	void FRenderDevice::CopyDescriptorsSimple(UINT numDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptorRangeStart, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptorRangeStart, D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapsType)
	{
		mDevice->CopyDescriptorsSimple(numDescriptors, destDescriptorRangeStart, srcDescriptorRangeStart, descriptorHeapsType);
	}

	HRESULT FRenderDevice::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& pCommandAllocator)
	{
		return mDevice->CreateCommandAllocator(type, IID_PPV_ARGS(&pCommandAllocator));
	}

	HRESULT FRenderDevice::CreateCommandList(UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pInitialState, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList1>& pCommandList)
	{
		return mDevice->CreateCommandList(nodeMask, type, pCommandAllocator, pInitialState, IID_PPV_ARGS(&pCommandList));
	}

	HRESULT FRenderDevice::CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC* pDesc, Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue)
	{
		return mDevice->CreateCommandQueue(pDesc, IID_PPV_ARGS(&pCommandQueue));
	}

	HRESULT FRenderDevice::CreateCommandSignature(const D3D12_COMMAND_SIGNATURE_DESC* pDesc, ID3D12RootSignature* pRootSignature, Microsoft::WRL::ComPtr<ID3D12CommandSignature>& pvCommandSignature)
	{
		return mDevice->CreateCommandSignature(pDesc, pRootSignature, IID_PPV_ARGS(&pvCommandSignature));
	}

	HRESULT FRenderDevice::CreateCommittedResource(const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES initialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, Microsoft::WRL::ComPtr<ID3D12Resource>& pvResource)
	{
		return mDevice->CreateCommittedResource(pHeapProperties, heapFlags, pDesc, initialResourceState, pOptimizedClearValue, IID_PPV_ARGS(&pvResource));
	}

	HRESULT FRenderDevice::CreateComputePipelineState(const D3D12_COMPUTE_PIPELINE_STATE_DESC* pDesc, Microsoft::WRL::ComPtr<ID3D12PipelineState>& pPipelineState)
	{
		return mDevice->CreateComputePipelineState(pDesc, IID_PPV_ARGS(&pPipelineState));
	}

	void FRenderDevice::CreateConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		mDevice->CreateConstantBufferView(pDesc, destDescriptor);
	}

	void FRenderDevice::CreateDepthStencilView(ID3D12Resource* pResource, const D3D12_DEPTH_STENCIL_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		mDevice->CreateDepthStencilView(pResource, pDesc, destDescriptor);
	}

	HRESULT FRenderDevice::CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC* pDescriptorHeapDesc, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& ppvHeap)
	{
		return mDevice->CreateDescriptorHeap(pDescriptorHeapDesc, IID_PPV_ARGS(&ppvHeap));
	}

	HRESULT FRenderDevice::CreateFence(UINT64 initialValue, D3D12_FENCE_FLAGS flags, Microsoft::WRL::ComPtr<ID3D12Fence>& ppFence)
	{
		return mDevice->CreateFence(initialValue, flags, IID_PPV_ARGS(&ppFence));
	}

	HRESULT FRenderDevice::CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* pDesc, Microsoft::WRL::ComPtr<ID3D12PipelineState>& ppPipelineState)
	{
		return mDevice->CreateGraphicsPipelineState(pDesc, IID_PPV_ARGS(&ppPipelineState));
	}

	HRESULT FRenderDevice::CreatePipelineState(const D3D12_PIPELINE_STATE_STREAM_DESC* pDesc, Microsoft::WRL::ComPtr<ID3D12PipelineState>& ppPipelineState)
	{
		return mDevice->CreatePipelineState(pDesc, IID_PPV_ARGS(&ppPipelineState));
	}

	HRESULT FRenderDevice::CreateHeap(const D3D12_HEAP_DESC* pDesc, Microsoft::WRL::ComPtr<ID3D12Heap>& ppvHeap)
	{
		return mDevice->CreateHeap(pDesc, IID_PPV_ARGS(&ppvHeap));
	}

	HRESULT FRenderDevice::CreatePlacedResource(ID3D12Heap* pHeap, UINT64 heapOffset, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, Microsoft::WRL::ComPtr<ID3D12Resource>& pvResource)
	{
		return mDevice->CreatePlacedResource(pHeap, heapOffset, pDesc, initialState, pOptimizedClearValue, IID_PPV_ARGS(&pvResource));
	}

	HRESULT FRenderDevice::CreateQueryHeap(const D3D12_QUERY_HEAP_DESC* pDesc, Microsoft::WRL::ComPtr<ID3D12QueryHeap>& ppvHeap)
	{
		return mDevice->CreateQueryHeap(pDesc, IID_PPV_ARGS(&ppvHeap));
	}

	void FRenderDevice::CreateRenderTargetView(ID3D12Resource* pResource, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		mDevice->CreateRenderTargetView(pResource, pDesc, destDescriptor);
	}

	HRESULT FRenderDevice::CreateReservedResource(const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, Microsoft::WRL::ComPtr<ID3D12Resource>& pvResource)
	{
		return mDevice->CreateReservedResource(pDesc, initialState, pOptimizedClearValue, IID_PPV_ARGS(&pvResource));
	}

	HRESULT FRenderDevice::CreateRootSignature(UINT nodeMask, const void* pBlobWithRootSignature, SIZE_T blobLengthInBytes, Microsoft::WRL::ComPtr<ID3D12RootSignature>& pvRootSignature)
	{
		return mDevice->CreateRootSignature(nodeMask, pBlobWithRootSignature, blobLengthInBytes, IID_PPV_ARGS(&pvRootSignature));
	}

	void FRenderDevice::CreateSampler(const D3D12_SAMPLER_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		mDevice->CreateSampler(pDesc, destDescriptor);
	}

	void FRenderDevice::CreateShaderResourceView(ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		mDevice->CreateShaderResourceView(pResource, pDesc, destDescriptor);
	}

	HRESULT FRenderDevice::CreateSharedHandle(ID3D12DeviceChild* pObject, const SECURITY_ATTRIBUTES* pAttributes, DWORD access, LPCWSTR name, HANDLE* pHandle)
	{
		return mDevice->CreateSharedHandle(pObject, pAttributes, access, name, pHandle);
	}

	void FRenderDevice::CreateUnorderedAccessView(ID3D12Resource* pResource, ID3D12Resource* pCounterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		mDevice->CreateUnorderedAccessView(pResource, pCounterResource, pDesc, destDescriptor);
	}

	HRESULT FRenderDevice::Evict(UINT numObjects, ID3D12Pageable* const* ppObjects)
	{
		return mDevice->Evict(numObjects, ppObjects);
	}

	LUID FRenderDevice::GetAdapterLuid()
	{
		return mDevice->GetAdapterLuid();
	}

	void FRenderDevice::GetCopyableFootprints(const D3D12_RESOURCE_DESC* pResourceDesc, UINT firstSubresource, UINT numSubresources, UINT64 baseOffset, D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts, UINT* pNumRows, UINT64* pRowSizeInBytes, UINT64* pTotalBytes)
	{
		mDevice->GetCopyableFootprints(pResourceDesc, firstSubresource, numSubresources, baseOffset, pLayouts, pNumRows, pRowSizeInBytes, pTotalBytes);
	}

	D3D12_HEAP_PROPERTIES FRenderDevice::GetCustomHeapProperties(UINT nodeMask, D3D12_HEAP_TYPE heapType)
	{
		return mDevice->GetCustomHeapProperties(nodeMask, heapType);
	}

	UINT FRenderDevice::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType)
	{
		return mDevice->GetDescriptorHandleIncrementSize(descriptorHeapType);
	}

	HRESULT FRenderDevice::GetDeviceRemovedReason()
	{
		return mDevice->GetDeviceRemovedReason();
	}

	UINT FRenderDevice::GetNodeCount()
	{
		return mDevice->GetNodeCount();
	}

	D3D12_RESOURCE_ALLOCATION_INFO FRenderDevice::GetResourceAllocationInfo(UINT visibleMask, UINT numResourceDescs, const D3D12_RESOURCE_DESC* pResourceDescs)
	{
		return mDevice->GetResourceAllocationInfo(visibleMask, numResourceDescs, pResourceDescs);
	}

	void FRenderDevice::GetResourceTiling(ID3D12Resource* pTiledResource, UINT* pNumTilesForEntireResource, D3D12_PACKED_MIP_INFO* pPackedMipDesc, D3D12_TILE_SHAPE* pStandardTileShapeForNonPackedMips, UINT* pNumSubresourceTilings, UINT firstSubresourceTilingToGet, D3D12_SUBRESOURCE_TILING* pSubresourceTilingsForNonPackedMips)
	{
		mDevice->GetResourceTiling(pTiledResource, pNumTilesForEntireResource, pPackedMipDesc, pStandardTileShapeForNonPackedMips, pNumSubresourceTilings, firstSubresourceTilingToGet, pSubresourceTilingsForNonPackedMips);
	}

	HRESULT FRenderDevice::MakeResident(UINT NumObjects, ID3D12Pageable* const* ppObjects)
	{
		return mDevice->MakeResident(NumObjects, ppObjects);
	}

	HRESULT FRenderDevice::SetStablePowerState(BOOL enable)
	{
		return mDevice->SetStablePowerState(enable);
	}

	FColorBufferRef FRenderDevice::CreateColorBuffer(const std::string& name, ID3D12Resource* resource, EResourceState initStates /*= EResourceState::Common*/)
	{
		std::shared_ptr<FMakeColorBuffer> bufferRef = std::make_shared<FMakeColorBuffer>(name, resource, initStates);
		return bufferRef;
	}

	FColorBufferRef FRenderDevice::CreateColorBuffer(const std::string& name, const FColorBufferDescription& desc, const FLinearColor& clearColor /*= FLinearColor{}*/)
	{
		std::shared_ptr<FMakeColorBuffer> bufferRef = std::make_shared<FMakeColorBuffer>(name, desc, clearColor);
		return bufferRef;
	}

	FColorBufferRef FRenderDevice::CreateColorBuffer(const std::string& name, uint32_t width, uint32_t height, uint32_t numMips, EResourceFormat format)
	{
		std::shared_ptr<FMakeColorBuffer> bufferRef = std::make_shared<FMakeColorBuffer>(name, width, height, numMips, format);
		return bufferRef;
	}

	FColorBufferRef FRenderDevice::CreateColorBufferArray(const std::string& name, uint32_t width, uint32_t height, uint32_t arrayCount, uint32_t numMips, EResourceFormat format)
	{
		std::shared_ptr<FMakeColorBuffer> bufferRef = std::make_shared<FMakeColorBuffer>(name, width, height, arrayCount, numMips, format);
		return bufferRef;
	}

	FDepthBufferRef FRenderDevice::CreateDepthBuffer(const std::string& name, const FDepthBufferDescription& desc)
	{
		std::shared_ptr<FMakeDepthBuffer> bufferRef = std::make_shared<FMakeDepthBuffer>(name, desc);
		return bufferRef;
	}

	FDepthBufferRef FRenderDevice::CreateDepthBuffer(const std::string& name, uint32_t width, uint32_t height, EResourceFormat format)
	{
		std::shared_ptr<FMakeDepthBuffer> bufferRef = std::make_shared<FMakeDepthBuffer>(name, width, height, format);
		return bufferRef;
	}

	FDepthBufferRef FRenderDevice::CreateDepthBuffer(const std::string& name, uint32_t width, uint32_t height, uint32_t sampleCount, uint32_t sampleQuality, EResourceFormat format)
	{
		std::shared_ptr<FMakeDepthBuffer> bufferRef = std::make_shared<FMakeDepthBuffer>(name, width, height, sampleCount, sampleQuality, format);
		return bufferRef;
	}

	FTextureBufferRef FRenderDevice::CreateTextureBufferFromMemory(const std::string& name, const FTextureBufferDescription& desc, const std::vector<const void*>& initialMipsData)
	{
		std::shared_ptr<FTextureBuffer> bufferRef = std::make_shared<FMakeTextureBuffer>(name, desc);

		ASSERT(desc.MipCount == initialMipsData.size());

		std::vector<FSubResourceData> subResources;

		for (size_t i = 0; i < desc.MipCount; i++)
		{
			size_t rowPitch = 0;
			size_t slicePitch = 0;

			desc.GetPitch(rowPitch, slicePitch, i);

			subResources.emplace_back(initialMipsData[i], rowPitch, slicePitch);
		}
		
		FCopyCommandContext::UpdateTextureBuffer(bufferRef, 0, static_cast<uint32_t>(subResources.size()), subResources.data());
		return bufferRef;
	}

	FTextureBufferRef FRenderDevice::CreateTextureBufferFromFile(const std::string& name, const std::string& fileName)
	{
		if(FFileUtility::IsPathExistent(fileName))
		{
			std::string fileExtension = FFileUtility::GetFileExtension(fileName);

			bool loadSucceed = false;

			const FImportedTextureData& textureData = FTextureLoaderManager::Get().LoadTexture(fileName);

			if (loadSucceed)
			{
				std::shared_ptr<FTextureBuffer> bufferRef = std::make_shared<FMakeTextureBuffer>(name, textureData.TextureDescription);

				FCopyCommandContext::UpdateTextureBuffer(bufferRef, 0, static_cast<uint32_t>(textureData.SubResource.size()), textureData.SubResource.data());

				FTextureLoaderManager::Get().UnloadTexture(fileName);

				return bufferRef;
			}
		}
		
		return FTextureBufferRef();
	}

	FGpuVertexBufferRef FRenderDevice::CreateVertexBuffer(const std::string& name, uint32_t numElements, uint32_t elementSize, const void* initData)
	{
		std::shared_ptr<FMakeVertexBuffer> bufferRef = std::make_shared<FMakeVertexBuffer>(name, numElements, elementSize);
		if (initData != nullptr)
		{
			FCopyCommandContext::InitializeBuffer(bufferRef, initData, bufferRef->GetBufferSize());
		}
		return bufferRef;
	}

	FGpuIndexBufferRef FRenderDevice::CreateIndexBuffer(const std::string& name, uint32_t numElements, const void* initData, bool is32Bit)
	{
		std::shared_ptr<FMakeIndexBuffer> bufferRef = std::make_shared<FMakeIndexBuffer>(name, numElements, is32Bit);
		if (initData != nullptr)
		{
			FCopyCommandContext::InitializeBuffer(bufferRef, initData, bufferRef->GetBufferSize());
		}
		return bufferRef;
	}

	FGpuDynamicVertexBufferRef FRenderDevice::CreateDynamicVertexBuffer(const std::string& name, uint32_t numElements, uint32_t elementSize)
	{
		std::shared_ptr<FMakeDynamicVertexBuffer> bufferRef = std::make_shared<FMakeDynamicVertexBuffer>(name, numElements, elementSize);
		return bufferRef;
	}

	FGpuDynamicIndexBufferRef FRenderDevice::CreateDynamicIndexBuffer(const std::string& name, uint32_t numElements, bool is32Bit)
	{
		std::shared_ptr<FMakeDynamicIndexBuffer> bufferRef = std::make_shared<FMakeDynamicIndexBuffer>(name, numElements, is32Bit);
		return bufferRef;
	}

	FGpuConstantBufferRef FRenderDevice::CreateConstantBuffer(const std::string& name, uint32_t dataSize, const void* initData)
	{
		std::shared_ptr<FMakeConstantBuffer> bufferRef = std::make_shared<FMakeConstantBuffer>(name, dataSize);
		if (initData != nullptr)
		{
			FCopyCommandContext::InitializeBuffer(bufferRef, initData, bufferRef->GetBufferSize());
		}
		return bufferRef;
	}

	FStructuredBufferRef FRenderDevice::CreateStructuredBuffer(const std::string& name, uint32_t numElements, uint32_t elementSize, const void* initData)
	{
		std::shared_ptr<FStructuredBuffer> bufferRef = std::make_shared<FMakeStructuredBuffer>(name, numElements, elementSize);
		if (initData != nullptr)
		{
			FCopyCommandContext::InitializeBuffer(bufferRef, initData, bufferRef->GetBufferSize());
		}
		return bufferRef;
	}

}
