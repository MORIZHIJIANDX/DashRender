#include "PCH.h"
#include "WICTextureLoader.h"
#include "DirectXTex/DirectXTex.h"
#include "Utility/StringUtility.h"
#include "TextureLoaderHelper.h"

using namespace DirectX;

namespace Dash
{
	WIC_FLAGS GetWICLoadFlag(EWIC_LOAD_FLAGS mask)
	{
		WIC_FLAGS loadFlag = static_cast<WIC_FLAGS>(0);
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_ALLOW_MONO)) loadFlag |= WIC_FLAGS::WIC_FLAGS_ALLOW_MONO;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_ALL_FRAMES)) loadFlag |= WIC_FLAGS::WIC_FLAGS_ALL_FRAMES;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_DEFAULT_SRGB)) loadFlag |= WIC_FLAGS::WIC_FLAGS_DEFAULT_SRGB;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_DITHER)) loadFlag |= WIC_FLAGS::WIC_FLAGS_DITHER;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_DITHER_DIFFUSION)) loadFlag |= WIC_FLAGS::WIC_FLAGS_DITHER_DIFFUSION;

		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_FILTER_CUBIC)) loadFlag |= WIC_FLAGS::WIC_FLAGS_FILTER_CUBIC;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_FILTER_FANT)) loadFlag |= WIC_FLAGS::WIC_FLAGS_FILTER_FANT;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_FILTER_LINEAR)) loadFlag |= WIC_FLAGS::WIC_FLAGS_FILTER_LINEAR;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_FILTER_POINT)) loadFlag |= WIC_FLAGS::WIC_FLAGS_FILTER_POINT;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_FORCE_LINEAR)) loadFlag |= WIC_FLAGS::WIC_FLAGS_FORCE_LINEAR;

		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_FORCE_RGB)) loadFlag |= WIC_FLAGS::WIC_FLAGS_FORCE_RGB;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_FORCE_SRGB)) loadFlag |= WIC_FLAGS::WIC_FLAGS_FORCE_SRGB;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_IGNORE_SRGB)) loadFlag |= WIC_FLAGS::WIC_FLAGS_IGNORE_SRGB;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_NO_16BPP)) loadFlag |= WIC_FLAGS::WIC_FLAGS_NO_16BPP;
		if (EnumMaskContains(mask, EWIC_LOAD_FLAGS::WIC_FLAGS_NO_X2_BIAS)) loadFlag |= WIC_FLAGS::WIC_FLAGS_NO_X2_BIAS;

		return loadFlag;
	}

	bool LoadWICTextureFromFile(const std::string& fileName, EWIC_LOAD_FLAGS loadFlags, FTextureBufferDescription& textureDescription, std::vector<FSubResourceData>& subResource, std::vector<uint8_t>& decodedData)
	{
		std::wstring wFileName = FStringUtility::UTF8ToWideString(fileName);

		TexMetadata metadata;
		ScratchImage image;

		HRESULT result = LoadFromWICFile(wFileName.c_str(), GetWICLoadFlag(loadFlags), &metadata, image);

		if (FAILED(result))
		{
			return false;
		}

		InitTextureData(metadata, image, textureDescription, subResource, decodedData);

		return true;
	}
}
