#pragma once

#include "Graphics/RenderDevice.h"
#include "TextureLoader/TextureLoaderManager.h"
#include "Graphics/TextureBuffer.h"

namespace Dash
{
	class FTexture
	{
	public:
		FTexture(const std::string& texturePath);
		~FTexture();

		const FImportedTextureData& GetTextureImportedData() const { return mTextureData; }
		FTextureBufferRef GetTextureBufferRef() const { return mTextureBufferRef; }

		FTextureBufferRef& GetTextureBuffer() { return mTextureBufferRef; }
		const FTextureBufferRef& GetTextureBuffer() const { return mTextureBufferRef; }

	private:
		std::string mTexturePath;
		const FImportedTextureData& mTextureData;
		FTextureBufferRef mTextureBufferRef;
	};
}