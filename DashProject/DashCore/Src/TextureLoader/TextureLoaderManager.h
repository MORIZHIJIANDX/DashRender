#pragma once

#include "DDSTextureLoader.h"
#include "HDRTextureLoader.h"
#include "TGATextureLoader.h"
#include "WICTextureLoader.h"

namespace Dash
{	
	struct FImportedTextureData
	{
		friend class FTextureLoaderManager;

		std::string SourceTexturePath;
		FTextureBufferDescription TextureDescription;
		D3D12_SUBRESOURCE_DATA SubResource;
		std::vector<uint8_t> DecodedData;

	private:

		int32_t AddRef()
		{
			return ++RefCount;
		}

		int32_t Release()
		{
			return --RefCount;
		}

	private:

		int32_t RefCount = 0;
	};

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

	private:
		std::map<std::string, FImportedTextureData> mImportTextures;
	};
}