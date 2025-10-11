#include "PCH.h"
#include "TextureLoaderManager.h"
#include "Utility/FileUtility.h"
#include "Utility/StringUtility.h"

namespace Dash
{
	FTextureLoaderManager& FTextureLoaderManager::Get()
	{
		static FTextureLoaderManager manager;
		return manager;
	}

	void FTextureLoaderManager::Init()
	{
		CreateDefaultTextures();
	}

	void FTextureLoaderManager::Shutdown()
	{
		mImportTextures.clear();
	}

	const FImportedTextureData& FTextureLoaderManager::LoadTexture(const std::string& texturePath, bool forceSrgb)
	{
		if (!mImportTextures.contains(texturePath))
		{
			bool loadSucceed = false;

			if (FFileUtility::IsPathExistent(texturePath))
			{
				std::string fileExtension = FStringUtility::ToLower(FFileUtility::GetFileExtension(texturePath));

				FImportedTextureData importedTextureData;
				importedTextureData.SourceTexturePath = texturePath;

				if (fileExtension == "png")
				{
					loadSucceed = LoadWICTextureFromFile(texturePath, forceSrgb ? EWIC_LOAD_FLAGS::WIC_FLAGS_FORCE_SRGB : EWIC_LOAD_FLAGS::WIC_FLAGS_NONE, importedTextureData.TextureDescription, importedTextureData.SubResource, importedTextureData.DecodedData);
				}
				else if (fileExtension == "tga")
				{
					loadSucceed = LoadTGATextureFromFile(texturePath, forceSrgb ? ETGA_LOAD_FLAGS::TGA_FLAGS_FORCE_SRGB : ETGA_LOAD_FLAGS::TGA_FLAGS_NONE, importedTextureData.TextureDescription, importedTextureData.SubResource, importedTextureData.DecodedData);
				}
				else if (fileExtension == "hdr")
				{
					loadSucceed = LoadHDRTextureFromFile(texturePath, importedTextureData.TextureDescription, importedTextureData.SubResource, importedTextureData.DecodedData);
				}
				else if (fileExtension == "dds")
				{
					loadSucceed = LoadDDSTextureFromFile(texturePath, EDDS_LOAD_FLAGS::DDS_FLAGS_NONE, importedTextureData.TextureDescription, importedTextureData.SubResource, importedTextureData.DecodedData);
				}

				if (loadSucceed)
				{
					mImportTextures.emplace(texturePath, importedTextureData);
				}
			}
			
			if (!loadSucceed)
			{
				DASH_LOG(LogTemp, Error, "Failed to load texture : {}", texturePath);
				return mImportTextures["ErrorTexture"];
			}
		}

		mImportTextures[texturePath].AddRef();
		return mImportTextures[texturePath];
	}

	bool FTextureLoaderManager::UnloadTexture(const std::string& texturePath)
	{
		if (mImportTextures.contains(texturePath))
		{
			int32 RefCount = mImportTextures[texturePath].Release();
			if (RefCount <= 0)
			{
				mImportTextures.erase(texturePath);		
			}
			return true;
		}

		return false;
	}

	void FTextureLoaderManager::CreateDefaultTextures()
	{
		ConstructPureColorTexture("ErrorTexture", FColor::Magenta);
		ConstructPureColorTexture("Black", FColor::Black);
	}

	void FTextureLoaderManager::ConstructPureColorTexture(const std::string_view& textureName, const FColor& color, int32 width, int32 height)
	{
		FImportedTextureData importedTextureData;
		importedTextureData.SourceTexturePath = textureName;
		importedTextureData.TextureDescription = FTextureBufferDescription::Create2D(EResourceFormat::RGBA8_Unsigned_Norm, width, height);
		importedTextureData.DecodedData.resize(importedTextureData.TextureDescription.ResourceSizeInBytes());

		FColor* DataPtr = (FColor*)importedTextureData.DecodedData.data();

		for (uint32 y = 0; y < importedTextureData.TextureDescription.Magnitude.Height; y++)
		{
			for (uint32 x = 0; x < importedTextureData.TextureDescription.Magnitude.Width; x++)
			{
				*DataPtr = color;
			}
		}

		void* dataPtr = importedTextureData.DecodedData.data();
		size_t rowPitch = 0;
		size_t slicePitch = 0;

		importedTextureData.TextureDescription.GetPitch(rowPitch, slicePitch);

		importedTextureData.SubResource.emplace_back(dataPtr, rowPitch, slicePitch);

		importedTextureData.AddRef();

		mImportTextures.emplace(importedTextureData.SourceTexturePath, importedTextureData);
	}
}