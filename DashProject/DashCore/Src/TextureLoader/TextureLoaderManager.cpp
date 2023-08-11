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
				LOG_ERROR << "Failed to load texture : " << texturePath;
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
			int32_t RefCount = mImportTextures[texturePath].Release();
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
		FImportedTextureData importedTextureData;
		importedTextureData.SourceTexturePath = "ErrorTexture";
		importedTextureData.TextureDescription = FTextureBufferDescription::Create2D(EResourceFormat::RGBA8_Unsigned_Norm, 32, 32);
		importedTextureData.DecodedData.resize(importedTextureData.TextureDescription.ResourceSizeInBytes());
		
		FColor* DataPtr = (FColor*)importedTextureData.DecodedData.data();

		for (uint32_t y = 0; y < importedTextureData.TextureDescription.Magnitude.Height; y++)
		{
			for (uint32_t x = 0; x < importedTextureData.TextureDescription.Magnitude.Width; x++)
			{
				*DataPtr = FColor::Magenta;
			}
		}

		importedTextureData.SubResource.pData = importedTextureData.DecodedData.data();
		importedTextureData.SubResource.RowPitch = importedTextureData.TextureDescription.Magnitude.Width * BytesPerPixel(EResourceFormat::RGBA8_Unsigned_Norm);
		importedTextureData.SubResource.SlicePitch = importedTextureData.SubResource.RowPitch * importedTextureData.TextureDescription.Magnitude.Height;
		importedTextureData.AddRef();
		
		mImportTextures.emplace(importedTextureData.SourceTexturePath, importedTextureData);
	}
}