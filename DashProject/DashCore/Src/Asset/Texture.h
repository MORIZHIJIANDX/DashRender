#pragma once

#include "Graphics/RenderDevice.h"
#include "TextureLoader/TextureLoaderManager.h"

namespace Dash
{
	class FTexture
	{
	public:
		FTexture(const std::string& texturePath);
		~FTexture();
		
		const FImportedTextureData& GetTextureImportedData() const { return mTextureData; }
		FTextureBufferRef GetTextureBufferRef() const { return mTextureBufferRef; }

	private:
		std::string mTexturePath;
		const FImportedTextureData& mTextureData;
		FTextureBufferRef mTextureBufferRef;
	};
}