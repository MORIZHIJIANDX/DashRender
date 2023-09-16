#include "PCH.h"
#include "Texture.h"
#include "Graphics/GraphicsCore.h"

namespace Dash
{
    FTexture::FTexture(const std::string& texturePath)
        : mTexturePath(texturePath)
        , mTextureData(FTextureLoaderManager::Get().LoadTexture(texturePath))
    {
        FGraphicsCore::Device->CreateTextureBufferFromMemory(mTextureData.SourceTexturePath, mTextureData.TextureDescription
            , { mTextureData.DecodedData.data() });
    }

    FTexture::~FTexture()
    {
        FTextureLoaderManager::Get().UnloadTexture(mTexturePath);
    }

    FTextureRef FTexture::MakeTexture(const std::string& texturePath)
    {
        if (!mTextureResourceMap.contains(texturePath) && !mTextureResourceMap[texturePath].lock())
        {
            FTextureRef newTexture = std::make_shared<FTexture>(texturePath);
            mTextureResourceMap.emplace(texturePath, newTexture);
        }

        return mTextureResourceMap[texturePath].lock();
    }
}