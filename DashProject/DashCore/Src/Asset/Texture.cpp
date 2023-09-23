#include "PCH.h"
#include "Texture.h"
#include "Graphics/GraphicsCore.h"

namespace Dash
{
    FTexture::FTexture(const std::string& texturePath)
        : mTexturePath(texturePath)
        , mTextureData(FTextureLoaderManager::Get().LoadTexture(texturePath))
    {
        mTextureBufferRef = FGraphicsCore::Device->CreateTextureBufferFromMemory(mTextureData.SourceTexturePath, mTextureData.TextureDescription
            , { mTextureData.DecodedData.data() });
    }

    FTexture::~FTexture()
    {
        FTextureLoaderManager::Get().UnloadTexture(mTexturePath);
    }
}