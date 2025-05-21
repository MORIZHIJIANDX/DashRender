#pragma once

#include "DDSTextureLoader.h"
#include "HDRTextureLoader.h"
#include "TGATextureLoader.h"
#include "WICTextureLoader.h"
#include "TextureLoaderHelper.h"

namespace Dash
{	
	class FTextureLoaderManager
	{
	public:
		static FTextureLoaderManager& Get();

		void Init();
		void Shutdown();

		const FImportedTextureData& LoadTexture(const std::string& texturePath, bool forceSrgb = false);

		bool UnloadTexture(const std::string& texturePath);

	private:
		void CreateDefaultTextures();

		void ConstructPureColorTexture(const std::string_view& textureName, const FColor& color, int32 width = 32, int32 height = 32);

	private:
		std::map<std::string, FImportedTextureData> mImportTextures;
	};
}