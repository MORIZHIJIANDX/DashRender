#include "PCH.h"
#include "Texture.h"
#include "Graphics/GraphicsCore.h"

namespace Dash
{
    std::map<std::string, std::weak_ptr<FTexture>> FTexture::mTextureResourceMap;

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
        FTextureRef texture = nullptr;

        if (!mTextureResourceMap.contains(texturePath) || !mTextureResourceMap[texturePath].lock())
        {
            texture = std::make_shared<FTexture>(texturePath);
            mTextureResourceMap[texturePath] = texture;
        }
        else
        {
            texture = mTextureResourceMap[texturePath].lock();
        }

        return texture;
    }
}