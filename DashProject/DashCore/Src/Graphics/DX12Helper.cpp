#include "PCH.h"


namespace Dash
{

}


//#include "DX12Helper.h"
//#include <D3Dcompiler.h>
/*

namespace Dash
{
    bool FormatIsUAVCompatible(_In_ ID3D12Device* device, bool typedUAVLoadAdditionalFormats, DXGI_FORMAT format) noexcept
    {
        switch (format)
        {
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
            // Unconditionally supported.
            return true;

        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SINT:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SINT:
            // All these are supported if this optional feature is set.
            return typedUAVLoadAdditionalFormats;

        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_A8_UNORM:
        case DXGI_FORMAT_B5G6R5_UNORM:
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            // Conditionally supported by specific devices.
            if (typedUAVLoadAdditionalFormats)
            {
                D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = { format, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE };
                if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport))))
                {
                    const DWORD mask = D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD | D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE;
                    return ((formatSupport.Support2 & mask) == mask);
                }
            }
            return false;

        default:
            return false;
        }
    }

    bool FormatIsBGR(DXGI_FORMAT format) noexcept
    {
        switch (format)
        {
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            return true;
        default:
            return false;
        }
    }

    bool FormatIsSRGB(DXGI_FORMAT format) noexcept
    {
        switch (format)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            return true;
        default:
            return false;
        }
    }

    DXGI_FORMAT ConvertSRVtoResourceFormat(DXGI_FORMAT format) noexcept
    {
        switch (format)
        {
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            return DXGI_FORMAT_R32G32B32A32_TYPELESS;

        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
            return DXGI_FORMAT_R16G16B16A16_TYPELESS;

        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
            return DXGI_FORMAT_R32G32_TYPELESS;

        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
            return DXGI_FORMAT_R10G10B10A2_TYPELESS;

        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
            return DXGI_FORMAT_R8G8B8A8_TYPELESS;

        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
            return DXGI_FORMAT_R16G16_TYPELESS;

        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
            return DXGI_FORMAT_R32_TYPELESS;

        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
            return DXGI_FORMAT_R8G8_TYPELESS;

        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
            return DXGI_FORMAT_R16_TYPELESS;

        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
            return DXGI_FORMAT_R8_TYPELESS;

        default:
            return format;
        }
    } 

    FTextureHelper::FTextureHelper(Microsoft::WRL::ComPtr<ID3D12Device>& device)
        : mD3DDevice(device)
    {
        Microsoft::WRL::ComPtr<ID3DBlob> generateMipsShader;

        UINT shaderCompileFlag = 0;

#ifdef DASH_DEBUG
        shaderCompileFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // DASH_DEBUG

        shaderCompileFlag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

        std::filesystem::path currentPath = std::filesystem::current_path();
        //std::filesystem::path shaderPath = currentPath / "src\\resources\\generateMips.hlsl";  DashCore\Src\Resources

        std::filesystem::path shaderPath = currentPath.parent_path() / "DashCore\\Src\\Resources\\generateMips.hlsl";

        DX_CALL(D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "CSMain", "cs_5_0", shaderCompileFlag, 0, &generateMipsShader, nullptr));

        mRootSignature = CreateGenMipsRootSignature();
        mPipelineState = CreateGenMipsPipelineState(mRootSignature, generateMipsShader->GetBufferPointer(), generateMipsShader->GetBufferSize());

        D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
        if (SUCCEEDED(device->CheckFeatureSupport(
            D3D12_FEATURE_D3D12_OPTIONS,
            &options,
            sizeof(options))))
        {
            mTypedUAVLoadAdditionalFormats = options.TypedUAVLoadAdditionalFormats != 0;
            mStandardSwizzle64KBSupported = options.StandardSwizzle64KBSupported != 0;
        }
    }

    void FTextureHelper::Destroy()
    {
        mTrackedObjects.clear();

        mD3DDevice.Reset();
        mCommandList.Reset();
        mCommandAllocator.Reset();

        mRootSignature.Reset();
        mPipelineState.Reset();
    }

    HRESULT FTextureHelper::CreateWICTextureFromFileEx(
        const std::wstring& fileName,
        Microsoft::WRL::ComPtr<ID3D12Resource>& destTexture,
        FTexture& decodeTexture,
        D3D12_RESOURCE_FLAGS resFlags,
        EWIC_LOADER_FLAGS loadFlags)
    {
        if (destTexture)
        {
            destTexture.Reset();
        }

        std::filesystem::path filePath{fileName};

        if (!std::filesystem::exists(filePath))
        {
            ASSERT_FAIL("File: %s not exists!", filePath.c_str());
            return E_INVALIDARG;
        }

        decodeTexture = LoadWICTexture(fileName, loadFlags);

        UINT textureWidth = (UINT)decodeTexture.GetWidth();
        UINT textureHeight = (UINT)decodeTexture.GetHeight();

        // Count the number of mips
        uint32_t mipCount = Valid(loadFlags & (EWIC_LOADER_FLAGS::WIC_LOADER_MIP_AUTOGEN | EWIC_LOADER_FLAGS::WIC_LOADER_MIP_RESERVE))
            ? CountMips(textureWidth, textureHeight) : 1u;

        // Create texture
        D3D12_RESOURCE_DESC desc = {};
        desc.Width = textureWidth;
        desc.Height = textureHeight;
        desc.MipLevels = static_cast<UINT16>(mipCount);
        desc.DepthOrArraySize = 1;
        desc.Format = DashFormatToDXGIFormat(decodeTexture.GetFormat());
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Flags = resFlags;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        //ID3D12Resource* tex = nullptr;
        HRESULT hr = mD3DDevice->CreateCommittedResource(
            &defaultHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&destTexture));

        destTexture->SetName(filePath.filename().c_str());

        if (FAILED(hr))
        {
            return hr;
        }

        ASSERT(destTexture != nullptr);

        D3D12_SUBRESOURCE_DATA subresource;
        subresource.pData = decodeTexture.GetRawData();
        subresource.RowPitch = static_cast<LONG>(decodeTexture.GetRowPitch());
        subresource.SlicePitch = static_cast<LONG>(decodeTexture.GetRowPitch() * textureHeight);

        Upload(destTexture, 0, &subresource, 1);

        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(destTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

        // Generate mips?
        if (Valid(loadFlags & EWIC_LOADER_FLAGS::WIC_LOADER_MIP_AUTOGEN))
        {
            GenerateMipmap(destTexture);
        }

        return hr;
    }

    HRESULT FTextureHelper::CreateWICTextureFromMemory(
        Microsoft::WRL::ComPtr<ID3D12Resource>& destTexture,
        FTexture& decodeTexture,
        D3D12_RESOURCE_FLAGS resFlags,
        bool autoGenMips)
    {
        UINT textureWidth = (UINT)decodeTexture.GetWidth();
        UINT textureHeight = (UINT)decodeTexture.GetHeight();

        // Count the number of mips
        uint32_t mipCount = autoGenMips
            ? CountMips(textureWidth, textureHeight) : 1u;

        // Create texture
        D3D12_RESOURCE_DESC desc = {};
        desc.Width = textureWidth;
        desc.Height = textureHeight;
        desc.MipLevels = static_cast<UINT16>(mipCount);
        desc.DepthOrArraySize = 1;
        desc.Format = DashFormatToDXGIFormat(decodeTexture.GetFormat());
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Flags = resFlags;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        HRESULT hr = mD3DDevice->CreateCommittedResource(
            &defaultHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&destTexture));

        if (FAILED(hr))
        {
            return hr;
        }

        ASSERT(destTexture != nullptr);

        D3D12_SUBRESOURCE_DATA subresource;
        subresource.pData = decodeTexture.GetRawData();
        subresource.RowPitch = static_cast<LONG>(decodeTexture.GetRowPitch());
        subresource.SlicePitch = static_cast<LONG>(decodeTexture.GetRowPitch() * textureHeight);

        Upload(destTexture, 0, &subresource, 1);

        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(destTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

        // Generate mips?
        if (autoGenMips)
        {
            GenerateMipmap(destTexture);
        }

        return hr;
    }

    HRESULT FTextureHelper::CreateWICTextureArrayFromFileEx(const std::vector<std::wstring>& fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& destTexture, D3D12_RESOURCE_FLAGS resFlags, EWIC_LOADER_FLAGS loadFlags)
    {
        if (fileName.size() <= 0)
        {
            return E_INVALIDARG;
        }

        UINT arraySize = (UINT)fileName.size();

        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> textures;

        textures.resize(arraySize);

        FTexture tempDecodeTex;

        HRESULT hr;

        for (size_t i = 0; i < arraySize; i++)
        {
            hr = CreateWICTextureFromFileEx(
                fileName[i],
                textures[i],
                tempDecodeTex,
                resFlags,
                loadFlags
                );
        }

        for (size_t i = 0; i < arraySize; i++)
        {
            mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textures[i].Get(),
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                D3D12_RESOURCE_STATE_COPY_SOURCE));

            mTrackedObjects.push_back(textures[i]);
        }

        D3D12_RESOURCE_DESC soruceDesc = textures[0]->GetDesc();

        // Create texture
        D3D12_RESOURCE_DESC desc = {};
        desc.Width = soruceDesc.Width;
        desc.Height = soruceDesc.Height;
        desc.MipLevels = soruceDesc.MipLevels;
        desc.DepthOrArraySize = arraySize;
        desc.Format = soruceDesc.Format;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Flags = resFlags;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        hr = mD3DDevice->CreateCommittedResource(
            &defaultHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&destTexture));

        UINT mipLevels = soruceDesc.MipLevels;

        for (UINT arrayIndex = 0; arrayIndex < arraySize; arrayIndex++)
        {
            for (UINT mipLevel = 0; mipLevel < mipLevels; mipLevel++)
            {
                UINT destSubResourceIndex = D3D12CalcSubresource(mipLevel, arrayIndex, 0, mipLevels, arraySize);

                CD3DX12_TEXTURE_COPY_LOCATION Dst(destTexture.Get(), destSubResourceIndex);
                CD3DX12_TEXTURE_COPY_LOCATION Src(textures[arrayIndex].Get(), mipLevel);
                mCommandList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
            }
        }

        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(destTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

        return hr;
    }

    HRESULT FTextureHelper::CreateWICTextureCubeFromFileEx(const std::vector<std::wstring>& fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& destTexture, D3D12_RESOURCE_FLAGS resFlags, EWIC_LOADER_FLAGS loadFlags)
    {
        ASSERT(fileName.size() == 6);

        return CreateWICTextureArrayFromFileEx(fileName, destTexture, resFlags, loadFlags);
    }

    HRESULT FTextureHelper::CreateWICTextureCubeFromFileEx(const std::wstring& fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& destTexture, D3D12_RESOURCE_FLAGS resFlags, EWIC_LOADER_FLAGS loadFlags)
    {
        FTexture tempDecodeTex;

        HRESULT hr;

        Microsoft::WRL::ComPtr<ID3D12Resource> loadTexture;

        hr = CreateWICTextureFromFileEx(
            fileName,
            loadTexture,
            tempDecodeTex,
            resFlags,
            loadFlags
        );

        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(loadTexture.Get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_COPY_SOURCE));

        mTrackedObjects.push_back(loadTexture);

        D3D12_RESOURCE_DESC soruceDesc = loadTexture->GetDesc();

        UINT squareLength = (UINT)soruceDesc.Width / 4;

        // Create texture
        D3D12_RESOURCE_DESC desc = {};
        desc.Width = squareLength;
        desc.Height = squareLength;
        desc.MipLevels = Valid(loadFlags | EWIC_LOADER_FLAGS::WIC_LOADER_MIP_AUTOGEN) ? (soruceDesc.MipLevels - 2) : 1;
        desc.DepthOrArraySize = 6;
        desc.Format = soruceDesc.Format;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Flags = resFlags;
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        hr = mD3DDevice->CreateCommittedResource(
            &defaultHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&destTexture));

        UINT mipLevels = desc.MipLevels;

        D3D12_BOX box;
        // box坐标轴如下: 
        //    front
        //   / 
        //  /_____right
        //  |
        //  |
        //  bottom
        box.front = 0;
        box.back = 1;

        enum D3D11_TEXTURECUBE_FACE {
            TEXTURECUBE_FACE_POSITIVE_X = 0,
            TEXTURECUBE_FACE_NEGATIVE_X = 1,
            TEXTURECUBE_FACE_POSITIVE_Y = 2,
            TEXTURECUBE_FACE_NEGATIVE_Y = 3,
            TEXTURECUBE_FACE_POSITIVE_Z = 4,
            TEXTURECUBE_FACE_NEGATIVE_Z = 5
        };

        for (UINT mipLevel = 0; mipLevel < mipLevels; mipLevel++)
        {
            CD3DX12_TEXTURE_COPY_LOCATION Src(loadTexture.Get(), mipLevel);

            // +X面拷贝
            box.left = squareLength * 2;
            box.top = squareLength;
            box.right = squareLength * 3;
            box.bottom = squareLength * 2;

            UINT destSubResourceIndex = D3D12CalcSubresource(mipLevel, TEXTURECUBE_FACE_POSITIVE_X, 0, mipLevels, 6);

            CD3DX12_TEXTURE_COPY_LOCATION xPositiveDst(destTexture.Get(), destSubResourceIndex);     
            mCommandList->CopyTextureRegion(&xPositiveDst, 0, 0, 0, &Src, &box);

            // -X面拷贝
            box.left = 0;
            box.top = squareLength;
            box.right = squareLength;
            box.bottom = squareLength * 2;

            destSubResourceIndex = D3D12CalcSubresource(mipLevel, TEXTURECUBE_FACE_NEGATIVE_X, 0, mipLevels, 6);

            CD3DX12_TEXTURE_COPY_LOCATION xNegativeDst(destTexture.Get(), destSubResourceIndex);
            mCommandList->CopyTextureRegion(&xNegativeDst, 0, 0, 0, &Src, &box);

            // +Y面拷贝
            box.left = squareLength;
            box.top = 0;
            box.right = squareLength * 2;
            box.bottom = squareLength;

            destSubResourceIndex = D3D12CalcSubresource(mipLevel, TEXTURECUBE_FACE_POSITIVE_Y, 0, mipLevels, 6);

            CD3DX12_TEXTURE_COPY_LOCATION yPositiveDst(destTexture.Get(), destSubResourceIndex);
            mCommandList->CopyTextureRegion(&yPositiveDst, 0, 0, 0, &Src, &box);


            // -Y面拷贝
            box.left = squareLength;
            box.top = squareLength * 2;
            box.right = squareLength * 2;
            box.bottom = squareLength * 3;

            destSubResourceIndex = D3D12CalcSubresource(mipLevel, TEXTURECUBE_FACE_NEGATIVE_Y, 0, mipLevels, 6);

            CD3DX12_TEXTURE_COPY_LOCATION yNegativeDst(destTexture.Get(), destSubResourceIndex);
            mCommandList->CopyTextureRegion(&yNegativeDst, 0, 0, 0, &Src, &box);

            // +Z面拷贝
            box.left = squareLength;
            box.top = squareLength;
            box.right = squareLength * 2;
            box.bottom = squareLength * 2;

            destSubResourceIndex = D3D12CalcSubresource(mipLevel, TEXTURECUBE_FACE_POSITIVE_Z, 0, mipLevels, 6);

            CD3DX12_TEXTURE_COPY_LOCATION zPositiveDst(destTexture.Get(), destSubResourceIndex);
            mCommandList->CopyTextureRegion(&zPositiveDst, 0, 0, 0, &Src, &box);

            // -Z面拷贝
            box.left = squareLength * 3;
            box.top = squareLength;
            box.right = squareLength * 4;
            box.bottom = squareLength * 2;

            destSubResourceIndex = D3D12CalcSubresource(mipLevel, TEXTURECUBE_FACE_NEGATIVE_Z, 0, mipLevels, 6);

            CD3DX12_TEXTURE_COPY_LOCATION zNegativeDst(destTexture.Get(), destSubResourceIndex);
            mCommandList->CopyTextureRegion(&zNegativeDst, 0, 0, 0, &Src, &box);

            // 下一个mipLevel的纹理宽高都是原来的1/2
            squareLength /= 2;
        }

        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(destTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

        return hr;
    }

    HRESULT FTextureHelper::SaveWICTextureToFile(
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQ,
        Microsoft::WRL::ComPtr<ID3D12Resource> pSource,
        REFGUID guidContainerFormat, 
        const std::wstring& fileName, 
        D3D12_RESOURCE_STATES beforeState, 
        D3D12_RESOURCE_STATES afterState, 
        const GUID* targetFormat, 
        bool forceSRGB)
    {
        // Get the size of the image
        const auto desc = pSource->GetDesc();

        if (desc.Width > UINT32_MAX)
            return E_INVALIDARG;

        UINT64 totalResourceSize = 0;
        UINT64 fpRowPitch = 0;
        UINT fpRowCount = 0;
        // Get the rowcount, pitch and size of the top mip
        mD3DDevice->GetCopyableFootprints(
            &desc,
            0,
            1,
            0,
            nullptr,
            &fpRowCount,
            &fpRowPitch,
            &totalResourceSize);

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
        // Round up the srcPitch to multiples of 1024
        UINT64 dstRowPitch = (fpRowPitch + static_cast<uint64_t>(D3D12XBOX_TEXTURE_DATA_PITCH_ALIGNMENT) - 1u) & ~(static_cast<uint64_t>(D3D12XBOX_TEXTURE_DATA_PITCH_ALIGNMENT) - 1u);
#else
        // Round up the srcPitch to multiples of 256
        UINT64 dstRowPitch = (fpRowPitch + 255) & ~0xFFu;
#endif

        if (dstRowPitch > UINT32_MAX)
            return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

        Microsoft::WRL::ComPtr<ID3D12Resource> pStaging;
        HRESULT hr = CaptureTexture(pCommandQ, pSource, dstRowPitch, desc, pStaging, beforeState, afterState);
        if (FAILED(hr))
            return hr;

        // Determine source format's WIC equivalent
        WICPixelFormatGUID pfGuid = {};
        bool sRGB = forceSRGB;
        switch (desc.Format)
        {
        case DXGI_FORMAT_R32G32B32A32_FLOAT:            pfGuid = GUID_WICPixelFormat128bppRGBAFloat; break;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:            pfGuid = GUID_WICPixelFormat64bppRGBAHalf; break;
        case DXGI_FORMAT_R16G16B16A16_UNORM:            pfGuid = GUID_WICPixelFormat64bppRGBA; break;
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:    pfGuid = GUID_WICPixelFormat32bppRGBA1010102XR; break;
        case DXGI_FORMAT_R10G10B10A2_UNORM:             pfGuid = GUID_WICPixelFormat32bppRGBA1010102; break;
        case DXGI_FORMAT_B5G5R5A1_UNORM:                pfGuid = GUID_WICPixelFormat16bppBGRA5551; break;
        case DXGI_FORMAT_B5G6R5_UNORM:                  pfGuid = GUID_WICPixelFormat16bppBGR565; break;
        case DXGI_FORMAT_R32_FLOAT:                     pfGuid = GUID_WICPixelFormat32bppGrayFloat; break;
        case DXGI_FORMAT_R16_FLOAT:                     pfGuid = GUID_WICPixelFormat16bppGrayHalf; break;
        case DXGI_FORMAT_R16_UNORM:                     pfGuid = GUID_WICPixelFormat16bppGray; break;
        case DXGI_FORMAT_R8_UNORM:                      pfGuid = GUID_WICPixelFormat8bppGray; break;
        case DXGI_FORMAT_A8_UNORM:                      pfGuid = GUID_WICPixelFormat8bppAlpha; break;

        case DXGI_FORMAT_R8G8B8A8_UNORM:
            pfGuid = GUID_WICPixelFormat32bppRGBA;
            break;

        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            pfGuid = GUID_WICPixelFormat32bppRGBA;
            sRGB = true;
            break;

        case DXGI_FORMAT_B8G8R8A8_UNORM:
            pfGuid = GUID_WICPixelFormat32bppBGRA;
            break;

        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            pfGuid = GUID_WICPixelFormat32bppBGRA;
            sRGB = true;
            break;

        case DXGI_FORMAT_B8G8R8X8_UNORM:
            pfGuid = GUID_WICPixelFormat32bppBGR;
            break;

        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            pfGuid = GUID_WICPixelFormat32bppBGR;
            sRGB = true;
            break;

        default:
            LOG_ERROR << "ERROR: ScreenGrab does not support all DXGI formats " << static_cast<uint32_t>(desc.Format) <<". Consider using DirectXTex.";
            return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        }

        auto pWIC = _GetWIC();
        if (!pWIC)
            return E_NOINTERFACE;

        Microsoft::WRL::ComPtr<IWICStream> stream;
        hr = pWIC->CreateStream(stream.GetAddressOf());
        if (FAILED(hr))
            return hr;

        hr = stream->InitializeFromFilename(fileName.c_str(), GENERIC_WRITE);
        if (FAILED(hr))
            return hr;

        AutoDeleteFileWic delonfail(stream, fileName.c_str());

        Microsoft::WRL::ComPtr<IWICBitmapEncoder> encoder;
        hr = pWIC->CreateEncoder(guidContainerFormat, nullptr, encoder.GetAddressOf());
        if (FAILED(hr))
            return hr;

        hr = encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache);
        if (FAILED(hr))
            return hr;

        Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> frame;
        Microsoft::WRL::ComPtr<IPropertyBag2> props;
        hr = encoder->CreateNewFrame(frame.GetAddressOf(), props.GetAddressOf());
        if (FAILED(hr))
            return hr;

        if (targetFormat && memcmp(&guidContainerFormat, &GUID_ContainerFormatBmp, sizeof(WICPixelFormatGUID)) == 0)
        {
            // Opt-in to the WIC2 support for writing 32-bit Windows BMP files with an alpha channel
            PROPBAG2 option = {};
            option.pstrName = const_cast<wchar_t*>(L"EnableV5Header32bppBGRA");

            VARIANT varValue;
            varValue.vt = VT_BOOL;
            varValue.boolVal = VARIANT_TRUE;
            (void)props->Write(1, &option, &varValue);
        }

        //if (setCustomProps)
        //{
        //    setCustomProps(props.Get());
        //}

        hr = frame->Initialize(props.Get());
        if (FAILED(hr))
            return hr;

        hr = frame->SetSize(static_cast<UINT>(desc.Width), desc.Height);
        if (FAILED(hr))
            return hr;

        hr = frame->SetResolution(72, 72);
        if (FAILED(hr))
            return hr;

        // Pick a target format
        WICPixelFormatGUID targetGuid = {};
        if (targetFormat)
        {
            targetGuid = *targetFormat;
        }
        else
        {
            // Screenshots don't typically include the alpha channel of the render target
            switch (desc.Format)
            {
            case DXGI_FORMAT_R32G32B32A32_FLOAT:
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                targetGuid = GUID_WICPixelFormat96bppRGBFloat; // WIC 2
                break;

            case DXGI_FORMAT_R16G16B16A16_UNORM: targetGuid = GUID_WICPixelFormat48bppBGR; break;
            case DXGI_FORMAT_B5G5R5A1_UNORM:     targetGuid = GUID_WICPixelFormat16bppBGR555; break;
            case DXGI_FORMAT_B5G6R5_UNORM:       targetGuid = GUID_WICPixelFormat16bppBGR565; break;

            case DXGI_FORMAT_R32_FLOAT:
            case DXGI_FORMAT_R16_FLOAT:
            case DXGI_FORMAT_R16_UNORM:
            case DXGI_FORMAT_R8_UNORM:
            case DXGI_FORMAT_A8_UNORM:
                targetGuid = GUID_WICPixelFormat8bppGray;
                break;

            default:
                targetGuid = GUID_WICPixelFormat24bppBGR;
                break;
            }
        }

        hr = frame->SetPixelFormat(&targetGuid);
        if (FAILED(hr))
            return hr;

        if (targetFormat && memcmp(targetFormat, &targetGuid, sizeof(WICPixelFormatGUID)) != 0)
        {
            // Requested output pixel format is not supported by the WIC codec
            return E_FAIL;
        }

        // Encode WIC metadata
        Microsoft::WRL::ComPtr<IWICMetadataQueryWriter> metawriter;
        if (SUCCEEDED(frame->GetMetadataQueryWriter(metawriter.GetAddressOf())))
        {
            PROPVARIANT value;
            PropVariantInit(&value);

            value.vt = VT_LPSTR;
            value.pszVal = const_cast<char*>("DirectXTK");

            if (memcmp(&guidContainerFormat, &GUID_ContainerFormatPng, sizeof(GUID)) == 0)
            {
                // Set Software name
                (void)metawriter->SetMetadataByName(L"/tEXt/{str=Software}", &value);

                // Set sRGB chunk
                if (sRGB)
                {
                    value.vt = VT_UI1;
                    value.bVal = 0;
                    (void)metawriter->SetMetadataByName(L"/sRGB/RenderingIntent", &value);
                }
                else
                {
                    // add gAMA chunk with gamma 1.0
                    value.vt = VT_UI4;
                    value.uintVal = 100000; // gama value * 100,000 -- i.e. gamma 1.0
                    (void)metawriter->SetMetadataByName(L"/gAMA/ImageGamma", &value);

                    // remove sRGB chunk which is added by default.
                    (void)metawriter->RemoveMetadataByName(L"/sRGB/RenderingIntent");
                }
            }
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
            else if (memcmp(&guidContainerFormat, &GUID_ContainerFormatJpeg, sizeof(GUID)) == 0)
            {
                // Set Software name
                (void)metawriter->SetMetadataByName(L"/app1/ifd/{ushort=305}", &value);

                if (sRGB)
                {
                    // Set EXIF Colorspace of sRGB
                    value.vt = VT_UI2;
                    value.uiVal = 1;
                    (void)metawriter->SetMetadataByName(L"/app1/ifd/exif/{ushort=40961}", &value);
                }
            }
            else if (memcmp(&guidContainerFormat, &GUID_ContainerFormatTiff, sizeof(GUID)) == 0)
            {
                // Set Software name
                (void)metawriter->SetMetadataByName(L"/ifd/{ushort=305}", &value);

                if (sRGB)
                {
                    // Set EXIF Colorspace of sRGB
                    value.vt = VT_UI2;
                    value.uiVal = 1;
                    (void)metawriter->SetMetadataByName(L"/ifd/exif/{ushort=40961}", &value);
                }
            }
#else
            else
            {
                // Set Software name
                (void)metawriter->SetMetadataByName(L"System.ApplicationName", &value);

                if (sRGB)
                {
                    // Set EXIF Colorspace of sRGB
                    value.vt = VT_UI2;
                    value.uiVal = 1;
                    (void)metawriter->SetMetadataByName(L"System.Image.ColorSpace", &value);
                }
            }
#endif
        }

        UINT64 imageSize = dstRowPitch * UINT64(desc.Height);
        if (imageSize > UINT32_MAX)
            return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

        void* pMappedMemory = nullptr;
        D3D12_RANGE readRange = { 0, static_cast<SIZE_T>(imageSize) };
        D3D12_RANGE writeRange = { 0, 0 };
        hr = pStaging->Map(0, &readRange, &pMappedMemory);
        if (FAILED(hr))
            return hr;

        if (memcmp(&targetGuid, &pfGuid, sizeof(WICPixelFormatGUID)) != 0)
        {
            // Conversion required to write
            Microsoft::WRL::ComPtr<IWICBitmap> source;
            hr = pWIC->CreateBitmapFromMemory(static_cast<UINT>(desc.Width), desc.Height,
                pfGuid,
                static_cast<UINT>(dstRowPitch), static_cast<UINT>(imageSize),
                static_cast<BYTE*>(pMappedMemory), source.GetAddressOf());
            if (FAILED(hr))
            {
                pStaging->Unmap(0, &writeRange);
                return hr;
            }

            Microsoft::WRL::ComPtr<IWICFormatConverter> FC;
            hr = pWIC->CreateFormatConverter(FC.GetAddressOf());
            if (FAILED(hr))
            {
                pStaging->Unmap(0, &writeRange);
                return hr;
            }

            BOOL canConvert = FALSE;
            hr = FC->CanConvert(pfGuid, targetGuid, &canConvert);
            if (FAILED(hr) || !canConvert)
            {
                pStaging->Unmap(0, &writeRange);
                return E_UNEXPECTED;
            }

            hr = FC->Initialize(source.Get(), targetGuid, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeMedianCut);
            if (FAILED(hr))
            {
                pStaging->Unmap(0, &writeRange);
                return hr;
            }

            WICRect rect = { 0, 0, static_cast<INT>(desc.Width), static_cast<INT>(desc.Height) };
            hr = frame->WriteSource(FC.Get(), &rect);
        }
        else
        {
            // No conversion required
            hr = frame->WritePixels(desc.Height, static_cast<UINT>(dstRowPitch), static_cast<UINT>(imageSize), static_cast<BYTE*>(pMappedMemory));
        }

        pStaging->Unmap(0, &writeRange);

        if (FAILED(hr))
            return hr;

        hr = frame->Commit();
        if (FAILED(hr))
            return hr;

        hr = encoder->Commit();
        if (FAILED(hr))
            return hr;

        delonfail.clear();

        return S_OK;
    }

    HRESULT FTextureHelper::CreateDDSTextureFromFileEx(
        const std::wstring& fileName,
        Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
        D3D12_RESOURCE_FLAGS resFlags,
        EDDS_LOADER_FLAGS loadFlags,
        DirectX::DDS_ALPHA_MODE* alphaMode,
        bool* isCubeMap)
    {
        if (texture)
        {
            texture.Reset();
        }

        if (alphaMode)
        {
            *alphaMode = DirectX::DDS_ALPHA_MODE_UNKNOWN;
        }
        if (isCubeMap)
        {
            *isCubeMap = false;
        }

        std::filesystem::path filePath{ fileName };

        if (!std::filesystem::exists(filePath))
        {
            ASSERT_FAIL("File: %s not exists!", filePath.c_str());
            return E_INVALIDARG;
        }

        const DirectX::DDS_HEADER* header = nullptr;
        const uint8_t* bitData = nullptr;
        size_t bitSize = 0;

        std::unique_ptr<uint8_t[]> ddsData;
        HRESULT hr = LoadDDSTextureDataFromFile(fileName,
            ddsData,
            &header,
            &bitData,
            &bitSize
        );

        if (FAILED(hr))
        {
            return hr;
        }

        if (loadFlags & DDS_LOADER_MIP_AUTOGEN)
        {
            DXGI_FORMAT fmt = GetDDSPixelFormat(header);
            if (!IsSupportedForGenerateMips(fmt))
            {
                ASSERT_FAIL("WARNING: Autogen of mips ignored (device doesn't support this format (%d) or trying to use a copy queue)\n", static_cast<int>(fmt));
                loadFlags = static_cast<EDDS_LOADER_FLAGS>(loadFlags & (~DDS_LOADER_MIP_AUTOGEN));
            }
        }

        std::vector<D3D12_SUBRESOURCE_DATA> subresources;
        hr = CreateTextureFromDDS(
            header, bitData, bitSize, 65536,
            resFlags, loadFlags,
            texture, subresources, isCubeMap);

        if (SUCCEEDED(hr))
        {
            texture->SetName(filePath.filename().c_str());

            if (alphaMode)
                *alphaMode = GetAlphaMode(header);

            Upload(texture, 0, subresources.data(), static_cast<UINT>(subresources.size()));

            mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

            // If it's missing mips, let's generate them
            if ((loadFlags & DDS_LOADER_MIP_AUTOGEN) && subresources.size() != texture->GetDesc().MipLevels)
            {
                GenerateMipmap(texture);
            }
        }

        return hr;
    }

    HRESULT __cdecl FTextureHelper::SaveDDSTextureToFile(
        Microsoft::WRL::ComPtr<ID3D12CommandQueue>& pCommandQueue, 
        Microsoft::WRL::ComPtr<ID3D12Resource>& pSource, 
        const std::wstring& fileName, 
        D3D12_RESOURCE_STATES beforeState, 
        D3D12_RESOURCE_STATES afterState) noexcept
    {
        //Microsoft::WRL::ComPtr<ID3D12Device> mD3DDevice;
        //pCommandQ->GetDevice(IID(mD3DDevice.GetAddressOf()));

        // Get the size of the image
        const auto desc = pSource->GetDesc();

        if (desc.Width > UINT32_MAX)
            return E_INVALIDARG;

        UINT64 totalResourceSize = 0;
        UINT64 fpRowPitch = 0;
        UINT fpRowCount = 0;
        // Get the rowcount, pitch and size of the top mip
        mD3DDevice->GetCopyableFootprints(
            &desc,
            0,
            1,
            0,
            nullptr,
            &fpRowCount,
            &fpRowPitch,
            &totalResourceSize);

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
        // Round up the srcPitch to multiples of 1024
        UINT64 dstRowPitch = (fpRowPitch + static_cast<uint64_t>(D3D12XBOX_TEXTURE_DATA_PITCH_ALIGNMENT) - 1u) & ~(static_cast<uint64_t>(D3D12XBOX_TEXTURE_DATA_PITCH_ALIGNMENT) - 1u);
#else
        // Round up the srcPitch to multiples of 256
        UINT64 dstRowPitch = (fpRowPitch + 255) & ~0xFFu;
#endif

        if (dstRowPitch > UINT32_MAX)
            return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

        Microsoft::WRL::ComPtr<ID3D12Resource> pStaging;
        HRESULT hr = CaptureTexture(pCommandQueue, pSource, dstRowPitch, desc, pStaging, beforeState, afterState);
        if (FAILED(hr))
            return hr;

        // Create file
        ScopedHandle hFile(safe_handle(CreateFile2(fileName.c_str(), GENERIC_WRITE, 0, CREATE_ALWAYS, nullptr)));
        if (!hFile)
            return HRESULT_FROM_WIN32(GetLastError());

        AutoDeleteFile delonfail(hFile.get());

        // Setup header
        const size_t MAX_HEADER_SIZE = sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER) + sizeof(DirectX::DDS_HEADER_DXT10);
        uint8_t fileHeader[MAX_HEADER_SIZE] = {};

        *reinterpret_cast<uint32_t*>(&fileHeader[0]) = DirectX::DDS_MAGIC;

        auto header = reinterpret_cast<DirectX::DDS_HEADER*>(&fileHeader[0] + sizeof(uint32_t));
        size_t headerSize = sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER);
        header->size = sizeof(DirectX::DDS_HEADER);
        header->flags = DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_MIPMAP;
        header->height = desc.Height;
        header->width = static_cast<uint32_t>(desc.Width);
        header->mipMapCount = 1;
        header->caps = DDS_SURFACE_FLAGS_TEXTURE;

        // Try to use a legacy .DDS pixel format for better tools support, otherwise fallback to 'DX10' header extension
        DirectX::DDS_HEADER_DXT10* extHeader = nullptr;
        switch (desc.Format)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:        memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_A8B8G8R8, sizeof(DirectX::DDS_PIXELFORMAT));    break;
        case DXGI_FORMAT_R16G16_UNORM:          memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_G16R16, sizeof(DirectX::DDS_PIXELFORMAT));      break;
        case DXGI_FORMAT_R8G8_UNORM:            memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_A8L8, sizeof(DirectX::DDS_PIXELFORMAT));        break;
        case DXGI_FORMAT_R16_UNORM:             memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_L16, sizeof(DirectX::DDS_PIXELFORMAT));         break;
        case DXGI_FORMAT_R8_UNORM:              memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_L8, sizeof(DirectX::DDS_PIXELFORMAT));          break;
        case DXGI_FORMAT_A8_UNORM:              memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_A8, sizeof(DirectX::DDS_PIXELFORMAT));          break;
        case DXGI_FORMAT_R8G8_B8G8_UNORM:       memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_R8G8_B8G8, sizeof(DirectX::DDS_PIXELFORMAT));   break;
        case DXGI_FORMAT_G8R8_G8B8_UNORM:       memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_G8R8_G8B8, sizeof(DirectX::DDS_PIXELFORMAT));   break;
        case DXGI_FORMAT_BC1_UNORM:             memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_DXT1, sizeof(DirectX::DDS_PIXELFORMAT));        break;
        case DXGI_FORMAT_BC2_UNORM:             memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_DXT3, sizeof(DirectX::DDS_PIXELFORMAT));        break;
        case DXGI_FORMAT_BC3_UNORM:             memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_DXT5, sizeof(DirectX::DDS_PIXELFORMAT));        break;
        case DXGI_FORMAT_BC4_UNORM:             memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_BC4_UNORM, sizeof(DirectX::DDS_PIXELFORMAT));   break;
        case DXGI_FORMAT_BC4_SNORM:             memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_BC4_SNORM, sizeof(DirectX::DDS_PIXELFORMAT));   break;
        case DXGI_FORMAT_BC5_UNORM:             memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_BC5_UNORM, sizeof(DirectX::DDS_PIXELFORMAT));   break;
        case DXGI_FORMAT_BC5_SNORM:             memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_BC5_SNORM, sizeof(DirectX::DDS_PIXELFORMAT));   break;
        case DXGI_FORMAT_B5G6R5_UNORM:          memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_R5G6B5, sizeof(DirectX::DDS_PIXELFORMAT));      break;
        case DXGI_FORMAT_B5G5R5A1_UNORM:        memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_A1R5G5B5, sizeof(DirectX::DDS_PIXELFORMAT));    break;
        case DXGI_FORMAT_R8G8_SNORM:            memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_V8U8, sizeof(DirectX::DDS_PIXELFORMAT));        break;
        case DXGI_FORMAT_R8G8B8A8_SNORM:        memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_Q8W8V8U8, sizeof(DirectX::DDS_PIXELFORMAT));    break;
        case DXGI_FORMAT_R16G16_SNORM:          memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_V16U16, sizeof(DirectX::DDS_PIXELFORMAT));      break;
        case DXGI_FORMAT_B8G8R8A8_UNORM:        memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_A8R8G8B8, sizeof(DirectX::DDS_PIXELFORMAT));    break;
        case DXGI_FORMAT_B8G8R8X8_UNORM:        memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_X8R8G8B8, sizeof(DirectX::DDS_PIXELFORMAT));    break;
        case DXGI_FORMAT_YUY2:                  memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_YUY2, sizeof(DirectX::DDS_PIXELFORMAT));        break;
        case DXGI_FORMAT_B4G4R4A4_UNORM:        memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_A4R4G4B4, sizeof(DirectX::DDS_PIXELFORMAT));    break;

            // Legacy D3DX formats using D3DFMT enum value as FourCC
        case DXGI_FORMAT_R32G32B32A32_FLOAT:    header->ddspf.size = sizeof(DirectX::DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 116; break; // D3DFMT_A32B32G32R32F
        case DXGI_FORMAT_R16G16B16A16_FLOAT:    header->ddspf.size = sizeof(DirectX::DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 113; break; // D3DFMT_A16B16G16R16F
        case DXGI_FORMAT_R16G16B16A16_UNORM:    header->ddspf.size = sizeof(DirectX::DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 36;  break; // D3DFMT_A16B16G16R16
        case DXGI_FORMAT_R16G16B16A16_SNORM:    header->ddspf.size = sizeof(DirectX::DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 110; break; // D3DFMT_Q16W16V16U16
        case DXGI_FORMAT_R32G32_FLOAT:          header->ddspf.size = sizeof(DirectX::DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 115; break; // D3DFMT_G32R32F
        case DXGI_FORMAT_R16G16_FLOAT:          header->ddspf.size = sizeof(DirectX::DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 112; break; // D3DFMT_G16R16F
        case DXGI_FORMAT_R32_FLOAT:             header->ddspf.size = sizeof(DirectX::DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 114; break; // D3DFMT_R32F
        case DXGI_FORMAT_R16_FLOAT:             header->ddspf.size = sizeof(DirectX::DDS_PIXELFORMAT); header->ddspf.flags = DDS_FOURCC; header->ddspf.fourCC = 111; break; // D3DFMT_R16F

        case DXGI_FORMAT_AI44:
        case DXGI_FORMAT_IA44:
        case DXGI_FORMAT_P8:
        case DXGI_FORMAT_A8P8:
            ASSERT("ERROR: ScreenGrab does not support video textures. Consider using DirectXTex.\n");
            return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

        default:
            memcpy_s(&header->ddspf, sizeof(header->ddspf), &DirectX::DDSPF_DX10, sizeof(DirectX::DDS_PIXELFORMAT));

            headerSize += sizeof(DirectX::DDS_HEADER_DXT10);
            extHeader = reinterpret_cast<DirectX::DDS_HEADER_DXT10*>(fileHeader + sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER));
            extHeader->dxgiFormat = desc.Format;
            extHeader->resourceDimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            extHeader->arraySize = 1;
            break;
        }

        size_t rowPitch, slicePitch, rowCount;
        hr = GetSurfaceInfo(static_cast<size_t>(desc.Width), desc.Height, desc.Format, &slicePitch, &rowPitch, &rowCount);
        if (FAILED(hr))
            return hr;

        if (rowPitch > UINT32_MAX || slicePitch > UINT32_MAX)
            return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

        if (IsCompressed(desc.Format))
        {
            header->flags |= DDS_HEADER_FLAGS_LINEARSIZE;
            header->pitchOrLinearSize = static_cast<uint32_t>(slicePitch);
        }
        else
        {
            header->flags |= DDS_HEADER_FLAGS_PITCH;
            header->pitchOrLinearSize = static_cast<uint32_t>(rowPitch);
        }

        // Setup pixels
        std::unique_ptr<uint8_t[]> pixels(new (std::nothrow) uint8_t[slicePitch]);
        if (!pixels)
            return E_OUTOFMEMORY;

        assert(fpRowCount == rowCount);
        assert(fpRowPitch == rowPitch);

        UINT64 imageSize = dstRowPitch * UINT64(rowCount);
        if (imageSize > UINT32_MAX)
            return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

        void* pMappedMemory = nullptr;
        D3D12_RANGE readRange = { 0, static_cast<SIZE_T>(imageSize) };
        D3D12_RANGE writeRange = { 0, 0 };
        hr = pStaging->Map(0, &readRange, &pMappedMemory);
        if (FAILED(hr))
            return hr;

        auto sptr = static_cast<const uint8_t*>(pMappedMemory);
        if (!sptr)
        {
            pStaging->Unmap(0, &writeRange);
            return E_POINTER;
        }

        uint8_t* dptr = pixels.get();

        size_t msize = std::min<size_t>(rowPitch, size_t(dstRowPitch));
        for (size_t h = 0; h < rowCount; ++h)
        {
            memcpy_s(dptr, rowPitch, sptr, msize);
            sptr += dstRowPitch;
            dptr += rowPitch;
        }

        pStaging->Unmap(0, &writeRange);

        // Write header & pixels
        DWORD bytesWritten;
        if (!WriteFile(hFile.get(), fileHeader, static_cast<DWORD>(headerSize), &bytesWritten, nullptr))
            return HRESULT_FROM_WIN32(GetLastError());

        if (bytesWritten != headerSize)
            return E_FAIL;

        if (!WriteFile(hFile.get(), pixels.get(), static_cast<DWORD>(slicePitch), &bytesWritten, nullptr))
            return HRESULT_FROM_WIN32(GetLastError());

        if (bytesWritten != slicePitch)
            return E_FAIL;

        delonfail.clear();

        return S_OK;
    }

    void FTextureHelper::Upload(
        Microsoft::WRL::ComPtr<ID3D12Resource>& resource, 
        uint32_t subresourceIndexStart, 
        const D3D12_SUBRESOURCE_DATA* subRes, 
        uint32_t numSubresources)
    {
        D3D12_COMMAND_LIST_TYPE commandListType = mCommandList->GetType();
        ASSERT((commandListType == D3D12_COMMAND_LIST_TYPE_DIRECT) || (commandListType == D3D12_COMMAND_LIST_TYPE_COPY));

        UINT64 uploadBufferSize = GetRequiredIntermediateSize(resource.Get(), subresourceIndexStart, numSubresources);

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

        // Create a temporary buffer
        Microsoft::WRL::ComPtr<ID3D12Resource> scratchResource = nullptr;
        DX_CALL(mD3DDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr, // D3D12_CLEAR_VALUE* pOptimizedClearValue
            IID_PPV_ARGS(scratchResource.GetAddressOf())));

        SetDebugObjectName(scratchResource.Get(), L"ResourceUploadBatch Temporary");

        UpdateSubresources(mCommandList, resource, scratchResource, 0, subresourceIndexStart, numSubresources, subRes);

        mTrackedObjects.push_back(scratchResource);
    }

    UINT64 FTextureHelper::UpdateSubresources(
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCmdList,
        Microsoft::WRL::ComPtr<ID3D12Resource>& pDestinationResource,
        Microsoft::WRL::ComPtr<ID3D12Resource>& pIntermediate, 
        UINT64 IntermediateOffset,
        UINT FirstSubresource,
        UINT NumSubresources,
        const D3D12_SUBRESOURCE_DATA* pSrcData) noexcept
    {
        UINT64 RequiredSize = 0;
        UINT64 MemToAlloc = static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * NumSubresources;
        if (MemToAlloc > SIZE_MAX)
        {
            return 0;
        }
        void* pMem = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(MemToAlloc));
        if (pMem == nullptr)
        {
            return 0;
        }
        auto pLayouts = static_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(pMem);
        //allocate NumSubresources D3D12_PLACED_SUBRESOURCE_FOOTPRINT
        UINT64* pRowSizesInBytes = reinterpret_cast<UINT64*>(pLayouts + NumSubresources);
        //allocate NumSubresources RowSize Array
        //allocate NumSubresources NumRow Array
        UINT* pNumRows = reinterpret_cast<UINT*>(pRowSizesInBytes + NumSubresources);

        auto Desc = pDestinationResource->GetDesc();
        mD3DDevice->GetCopyableFootprints(&Desc, FirstSubresource, NumSubresources, IntermediateOffset, pLayouts, pNumRows, pRowSizesInBytes, &RequiredSize);
        mD3DDevice->Release();

        UINT64 Result = UpdateSubresources(pCmdList, pDestinationResource, pIntermediate, FirstSubresource, NumSubresources, RequiredSize, pLayouts, pNumRows, pRowSizesInBytes, pSrcData);
        HeapFree(GetProcessHeap(), 0, pMem);
        return Result;
    }


    void FTextureHelper::Begin()
    {
        DX_CALL(mD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));

        SetDebugObjectName(mCommandAllocator.Get(), L"Mips Command Allocator");

        DX_CALL(mD3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), mPipelineState.Get(), IID_PPV_ARGS(&mCommandList)));

        SetDebugObjectName(mCommandList.Get(), L"Mips Command List");
    }

    void FTextureHelper::End(Microsoft::WRL::ComPtr<ID3D12CommandQueue>& commandQueue)
    {
        DX_CALL(mCommandList->Close());

        ID3D12CommandList* commands[] = { mCommandList.Get() };
        commandQueue->ExecuteCommandLists(_countof(commands), commands);

        // Set an event so we get notified when the GPU has completed all its work
        Microsoft::WRL::ComPtr<ID3D12Fence> fence;
        DX_CALL(mD3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));

        SetDebugObjectName(fence.Get(), L"Mips Fence");

        HANDLE gpuCompletedEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
        if (!gpuCompletedEvent)
            throw std::exception("CreateEventEx");

        DX_CALL(commandQueue->Signal(fence.Get(), 1ULL));
        DX_CALL(fence->SetEventOnCompletion(1ULL, gpuCompletedEvent));

        // Wait on the GPU-complete notification
        DWORD wr = WaitForSingleObject(gpuCompletedEvent, INFINITE);
        if (wr != WAIT_OBJECT_0)
        {
            if (wr == WAIT_FAILED)
            {
                DX_CALL(HRESULT_FROM_WIN32(GetLastError()));
            }
            else
            {
                throw std::exception("WaitForSingleObject");
            }
        }

        mCommandList.Reset();
        mCommandAllocator.Reset();

        mTrackedObjects.clear();
    }

    void FTextureHelper::GenerateMipmap(Microsoft::WRL::ComPtr<ID3D12Resource>& resource)
    {
        const D3D12_RESOURCE_DESC desc = resource->GetDesc();

        if (desc.MipLevels == 1)
        {
            // Nothing to do 
            return;
        }
        if (desc.MipLevels == 0)
        {
            throw std::exception("GenerateMips: texture has no mips");
        }
        if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D)
        {
            throw std::exception("GenerateMips only supports Texture2D resources");
        }
        if (desc.DepthOrArraySize != 1)
        {
            throw std::exception("GenerateMips only supports 2D textures of array size 1");
        }

        {
            bool uavCompat = FormatIsUAVCompatible(mD3DDevice.Get(), mTypedUAVLoadAdditionalFormats, desc.Format);

            if (!uavCompat && !FormatIsSRGB(desc.Format) && !FormatIsBGR(desc.Format))
            {
                throw std::exception("GenerateMips doesn't support this texture format on this device");
            }

            if (uavCompat)
            {
                GenerateMips_UnorderedAccessPath(resource);
            }
            else if (FormatIsBGR(desc.Format))
            {
                GenerateMips_TexturePathBGR(resource);
            }
            else
            {
                GenerateMips_TexturePath(resource);
            }
        }
    }

    DXGI_FORMAT FTextureHelper::GetDDSPixelFormat(const DirectX::DDS_HEADER* header) noexcept
    {
        if ((header->ddspf.flags & DDS_FOURCC) &&
            (MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC))
        {
            auto d3d10ext = reinterpret_cast<const DirectX::DDS_HEADER_DXT10*>(reinterpret_cast<const char*>(header) + sizeof(DirectX::DDS_HEADER));
            return d3d10ext->dxgiFormat;
        }
        else
            return DirectX::GetDXGIFormat(header->ddspf);
    }

    bool FTextureHelper::IsSupportedForGenerateMips(DXGI_FORMAT format) noexcept
    {
        //if (mCommandType == D3D12_COMMAND_LIST_TYPE_COPY)
        //    return false;

        if (FormatIsUAVCompatible(mD3DDevice.Get(), mTypedUAVLoadAdditionalFormats, format))
            return true;

        if (FormatIsBGR(format))
        {
#if defined(_GAMING_XBOX) || (defined(_XBOX_ONE) && defined(_TITLE))
            // We know the RGB and BGR memory layouts match for Xbox One
            return true;
#else
            // BGR path requires DXGI_FORMAT_R8G8B8A8_UNORM support for UAV load/store plus matching layouts
            return mTypedUAVLoadAdditionalFormats && mStandardSwizzle64KBSupported;
#endif
        }

        if (FormatIsSRGB(format))
        {
            // sRGB path requires DXGI_FORMAT_R8G8B8A8_UNORM support for UAV load/store
            return mTypedUAVLoadAdditionalFormats;
        }

        return false;
    }



    HRESULT FTextureHelper::CreateTextureFromDDS(
        const DirectX::DDS_HEADER* header, 
        const uint8_t* bitData, 
        size_t bitSize, 
        size_t maxsize, 
        D3D12_RESOURCE_FLAGS resFlags, 
        EDDS_LOADER_FLAGS loadFlags, 
        Microsoft::WRL::ComPtr<ID3D12Resource>& texture, 
        std::vector<D3D12_SUBRESOURCE_DATA>& subresources, 
        bool* outIsCubeMap)
    {
        HRESULT hr = S_OK;

        UINT width = header->width;
        UINT height = header->height;
        UINT depth = header->depth;

        D3D12_RESOURCE_DIMENSION resDim = D3D12_RESOURCE_DIMENSION_UNKNOWN;
        UINT arraySize = 1;
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        bool isCubeMap = false;

        size_t mipCount = header->mipMapCount;
        if (0 == mipCount)
        {
            mipCount = 1;
        }

        if ((header->ddspf.flags & DDS_FOURCC) &&
            (MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC))
        {
            auto d3d10ext = reinterpret_cast<const DirectX::DDS_HEADER_DXT10*>(reinterpret_cast<const char*>(header) + sizeof(DirectX::DDS_HEADER));

            arraySize = d3d10ext->arraySize;
            if (arraySize == 0)
            {
                return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
            }

            switch (d3d10ext->dxgiFormat)
            {
            case DXGI_FORMAT_AI44:
            case DXGI_FORMAT_IA44:
            case DXGI_FORMAT_P8:
            case DXGI_FORMAT_A8P8:
                ASSERT_FAIL("ERROR: DDSTextureLoader does not support video textures. Consider using DirectXTex instead.\n");
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

            default:
                if (BitsPerPixel(d3d10ext->dxgiFormat) == 0)
                {
                    ASSERT_FAIL("ERROR: Unknown DXGI format (%u)\n", static_cast<uint32_t>(d3d10ext->dxgiFormat));
                    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
                }
                break;
            }

            format = d3d10ext->dxgiFormat;

            switch (d3d10ext->resourceDimension)
            {
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
                // D3DX writes 1D textures with a fixed Height of 1
                if ((header->flags & DDS_HEIGHT) && height != 1)
                {
                    return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
                }
                height = depth = 1;
                break;

            case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
                if (d3d10ext->miscFlag & 0x4 ) // RESOURCE_MISC_TEXTURECUBE 
                {
                    arraySize *= 6;
                    isCubeMap = true;
                }
                depth = 1;
                break;

            case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
                if (!(header->flags & DDS_HEADER_FLAGS_VOLUME))
                {
                    return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
                }

                if (arraySize > 1)
                {
                    ASSERT_FAIL("ERROR: Volume textures are not texture arrays\n");
                    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
                }
                break;

            case D3D12_RESOURCE_DIMENSION_BUFFER:
                ASSERT_FAIL("ERROR: Resource dimension buffer type not supported for textures\n");
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

            case D3D12_RESOURCE_DIMENSION_UNKNOWN:
            default:
                ASSERT_FAIL("ERROR: Unknown resource dimension (%u)\n", static_cast<uint32_t>(d3d10ext->resourceDimension));
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            }

            resDim = static_cast<D3D12_RESOURCE_DIMENSION>(d3d10ext->resourceDimension);
        }
        else
        {
            format = GetDXGIFormat(header->ddspf);

            if (format == DXGI_FORMAT_UNKNOWN)
            {
                ASSERT_FAIL("ERROR: DDSTextureLoader does not support all legacy DDS formats. Consider using DirectXTex.\n");
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            }

            if (header->flags & DDS_HEADER_FLAGS_VOLUME)
            {
                resDim = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
            }
            else
            {
                if (header->caps2 & DDS_CUBEMAP)
                {
                    // We require all six faces to be defined
                    if ((header->caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES)
                    {
                        ASSERT_FAIL("ERROR: DirectX 12 does not support partial cubemaps\n");
                        return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
                    }

                    arraySize = 6;
                    isCubeMap = true;
                }

                depth = 1;
                resDim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

                // Note there's no way for a legacy Direct3D 9 DDS to express a '1D' texture
            }

            assert(BitsPerPixel(format) != 0);
        }

        // Bound sizes (for security purposes we don't trust DDS file metadata larger than the Direct3D hardware requirements)
        if (mipCount > D3D12_REQ_MIP_LEVELS)
        {
            ASSERT_FAIL("ERROR: Too many mipmap levels defined for DirectX 12 (%zu).\n", mipCount);
            return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        }

        switch (resDim)
        {
        case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
            if ((arraySize > D3D12_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION) ||
                (width > D3D12_REQ_TEXTURE1D_U_DIMENSION))
            {
                ASSERT_FAIL("ERROR: Resource dimensions too large for DirectX 12 (1D: array %u, size %u)\n", arraySize, width);
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            }
            break;

        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            if (isCubeMap)
            {
                // This is the right bound because we set arraySize to (NumCubes*6) above
                if ((arraySize > D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) ||
                    (width > D3D12_REQ_TEXTURECUBE_DIMENSION) ||
                    (height > D3D12_REQ_TEXTURECUBE_DIMENSION))
                {
                    ASSERT_FAIL("ERROR: Resource dimensions too large for DirectX 12 (2D cubemap: array %u, size %u by %u)\n", arraySize, width, height);
                    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
                }
            }
            else if ((arraySize > D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) ||
                (width > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION) ||
                (height > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION))
            {
                ASSERT_FAIL("ERROR: Resource dimensions too large for DirectX 12 (2D: array %u, size %u by %u)\n", arraySize, width, height);
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            }
            break;

        case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
            if ((arraySize > 1) ||
                (width > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) ||
                (height > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) ||
                (depth > D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION))
            {
                ASSERT_FAIL("ERROR: Resource dimensions too large for DirectX 12 (3D: array %u, size %u by %u by %u)\n", arraySize, width, height, depth);
                return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
            }
            break;

        case D3D12_RESOURCE_DIMENSION_BUFFER:
            ASSERT_FAIL("ERROR: Resource dimension buffer type not supported for textures\n");
            return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

        default:
            ASSERT_FAIL("ERROR: Unknown resource dimension (%u)\n", static_cast<uint32_t>(resDim));
            return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        }

        UINT numberOfPlanes = D3D12GetFormatPlaneCount(mD3DDevice.Get(), format);
        if (!numberOfPlanes)
            return E_INVALIDARG;

        if ((numberOfPlanes > 1) && IsDepthStencil(format))
        {
            // DirectX 12 uses planes for stencil, DirectX 11 does not
            return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        }

        if (outIsCubeMap != nullptr)
        {
            *outIsCubeMap = isCubeMap;
        }

        // Create the texture
        size_t numberOfResources = (resDim == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
            ? 1 : arraySize;
        numberOfResources *= mipCount;
        numberOfResources *= numberOfPlanes;

        if (numberOfResources > D3D12_REQ_SUBRESOURCES)
            return E_INVALIDARG;

        subresources.reserve(numberOfResources);

        size_t skipMip = 0;
        size_t twidth = 0;
        size_t theight = 0;
        size_t tdepth = 0;
        hr = FillInitData(width, height, depth, mipCount, arraySize,
            numberOfPlanes, format,
            maxsize, bitSize, bitData,
            twidth, theight, tdepth, skipMip, subresources);

        if (SUCCEEDED(hr))
        {
            size_t reservedMips = mipCount;
            if (loadFlags & (DDS_LOADER_MIP_AUTOGEN | DDS_LOADER_MIP_RESERVE))
            {
                reservedMips = std::min<size_t>(D3D12_REQ_MIP_LEVELS,
                    CountMips(width, height));
            }

            hr = CreateTextureResource(resDim, twidth, theight, tdepth, reservedMips - skipMip, arraySize,
                format, resFlags, loadFlags, texture);

            if (FAILED(hr) && !maxsize && (mipCount > 1))
            {
                subresources.clear();

                maxsize = static_cast<size_t>(
                    (resDim == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
                    ? D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION
                    : D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION);

                hr = FillInitData(width, height, depth, mipCount, arraySize,
                    numberOfPlanes, format,
                    maxsize, bitSize, bitData,
                    twidth, theight, tdepth, skipMip, subresources);
                if (SUCCEEDED(hr))
                {
                    hr = CreateTextureResource(resDim, twidth, theight, tdepth, mipCount - skipMip, arraySize,
                        format, resFlags, loadFlags, texture);
                }
            }
        }

        if (FAILED(hr))
        {
            subresources.clear();
        }

        return hr;
    }

    //--------------------------------------------------------------------------------------
    FORCEINLINE void AdjustPlaneResource(
        _In_ DXGI_FORMAT fmt,
        _In_ size_t height,
        _In_ size_t slicePlane,
        _Inout_ D3D12_SUBRESOURCE_DATA& res) noexcept
    {
        switch (fmt)
        {
        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
        case DXGI_FORMAT_D16_UNORM_S8_UINT:
        case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
#endif
            if (!slicePlane)
            {
                // Plane 0
                res.SlicePitch = res.RowPitch * static_cast<LONG>(height);
            }
            else
            {
                // Plane 1
                res.pData = static_cast<const uint8_t*>(res.pData) + uintptr_t(res.RowPitch) * height;
                res.SlicePitch = res.RowPitch * ((static_cast<LONG>(height) + 1) >> 1);
            }
            break;

        case DXGI_FORMAT_NV11:
            if (!slicePlane)
            {
                // Plane 0
                res.SlicePitch = res.RowPitch * static_cast<LONG>(height);
            }
            else
            {
                // Plane 1
                res.pData = static_cast<const uint8_t*>(res.pData) + uintptr_t(res.RowPitch) * height;
                res.RowPitch = (res.RowPitch >> 1);
                res.SlicePitch = res.RowPitch * static_cast<LONG>(height);
            }
            break;

        default:
            break;
        }
    }

    HRESULT FTextureHelper::FillInitData(
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
        std::vector<D3D12_SUBRESOURCE_DATA>& initData)
    {
        if (!bitData)
        {
            return E_POINTER;
        }

        skipMip = 0;
        twidth = 0;
        theight = 0;
        tdepth = 0;

        size_t NumBytes = 0;
        size_t RowBytes = 0;
        const uint8_t* pEndBits = bitData + bitSize;

        initData.clear();

        for (size_t p = 0; p < numberOfPlanes; ++p)
        {
            const uint8_t* pSrcBits = bitData;

            for (size_t j = 0; j < arraySize; j++)
            {
                size_t w = width;
                size_t h = height;
                size_t d = depth;
                for (size_t i = 0; i < mipCount; i++)
                {
                    HRESULT hr = GetSurfaceInfo(w, h, format, &NumBytes, &RowBytes, nullptr);
                    if (FAILED(hr))
                        return hr;

                    if (NumBytes > UINT32_MAX || RowBytes > UINT32_MAX)
                        return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

                    if ((mipCount <= 1) || !maxsize || (w <= maxsize && h <= maxsize && d <= maxsize))
                    {
                        if (!twidth)
                        {
                            twidth = w;
                            theight = h;
                            tdepth = d;
                        }

                        D3D12_SUBRESOURCE_DATA res =
                        {
                            pSrcBits,
                            static_cast<LONG_PTR>(RowBytes),
                            static_cast<LONG_PTR>(NumBytes)
                        };

                        AdjustPlaneResource(format, h, p, res);

                        initData.emplace_back(res);
                    }
                    else if (!j)
                    {
                        // Count number of skipped mipmaps (first item only)
                        ++skipMip;
                    }

                    if (pSrcBits + (NumBytes * d) > pEndBits)
                    {
                        return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
                    }

                    pSrcBits += NumBytes * d;

                    w = w >> 1;
                    h = h >> 1;
                    d = d >> 1;
                    if (w == 0)
                    {
                        w = 1;
                    }
                    if (h == 0)
                    {
                        h = 1;
                    }
                    if (d == 0)
                    {
                        d = 1;
                    }
                }
            }
        }

        return initData.empty() ? E_FAIL : S_OK;
    }

    HRESULT FTextureHelper::CreateTextureResource(D3D12_RESOURCE_DIMENSION resDim, size_t width, size_t height, size_t depth, size_t mipCount, size_t arraySize, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resFlags, EDDS_LOADER_FLAGS loadFlags, Microsoft::WRL::ComPtr<ID3D12Resource>& texture) noexcept
    {
        if (!mD3DDevice)
            return E_POINTER;

        HRESULT hr = E_FAIL;

        if (loadFlags & DDS_LOADER_FORCE_SRGB)
        {
            format = MakeSRGB(format);
        }

        D3D12_RESOURCE_DESC desc = {};
        desc.Width = static_cast<UINT>(width);
        desc.Height = static_cast<UINT>(height);
        desc.MipLevels = static_cast<UINT16>(mipCount);
        desc.DepthOrArraySize = (resDim == D3D12_RESOURCE_DIMENSION_TEXTURE3D) ? static_cast<UINT16>(depth) : static_cast<UINT16>(arraySize);
        desc.Format = format;
        desc.Flags = resFlags;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Dimension = resDim;

        CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        hr = mD3DDevice->CreateCommittedResource(
            &defaultHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&texture));
        if (SUCCEEDED(hr))
        {
            ASSERT(texture != nullptr);
            _Analysis_assume_(texture != nullptr && *texture != nullptr);

            SetDebugObjectName(texture.Get(), L"DDSTextureLoader");
        }

        return hr;
    }

    UINT64 FTextureHelper::UpdateSubresources(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& pCmdList, Microsoft::WRL::ComPtr<ID3D12Resource>& pDestinationResource, Microsoft::WRL::ComPtr<ID3D12Resource>& pIntermediate, UINT FirstSubresource, UINT NumSubresources, UINT64 RequiredSize, const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts, const UINT* pNumRows, const UINT64* pRowSizesInBytes, const D3D12_SUBRESOURCE_DATA* pSrcData) noexcept
    {
        // Minor validation
        auto IntermediateDesc = pIntermediate->GetDesc();
        auto DestinationDesc = pDestinationResource->GetDesc();
        if (IntermediateDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
            IntermediateDesc.Width < RequiredSize + pLayouts[0].Offset ||
            RequiredSize > SIZE_T(-1) ||
            (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
                (FirstSubresource != 0 || NumSubresources != 1)))
        {
            return 0;
        }

        BYTE* pData;
        HRESULT hr = pIntermediate->Map(0, nullptr, reinterpret_cast<void**>(&pData));
        if (FAILED(hr))
        {
            return 0;
        }

        for (UINT i = 0; i < NumSubresources; ++i)
        {
            if (pRowSizesInBytes[i] > SIZE_T(-1)) return 0;
            D3D12_MEMCPY_DEST DestData = { pData + pLayouts[i].Offset, pLayouts[i].Footprint.RowPitch, SIZE_T(pLayouts[i].Footprint.RowPitch) * SIZE_T(pNumRows[i]) };
            MemcpySubresource(&DestData, &pSrcData[i], static_cast<SIZE_T>(pRowSizesInBytes[i]), pNumRows[i], pLayouts[i].Footprint.Depth);
        }
        pIntermediate->Unmap(0, nullptr);

        if (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
        {
            pCmdList->CopyBufferRegion(
                pDestinationResource.Get(), 0, pIntermediate.Get(), pLayouts[0].Offset, pLayouts[0].Footprint.Width);
        }
        else
        {
            for (UINT i = 0; i < NumSubresources; ++i)
            {
                CD3DX12_TEXTURE_COPY_LOCATION Dst(pDestinationResource.Get(), i + FirstSubresource);
                CD3DX12_TEXTURE_COPY_LOCATION Src(pIntermediate.Get(), pLayouts[i]);
                pCmdList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
            }
        }
        return RequiredSize;
    }

    HRESULT FTextureHelper::CaptureTexture(
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQ, 
        Microsoft::WRL::ComPtr<ID3D12Resource>& pSource, 
        UINT64 srcPitch, 
        const D3D12_RESOURCE_DESC& desc, 
        Microsoft::WRL::ComPtr<ID3D12Resource>& pStaging, 
        D3D12_RESOURCE_STATES beforeState, 
        D3D12_RESOURCE_STATES afterState) noexcept
    {
        if (!pCommandQ || !pSource)
            return E_INVALIDARG;

        if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D)
        {
            ASSERT("ERROR: ScreenGrab does not support 1D or volume textures. Consider using DirectXTex instead.\n");
            return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        }

        if (desc.DepthOrArraySize > 1 || desc.MipLevels > 1)
        {
            ASSERT("WARNING: ScreenGrab does not support 2D arrays, cubemaps, or mipmaps; only the first surface is written. Consider using DirectXTex instead.\n");
        }

        if (srcPitch > UINT32_MAX)
            return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);

        UINT numberOfPlanes = D3D12GetFormatPlaneCount(mD3DDevice.Get(), desc.Format);
        if (numberOfPlanes != 1)
            return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

        D3D12_HEAP_PROPERTIES sourceHeapProperties;
        D3D12_HEAP_FLAGS sourceHeapFlags;
        HRESULT hr = pSource->GetHeapProperties(&sourceHeapProperties, &sourceHeapFlags);
        if (FAILED(hr))
            return hr;

        if (sourceHeapProperties.Type == D3D12_HEAP_TYPE_READBACK)
        {
            // Handle case where the source is already a staging texture we can use directly
            pStaging = pSource;
            return S_OK;
        }

        // Create a command allocator
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAlloc;
        hr = mD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAlloc.GetAddressOf()));
        if (FAILED(hr))
            return hr;

        SetDebugObjectName(commandAlloc.Get(), L"ScreenGrab");

        // Spin up a new command list
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
        hr = mD3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAlloc.Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf()));
        if (FAILED(hr))
            return hr;

        SetDebugObjectName(commandList.Get(), L"ScreenGrab");

        // Create a fence
        Microsoft::WRL::ComPtr<ID3D12Fence> fence;
        hr = mD3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
        if (FAILED(hr))
            return hr;

        SetDebugObjectName(fence.Get(), L"ScreenGrab");

        assert((srcPitch & 0xFF) == 0);

        CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_HEAP_PROPERTIES readBackHeapProperties(D3D12_HEAP_TYPE_READBACK);

        // Readback resources must be buffers
        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        bufferDesc.Height = 1;
        bufferDesc.Width = srcPitch * desc.Height;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufferDesc.MipLevels = 1;
        bufferDesc.SampleDesc.Count = 1;

        Microsoft::WRL::ComPtr<ID3D12Resource> copySource(pSource);
        if (desc.SampleDesc.Count > 1)
        {
            // MSAA content must be resolved before being copied to a staging texture
            auto descCopy = desc;
            descCopy.SampleDesc.Count = 1;
            descCopy.SampleDesc.Quality = 0;

            Microsoft::WRL::ComPtr<ID3D12Resource> pTemp;
            hr = mD3DDevice->CreateCommittedResource(
                &defaultHeapProperties,
                D3D12_HEAP_FLAG_NONE,
                &descCopy,
                D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&pTemp));
            if (FAILED(hr))
                return hr;

            assert(pTemp);

            SetDebugObjectName(pTemp.Get(), L"ScreenGrab temporary");

            DXGI_FORMAT fmt = EnsureNotTypeless(desc.Format);

            D3D12_FEATURE_DATA_FORMAT_SUPPORT formatInfo = { fmt, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE };
            hr = mD3DDevice->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatInfo, sizeof(formatInfo));
            if (FAILED(hr))
                return hr;

            if (!(formatInfo.Support1 & D3D12_FORMAT_SUPPORT1_TEXTURE2D))
                return E_FAIL;

            for (UINT item = 0; item < desc.DepthOrArraySize; ++item)
            {
                for (UINT level = 0; level < desc.MipLevels; ++level)
                {
                    UINT index = D3D12CalcSubresource(level, item, 0, desc.MipLevels, desc.DepthOrArraySize);
                    commandList->ResolveSubresource(pTemp.Get(), index, pSource.Get(), index, fmt);
                }
            }

            copySource = pTemp;
        }

        // Create a staging texture
        hr = mD3DDevice->CreateCommittedResource(
            &readBackHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(pStaging.ReleaseAndGetAddressOf()));
        if (FAILED(hr))
            return hr;

        SetDebugObjectName(pStaging.Get(), L"ScreenGrab staging");

        assert(pStaging);

        // Transition the resource if necessary
        //TransitionResource(commandList.Get(), pSource, beforeState, D3D12_RESOURCE_STATE_COPY_SOURCE);

        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pSource.Get(), beforeState, D3D12_RESOURCE_STATE_COPY_SOURCE));

        // Get the copy target location
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT bufferFootprint = {};
        bufferFootprint.Footprint.Width = static_cast<UINT>(desc.Width);
        bufferFootprint.Footprint.Height = desc.Height;
        bufferFootprint.Footprint.Depth = 1;
        bufferFootprint.Footprint.RowPitch = static_cast<UINT>(srcPitch);
        bufferFootprint.Footprint.Format = desc.Format;

        CD3DX12_TEXTURE_COPY_LOCATION copyDest(pStaging.Get(), bufferFootprint);
        CD3DX12_TEXTURE_COPY_LOCATION copySrc(copySource.Get(), 0);

        // Copy the texture
        commandList->CopyTextureRegion(&copyDest, 0, 0, 0, &copySrc, nullptr);

        // Transition the resource to the next state
        //TransitionResource(commandList.Get(), pSource, D3D12_RESOURCE_STATE_COPY_SOURCE, afterState);
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pSource.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, afterState));

        hr = commandList->Close();
        if (FAILED(hr))
            return hr;

        // Execute the command list
        pCommandQ->ExecuteCommandLists(1, CommandListCast(commandList.GetAddressOf()));

        // Signal the fence
        hr = pCommandQ->Signal(fence.Get(), 1);
        if (FAILED(hr))
            return hr;

        // Block until the copy is complete
        while (fence->GetCompletedValue() < 1)
            SwitchToThread();

        return S_OK;
    }

    Microsoft::WRL::ComPtr<ID3D12RootSignature> FTextureHelper::CreateGenMipsRootSignature()
    {
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

        CD3DX12_STATIC_SAMPLER_DESC sampler(
            0, // register
            D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

        CD3DX12_DESCRIPTOR_RANGE sourceDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        CD3DX12_DESCRIPTOR_RANGE targetDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

        CD3DX12_ROOT_PARAMETER rootParameters[FTextureHelper::RootParameterCount] = {};
        rootParameters[FTextureHelper::Constants].InitAsConstants(FTextureHelper::Num32BitConstants, 0);
        rootParameters[FTextureHelper::SourceTexture].InitAsDescriptorTable(1, &sourceDescriptorRange);
        rootParameters[FTextureHelper::TargetTexture].InitAsDescriptorTable(1, &targetDescriptorRange);

        CD3DX12_ROOT_SIGNATURE_DESC rsigDesc;
        rsigDesc.Init(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        DX_CALL(CreateRootSignature(mD3DDevice, &rsigDesc, rootSignature));

        return rootSignature;
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> FTextureHelper::CreateGenMipsPipelineState(Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature, const void* bytecode, size_t bytecodeSize)
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
        desc.CS.BytecodeLength = bytecodeSize;
        desc.CS.pShaderBytecode = bytecode;
        desc.pRootSignature = rootSignature.Get();

        Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
        DX_CALL(mD3DDevice->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pso)));

        return pso;
    }

    void FTextureHelper::GenerateMips_UnorderedAccessPath(Microsoft::WRL::ComPtr<ID3D12Resource>& resource)
    {
        const auto desc = resource->GetDesc();
        ASSERT(!FormatIsBGR(desc.Format) && !FormatIsSRGB(desc.Format));

        CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        D3D12_COMMAND_LIST_TYPE commandListType = mCommandList->GetType();

        ASSERT(commandListType != D3D12_COMMAND_LIST_TYPE_COPY);
        const D3D12_RESOURCE_STATES originalState = (commandListType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
            ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;  

        // Create a staging resource if we have to
        Microsoft::WRL::ComPtr<ID3D12Resource> staging;
        if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) == 0)
        {
            D3D12_RESOURCE_DESC stagingDesc = desc;
            stagingDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            stagingDesc.Format = ConvertSRVtoResourceFormat(desc.Format);

            DX_CALL(mD3DDevice->CreateCommittedResource(
                &defaultHeapProperties,
                D3D12_HEAP_FLAG_NONE,
                &stagingDesc,
                D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(staging.GetAddressOf())));

            SetDebugObjectName(staging.Get(), L"Staging Resource");

            // Copy the top mip of resource to staging
            mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), originalState, D3D12_RESOURCE_STATE_COPY_SOURCE));

            CD3DX12_TEXTURE_COPY_LOCATION src(resource.Get(), 0);
            CD3DX12_TEXTURE_COPY_LOCATION dst(staging.Get(), 0);
            mCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

            mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(staging.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
        }
        else
        {
            // Resource is already a UAV so we can do this in-place
            staging = resource;

            mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(staging.Get(), originalState, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
        }

        // Create a descriptor heap that holds our resource descriptors
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
        descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        descriptorHeapDesc.NumDescriptors = desc.MipLevels;
        mD3DDevice->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));

        auto descriptorSize = static_cast<int>(mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

        // Create SRV for all mips
        CD3DX12_CPU_DESCRIPTOR_HANDLE handleIt(descriptorHeap->GetCPUDescriptorHandleForHeapStart());
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;

        mD3DDevice->CreateShaderResourceView(staging.Get(), &srvDesc, handleIt);

        // Create the UAVs for the tail
        for (uint16_t mip = 1; mip < desc.MipLevels; ++mip)
        {
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = desc.Format;
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.MipSlice = mip;

            handleIt.Offset(descriptorSize);
            mD3DDevice->CreateUnorderedAccessView(staging.Get(), nullptr, &uavDesc, handleIt);
        }

        // Set up UAV barrier (used in loop)
        D3D12_RESOURCE_BARRIER barrierUAV = {};
        barrierUAV.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrierUAV.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrierUAV.UAV.pResource = staging.Get();

        // Barrier for transitioning the subresources to UAVs
        D3D12_RESOURCE_BARRIER srv2uavDesc = {};
        srv2uavDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        srv2uavDesc.Transition.pResource = staging.Get();
        srv2uavDesc.Transition.Subresource = 0;
        srv2uavDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        srv2uavDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

        // Barrier for transitioning the subresources to SRVs
        D3D12_RESOURCE_BARRIER uav2srvDesc = {};
        uav2srvDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        uav2srvDesc.Transition.pResource = staging.Get();
        uav2srvDesc.Transition.Subresource = 0;
        uav2srvDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        uav2srvDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

        // Set up state
        mCommandList->SetComputeRootSignature(mRootSignature.Get());
        mCommandList->SetPipelineState(mPipelineState.Get());
        mCommandList->SetDescriptorHeaps(1, descriptorHeap.GetAddressOf());
        mCommandList->SetComputeRootDescriptorTable(FTextureHelper::SourceTexture, descriptorHeap->GetGPUDescriptorHandleForHeapStart());

        // Get the descriptor handle -- uavH will increment over each loop
        CD3DX12_GPU_DESCRIPTOR_HANDLE uavH(
            descriptorHeap->GetGPUDescriptorHandleForHeapStart(),
            descriptorSize); // offset by 1 descriptor

        // Process each mip
        auto mipWidth = static_cast<uint32_t>(desc.Width);
        uint32_t mipHeight = desc.Height;
        for (uint32_t mip = 1; mip < desc.MipLevels; ++mip)
        {
            mipWidth = FMath::Max<uint32_t>(1, mipWidth >> 1);
            mipHeight = FMath::Max<uint32_t>(1, mipHeight >> 1);

            // Transition the mip to a UAV
            srv2uavDesc.Transition.Subresource = mip;
            mCommandList->ResourceBarrier(1, &srv2uavDesc);

            // Bind the mip subresources
            mCommandList->SetComputeRootDescriptorTable(FTextureHelper::TargetTexture, uavH);

            // Set constants
            FTextureHelper::ConstantData constants;
            constants.SrcMipIndex = mip - 1;
            constants.InvOutTexelSize = FVector2f(1 / float(mipWidth), 1 / float(mipHeight));
            mCommandList->SetComputeRoot32BitConstants(
                FTextureHelper::Constants,
                FTextureHelper::Num32BitConstants,
                &constants,
                0);

            // Process this mip
            // Every thread process one pixel ( ThreadGroupSize == 8 )
            mCommandList->Dispatch(
                (mipWidth + FTextureHelper::ThreadGroupSize - 1) / FTextureHelper::ThreadGroupSize,
                (mipHeight + FTextureHelper::ThreadGroupSize - 1) / FTextureHelper::ThreadGroupSize,
                1);

            //Transition uav resource to flag none
            mCommandList->ResourceBarrier(1, &barrierUAV);

            // Transition the mip to an SRV
            uav2srvDesc.Transition.Subresource = mip;
            mCommandList->ResourceBarrier(1, &uav2srvDesc);

            // Offset the descriptor heap handles
            uavH.Offset(descriptorSize);
        }

        // If the staging resource is NOT the same as the resource, we need to copy everything back
        if (staging.Get() != resource.Get())
        {
            // Transition the resources ready for copy
            D3D12_RESOURCE_BARRIER barrier[2] = {};
            barrier[0].Type = barrier[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier[0].Transition.Subresource = barrier[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier[0].Transition.pResource = staging.Get();
            barrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            barrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

            barrier[1].Transition.pResource = resource.Get();
            barrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
            barrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

            mCommandList->ResourceBarrier(2, barrier);

            // Copy the entire resource back
            mCommandList->CopyResource(resource.Get(), staging.Get());

            // Transition the target resource back to pixel shader resource
            mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, originalState));

            mTrackedObjects.push_back(staging);
        }
        else
        {
            mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(staging.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, originalState));
        }
        
        mTrackedObjects.push_back(resource);
        mTrackedObjects.push_back(descriptorHeap);
    }

    void FTextureHelper::GenerateMips_TexturePath(Microsoft::WRL::ComPtr<ID3D12Resource>& resource)
    {
        const auto resourceDesc = resource->GetDesc();
        ASSERT(!FormatIsBGR(resourceDesc.Format) || FormatIsSRGB(resourceDesc.Format));

        D3D12_COMMAND_LIST_TYPE commandListType = mCommandList->GetType();

        auto copyDesc = resourceDesc;
        copyDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        copyDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

        // Create a resource with the same description, but without SRGB, and with UAV flags
        Microsoft::WRL::ComPtr<ID3D12Resource> resourceCopy;
        DX_CALL(mD3DDevice->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &copyDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(resourceCopy.GetAddressOf())));

        SetDebugObjectName(resourceCopy.Get(), L"GenerateMips Resource Copy");

        ASSERT(commandListType != D3D12_COMMAND_LIST_TYPE_COPY);
        const D3D12_RESOURCE_STATES originalState = commandListType == D3D12_COMMAND_LIST_TYPE_COMPUTE
            ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

        // Copy the top mip of resource data
        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), originalState, D3D12_RESOURCE_STATE_COPY_SOURCE));

        CD3DX12_TEXTURE_COPY_LOCATION src(resource.Get(), 0);
        CD3DX12_TEXTURE_COPY_LOCATION dst(resourceCopy.Get(), 0);
        mCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resourceCopy.Get(), D3D12_RESOURCE_STATE_COPY_DEST, originalState));

        // Generate the mips
        GenerateMips_UnorderedAccessPath(resourceCopy);

        // Direct copy back
        D3D12_RESOURCE_BARRIER barrier[2] = {};
        barrier[0].Type = barrier[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier[0].Transition.Subresource = barrier[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier[0].Transition.pResource = resourceCopy.Get();
        barrier[0].Transition.StateBefore = originalState;
        barrier[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

        barrier[1].Transition.pResource = resource.Get();
        barrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
        barrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

        mCommandList->ResourceBarrier(2, barrier);

        // Copy the entire resource back
        mCommandList->CopyResource(resource.Get(), resourceCopy.Get());

        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, originalState));

        // Track these object lifetimes on the GPU
        mTrackedObjects.push_back(resourceCopy);
        mTrackedObjects.push_back(resource);
    }

    void FTextureHelper::GenerateMips_TexturePathBGR(Microsoft::WRL::ComPtr<ID3D12Resource>& resource)
    {
        D3D12_COMMAND_LIST_TYPE commandListType = mCommandList->GetType();

        const auto resourceDesc = resource->GetDesc();
        ASSERT(FormatIsBGR(resourceDesc.Format));

        // Create a resource with the same description with RGB and with UAV flags
        auto copyDesc = resourceDesc;
        copyDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        copyDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
#if !defined(_GAMING_XBOX) && !(defined(_XBOX_ONE) && defined(_TITLE))
        copyDesc.Layout = D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;
#endif

        D3D12_HEAP_DESC heapDesc = {};
        auto allocInfo = mD3DDevice->GetResourceAllocationInfo(0, 1, &copyDesc);
        heapDesc.SizeInBytes = allocInfo.SizeInBytes;
        heapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
        heapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

        Microsoft::WRL::ComPtr<ID3D12Heap> heap;
        DX_CALL(mD3DDevice->CreateHeap(&heapDesc, IID_PPV_ARGS(heap.GetAddressOf())));

        SetDebugObjectName(heap.Get(), L"GenerateMips Heap");

        Microsoft::WRL::ComPtr<ID3D12Resource> resourceCopy;
        DX_CALL(mD3DDevice->CreatePlacedResource(
            heap.Get(),
            0,
            &copyDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(resourceCopy.GetAddressOf())));

        SetDebugObjectName(resourceCopy.Get(), L"GenerateMips Resource Copy");

        // Create a BGRA alias
        auto aliasDesc = resourceDesc;
        aliasDesc.Format = (resourceDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM || resourceDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) ? DXGI_FORMAT_B8G8R8X8_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM;
        aliasDesc.Layout = copyDesc.Layout;

        Microsoft::WRL::ComPtr<ID3D12Resource> aliasCopy;
        DX_CALL(mD3DDevice->CreatePlacedResource(
            heap.Get(),
            0,
            &aliasDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(aliasCopy.GetAddressOf())));

        SetDebugObjectName(aliasCopy.Get(), L"GenerateMips BGR Alias Copy");

        ASSERT(commandListType != D3D12_COMMAND_LIST_TYPE_COPY);
        const D3D12_RESOURCE_STATES originalState = (commandListType == D3D12_COMMAND_LIST_TYPE_COMPUTE)
            ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

        // Copy the top mip of the resource data BGR to RGB
        D3D12_RESOURCE_BARRIER aliasBarrier[3] = {};
        aliasBarrier[0].Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
        aliasBarrier[0].Aliasing.pResourceAfter = aliasCopy.Get();

        aliasBarrier[1].Type = aliasBarrier[2].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        aliasBarrier[1].Transition.Subresource = aliasBarrier[2].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        aliasBarrier[1].Transition.pResource = resource.Get();
        aliasBarrier[1].Transition.StateBefore = originalState;
        aliasBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

        mCommandList->ResourceBarrier(2, aliasBarrier);

        CD3DX12_TEXTURE_COPY_LOCATION src(resource.Get(), 0);
        CD3DX12_TEXTURE_COPY_LOCATION dst(aliasCopy.Get(), 0);
        mCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

        // Generate the mips
        aliasBarrier[0].Aliasing.pResourceBefore = aliasCopy.Get();
        aliasBarrier[0].Aliasing.pResourceAfter = resourceCopy.Get();

        aliasBarrier[1].Transition.pResource = resourceCopy.Get();
        aliasBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        aliasBarrier[1].Transition.StateAfter = originalState;

        mCommandList->ResourceBarrier(2, aliasBarrier);
        GenerateMips_UnorderedAccessPath(resourceCopy);

        // Direct copy back RGB to BGR
        aliasBarrier[0].Aliasing.pResourceBefore = resourceCopy.Get();
        aliasBarrier[0].Aliasing.pResourceAfter = aliasCopy.Get();

        aliasBarrier[1].Transition.pResource = aliasCopy.Get();
        aliasBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        aliasBarrier[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

        aliasBarrier[2].Transition.pResource = resource.Get();
        aliasBarrier[2].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
        aliasBarrier[2].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

        mCommandList->ResourceBarrier(3, aliasBarrier);

        // Copy the entire resource back
        mCommandList->CopyResource(resource.Get(), aliasCopy.Get());
        mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, originalState));

        // Track these object lifetimes on the GPU
        mTrackedObjects.push_back(heap);
        mTrackedObjects.push_back(resourceCopy);
        mTrackedObjects.push_back(aliasCopy);
        mTrackedObjects.push_back(resource);
    }
}
*/