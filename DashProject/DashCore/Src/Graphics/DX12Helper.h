#pragma once

#include <wrl.h>
#include "d3dx12.h"
#include "../Utility/Image.h"
#include "../Utility/ImageHelper.h"
#include "../Utility/Exception.h"
#include "../DesignPatterns/Singleton.h"


/*
Create Mips
init : 创建 PSO, root signature
being : 创建command allocator, 创建command list
end : 传入 command queue, execute command list
释放  command list, command allocator
*/

namespace Dash
{
    FORCEINLINE uint32_t CountMips(uint32_t width, uint32_t height) noexcept
    {
        if (width == 0 || height == 0)
            return 0;

        uint32_t count = 1;
        while (width > 1 || height > 1)
        {
            width >>= 1;
            height >>= 1;
            count++;
        }
        return count;
    }

    // Helper sets a D3D resource name string (used by PIX and debug layer leak reporting).
    template<UINT TNameLength>
    FORCEINLINE void SetDebugObjectName(_In_ ID3D12DeviceChild* resource, _In_z_ const char(&name)[TNameLength]) noexcept
    {
        //#define CP_UTF8                   65001

        std::string str{ name };
        std::wstring wstr(str.length(), L' '); // Make room for characters

        // Copy string to wstring.
        std::copy(str.begin(), str.end(), wstr.begin());

        resource->SetName(wstr.c_str());

#if !defined(NO_D3D12_DEBUG_NAME) && (defined(_DEBUG) || defined(PROFILE))
        //wchar_t wname[MAX_PATH];
        //int result = MultiByteToWideChar(CP_UTF8, 0, name, TNameLength, wname, MAX_PATH);
        //if (result > 0)
        //{
        //    resource->SetName(wname);
        //}
#else
        UNREFERENCED_PARAMETER(resource);
        UNREFERENCED_PARAMETER(name);
#endif
    }

    template<UINT TNameLength>
    FORCEINLINE void SetDebugObjectName(_In_ ID3D12DeviceChild* resource, _In_z_ const wchar_t(&name)[TNameLength]) noexcept
    {
#if !defined(NO_D3D12_DEBUG_NAME) && (defined(_DEBUG) || defined(PROFILE))
        resource->SetName(name);
#else
        UNREFERENCED_PARAMETER(resource);
        UNREFERENCED_PARAMETER(name);
#endif
    }

    // Shorthand for creating a root signature
    FORCEINLINE HRESULT CreateRootSignature(
        Microsoft::WRL::ComPtr<ID3D12Device>& device,
        const D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc,
        Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature,
        UINT nodeMask = 0) noexcept
    {
        Microsoft::WRL::ComPtr<ID3DBlob> pSignature;
        Microsoft::WRL::ComPtr<ID3DBlob> pError;
        HRESULT hr = D3D12SerializeRootSignature(rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(), pError.GetAddressOf());
        if (SUCCEEDED(hr))
        {
            hr = device->CreateRootSignature(nodeMask, pSignature->GetBufferPointer(), pSignature->GetBufferSize(),
                IID_PPV_ARGS(&rootSignature)
            );
        }
        return hr;
    }

	static void UpdateTextureRegion(Microsoft::WRL::ComPtr<ID3D12Device>& device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList,
        Microsoft::WRL::ComPtr<ID3D12Resource>& destTexture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer,
		const FTexture& texture, UINT destSubresourceIndex = 0)
	{
		D3D12_COMMAND_LIST_TYPE commandListType = commandList->GetType();
		ASSERT((commandListType == D3D12_COMMAND_LIST_TYPE_DIRECT) || (commandListType == D3D12_COMMAND_LIST_TYPE_COPY));

        UINT subresourceIndex = destSubresourceIndex;
        UINT subresourceCount = 1;
        UINT numRows = 0;
        UINT64 rowSizeInByte = 0;
        UINT64 totalByte = 0;
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;

        device->GetCopyableFootprints(
            &(destTexture->GetDesc()),
            subresourceIndex,
            subresourceCount,
            0,
            &footprint,
            &numRows,
            &rowSizeInByte,
            &totalByte
        );

        BYTE* resourceData = nullptr;

        HR(uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&resourceData)));

        size_t textureRowPitch = texture.GetRowPitch();
        const uint8_t* textureData = texture.GetRawData();

        ASSERT(footprint.Footprint.RowPitch == textureRowPitch);
        ASSERT(numRows == texture.GetHeight());
        ASSERT(footprint.Footprint.Width == texture.GetWidth());

        BYTE* resourceCopyStart = resourceData + footprint.Offset;
        for (size_t rowIdx = 0; rowIdx < numRows; rowIdx++)
        {
            memcpy(resourceCopyStart + rowIdx * footprint.Footprint.RowPitch,
                textureData + rowIdx * textureRowPitch,
                textureRowPitch);
        }

        uploadBuffer->Unmap(0, nullptr);

        CD3DX12_TEXTURE_COPY_LOCATION copyDst{ destTexture.Get(), 0 };
        CD3DX12_TEXTURE_COPY_LOCATION copySrc{ uploadBuffer.Get(), footprint };

        commandList->CopyTextureRegion(&copyDst, 0, 0, 0, &copySrc, nullptr);

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(destTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}

    struct FTextureHelper : public TSingleton<FTextureHelper>
    {
        FTextureHelper() {};
        FTextureHelper(Microsoft::WRL::ComPtr<ID3D12Device>& device);

        void Destroy();

        void GenerateMipmap(Microsoft::WRL::ComPtr<ID3D12Resource>& resource);

        HRESULT CreateWICTextureFromFileEx(
            const std::wstring& fileName,
            Microsoft::WRL::ComPtr<ID3D12Resource>& destTexture,
            FTexture& decodeTexture,
            D3D12_RESOURCE_FLAGS resFlags,
            EWIC_LOADER_FLAGS loadFlags);

        HRESULT CreateWICTextureFromMemory(
            Microsoft::WRL::ComPtr<ID3D12Resource>& destTexture,
            FTexture& decodeTexture,
            D3D12_RESOURCE_FLAGS resFlags,
            bool autoGenMips = true);

        HRESULT CreateWICTextureArrayFromFileEx(
            const std::vector<std::wstring>& fileName,
            Microsoft::WRL::ComPtr<ID3D12Resource>& destTexture,
            D3D12_RESOURCE_FLAGS resFlags,
            EWIC_LOADER_FLAGS loadFlags);

        //  Texture Sequence :
        //	POSITIVE_X = 0,
        //  NEGATIVE_X = 1,
        //  POSITIVE_Y = 2,
        //  NEGATIVE_Y = 3,
        //  POSITIVE_Z = 4,
        //  NEGATIVE_Z = 5  
        HRESULT CreateWICTextureCubeFromFileEx(
            const std::vector<std::wstring>& fileName,
            Microsoft::WRL::ComPtr<ID3D12Resource>& destTexture,
            D3D12_RESOURCE_FLAGS resFlags,
            EWIC_LOADER_FLAGS loadFlags);

        HRESULT CreateWICTextureCubeFromFileEx(
            const std::wstring& fileName,
            Microsoft::WRL::ComPtr<ID3D12Resource>& destTexture,
            D3D12_RESOURCE_FLAGS resFlags,
            EWIC_LOADER_FLAGS loadFlags);
        
        HRESULT SaveWICTextureToFile(
            Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQ,
            Microsoft::WRL::ComPtr<ID3D12Resource> pSource,
            REFGUID guidContainerFormat,
            const std::wstring& fileName,
            D3D12_RESOURCE_STATES beforeState,
            D3D12_RESOURCE_STATES afterState,
            const GUID* targetFormat,
            bool forceSRGB);

        HRESULT CreateDDSTextureFromFileEx(
            const std::wstring& fileName,
            Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
            D3D12_RESOURCE_FLAGS resFlags,
            EDDS_LOADER_FLAGS loadFlags,
            DirectX::DDS_ALPHA_MODE* alphaMode = nullptr,
            bool* isCubeMap = nullptr);

        HRESULT __cdecl SaveDDSTextureToFile(
            Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue,
            Microsoft::WRL::ComPtr<ID3D12Resource>& pSource,
            const std::wstring& fileName,
            D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATES afterState = D3D12_RESOURCE_STATE_RENDER_TARGET) noexcept;

        void Upload(
            Microsoft::WRL::ComPtr<ID3D12Resource>& resource,
            uint32_t subresourceIndexStart,
            const D3D12_SUBRESOURCE_DATA* subRes,
            uint32_t numSubresources);

        UINT64 UpdateSubresources(
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCmdList,
            Microsoft::WRL::ComPtr<ID3D12Resource>& pDestinationResource,
            Microsoft::WRL::ComPtr<ID3D12Resource>& pIntermediate,
            UINT64 IntermediateOffset,
            UINT FirstSubresource,
            UINT NumSubresources,
            const D3D12_SUBRESOURCE_DATA* pSrcData) noexcept;

        void Begin();

        void End(Microsoft::WRL::ComPtr<ID3D12CommandQueue>& commandQueue);

    private:

#pragma pack(push, 4)
        struct ConstantData
        {
            FVector2f InvOutTexelSize;
            uint32_t SrcMipIndex = 0;
        };
#pragma pack(pop)

        static const uint32_t Num32BitConstants = static_cast<uint32_t>(sizeof(ConstantData) / sizeof(uint32_t));
        static const uint32_t ThreadGroupSize = 8;

        enum RootParameterIndex
        {
            Constants = 0,
            SourceTexture,
            TargetTexture,
            RootParameterCount
        };

        //--------------------------------------------------------------------------------------
        DXGI_FORMAT GetDDSPixelFormat(const DirectX::DDS_HEADER* header) noexcept;

        bool IsSupportedForGenerateMips(DXGI_FORMAT format) noexcept;

        //--------------------------------------------------------------------------------------
        HRESULT CreateTextureFromDDS(
            const DirectX::DDS_HEADER* header,
            const uint8_t* bitData,
            size_t bitSize,
            size_t maxsize,
            D3D12_RESOURCE_FLAGS resFlags,
            EDDS_LOADER_FLAGS loadFlags,
            Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
            std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
            bool* outIsCubeMap);

        HRESULT FillInitData(
            size_t width,
            size_t height,
            size_t depth,
            size_t mipCount,
            size_t arraySize,
            size_t numberOfPlanes,
            DXGI_FORMAT format,
            size_t maxsize,
            size_t bitSize,
            const uint8_t* bitData,
            size_t& twidth,
            size_t& theight,
            size_t& tdepth,
            size_t& skipMip,
            std::vector<D3D12_SUBRESOURCE_DATA>& initData);

        HRESULT CreateTextureResource(
            D3D12_RESOURCE_DIMENSION resDim,
            size_t width,
            size_t height,
            size_t depth,
            size_t mipCount,
            size_t arraySize,
            DXGI_FORMAT format,
            D3D12_RESOURCE_FLAGS resFlags,
            EDDS_LOADER_FLAGS loadFlags,
            Microsoft::WRL::ComPtr<ID3D12Resource>& texture) noexcept;

        UINT64 UpdateSubresources(
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCmdList,
            Microsoft::WRL::ComPtr<ID3D12Resource>& pDestinationResource,
            Microsoft::WRL::ComPtr<ID3D12Resource>& pIntermediate,
            UINT FirstSubresource,
            UINT NumSubresources,
            UINT64 RequiredSize,
            const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
            const UINT* pNumRows,
            const UINT64* pRowSizesInBytes,
            const D3D12_SUBRESOURCE_DATA* pSrcData) noexcept;

        HRESULT CaptureTexture(
            Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQ,
            Microsoft::WRL::ComPtr<ID3D12Resource>& pSource,
            UINT64 srcPitch,
            const D3D12_RESOURCE_DESC& desc,
            Microsoft::WRL::ComPtr<ID3D12Resource>& pStaging,
            D3D12_RESOURCE_STATES beforeState,
            D3D12_RESOURCE_STATES afterState) noexcept;

        Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateGenMipsRootSignature();

        Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGenMipsPipelineState(
            Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature,
            const void* bytecode,
            size_t bytecodeSize);

        void GenerateMips_UnorderedAccessPath(Microsoft::WRL::ComPtr<ID3D12Resource>& resource);

        // Resource is not UAV compatible
        void GenerateMips_TexturePath(Microsoft::WRL::ComPtr<ID3D12Resource>& resource);

        // Resource is not UAV compatible (copy via alias to avoid validation failure)
        void GenerateMips_TexturePathBGR(Microsoft::WRL::ComPtr<ID3D12Resource>& resource);

        Microsoft::WRL::ComPtr<ID3D12Device> mD3DDevice;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;

        Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState;

        bool mTypedUAVLoadAdditionalFormats = false;
        bool mStandardSwizzle64KBSupported = false;

        std::vector<Microsoft::WRL::ComPtr<ID3D12DeviceChild>>   mTrackedObjects;
    };

}