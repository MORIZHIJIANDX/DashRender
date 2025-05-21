#include "PCH.h"
#include "TGATextureLoader.h"
#include "DirectXTex/DirectXTex.h"
#include "Utility/StringUtility.h"
#include "TextureLoaderHelper.h"

using namespace DirectX;

namespace Dash
{
	TGA_FLAGS GetTGALoadFlag(ETGA_LOAD_FLAGS mask)
	{
		TGA_FLAGS loadFlag = static_cast<TGA_FLAGS>(0);
		if (EnumMaskContains(mask, ETGA_LOAD_FLAGS::TGA_FLAGS_ALLOW_ALL_ZERO_ALPHA)) loadFlag |= TGA_FLAGS::TGA_FLAGS_ALLOW_ALL_ZERO_ALPHA;
		if (EnumMaskContains(mask, ETGA_LOAD_FLAGS::TGA_FLAGS_BGR)) loadFlag |= TGA_FLAGS::TGA_FLAGS_BGR;
		if (EnumMaskContains(mask, ETGA_LOAD_FLAGS::TGA_FLAGS_DEFAULT_SRGB)) loadFlag |= TGA_FLAGS::TGA_FLAGS_DEFAULT_SRGB;
		if (EnumMaskContains(mask, ETGA_LOAD_FLAGS::TGA_FLAGS_FORCE_LINEAR)) loadFlag |= TGA_FLAGS::TGA_FLAGS_FORCE_LINEAR;
		if (EnumMaskContains(mask, ETGA_LOAD_FLAGS::TGA_FLAGS_FORCE_SRGB)) loadFlag |= TGA_FLAGS::TGA_FLAGS_FORCE_SRGB;
		if (EnumMaskContains(mask, ETGA_LOAD_FLAGS::TGA_FLAGS_IGNORE_SRGB)) loadFlag |= TGA_FLAGS::TGA_FLAGS_IGNORE_SRGB;

		return loadFlag;
	}

	bool LoadTGATextureFromFile(const std::string& fileName, ETGA_LOAD_FLAGS loadFlags, FTextureBufferDescription& textureDescription, std::vector<FSubResourceData>& subResource, std::vector<uint8>& decodedData)
	{
		std::wstring wFileName = FStringUtility::UTF8ToWideString(fileName);

		TexMetadata metadata;
		ScratchImage image;

		HRESULT result = LoadFromTGAFile(wFileName.c_str(), GetTGALoadFlag(loadFlags), &metadata, image);

		if (FAILED(result))
		{
			return false;
		}

		InitTextureData(metadata, image, textureDescription, subResource, decodedData);

		return true;
	}
}