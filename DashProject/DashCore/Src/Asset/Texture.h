#pragma once

#include "Graphics/RenderDevice.h"
#include "TextureLoader/TextureLoaderManager.h"

namespace Dash
{
	class FTexture;
	using FTextureRef = std::shared_ptr<FTexture>;

	class FTexture
	{
	public:
		FTexture(const std::string& texturePath);
		~FTexture();

		static FTextureRef MakeTexture(const std::string& texturePath);

		const FImportedTextureData& GetTextureImportedData() const { return mTextureData; }
		FTextureBufferRef GetTextureBufferRef() const { return mTextureBufferRef; }

		FTextureBufferRef& GetTextureBuffer() { return mTextureBufferRef; }
		const FTextureBufferRef& GetTextureBuffer() const { return mTextureBufferRef; }

	private:
		std::string mTexturePath;
		const FImportedTextureData& mTextureData;
		FTextureBufferRef mTextureBufferRef;

		static std::map<std::string, std::weak_ptr<FTexture>> mTextureResourceMap;
	};
}