#include "PCH.h"
#include "DDSTextureLoader.h"
#include "DirectXTex/DirectXTex.h"
#include "Utility/StringUtility.h"
#include "TextureLoaderHelper.h"

using namespace DirectX;

namespace Dash
{
	DDS_FLAGS GetDDSLoadFlag(EDDS_LOAD_FLAGS mask)
	{
		DDS_FLAGS loadFlag = static_cast<DDS_FLAGS>(0);
		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_ALLOW_LARGE_FILES)) loadFlag |= DDS_FLAGS::DDS_FLAGS_ALLOW_LARGE_FILES;
		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_BAD_DXTN_TAILS)) loadFlag |= DDS_FLAGS::DDS_FLAGS_BAD_DXTN_TAILS;
		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_EXPAND_LUMINANCE)) loadFlag |= DDS_FLAGS::DDS_FLAGS_EXPAND_LUMINANCE;
		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_FORCE_DX10_EXT)) loadFlag |= DDS_FLAGS::DDS_FLAGS_FORCE_DX10_EXT;
		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_FORCE_DX10_EXT_MISC2)) loadFlag |= DDS_FLAGS::DDS_FLAGS_FORCE_DX10_EXT_MISC2;
		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_FORCE_DX9_LEGACY)) loadFlag |= DDS_FLAGS::DDS_FLAGS_FORCE_DX9_LEGACY;

		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_FORCE_DXT5_RXGB)) loadFlag |= DDS_FLAGS::DDS_FLAGS_FORCE_DXT5_RXGB;
		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_FORCE_RGB)) loadFlag |= DDS_FLAGS::DDS_FLAGS_FORCE_RGB;
		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_LEGACY_DWORD)) loadFlag |= DDS_FLAGS::DDS_FLAGS_LEGACY_DWORD;
		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_NO_16BPP)) loadFlag |= DDS_FLAGS::DDS_FLAGS_NO_16BPP;
		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_NO_LEGACY_EXPANSION)) loadFlag |= DDS_FLAGS::DDS_FLAGS_NO_LEGACY_EXPANSION;
		if (EnumMaskContains(mask, EDDS_LOAD_FLAGS::DDS_FLAGS_NO_R10B10G10A2_FIXUP)) loadFlag |= DDS_FLAGS::DDS_FLAGS_NO_R10B10G10A2_FIXUP;

		return loadFlag;
	}

	bool LoadDDSTextureFromFile(const std::string& fileName, EDDS_LOAD_FLAGS loadFlags, FTextureBufferDescription& textureDescription, std::vector<FSubResourceData>& subResource, std::vector<uint8_t>& decodedData)
	{
		std::wstring wFileName = FStringUtility::UTF8ToWideString(fileName);

		TexMetadata metadata;
		ScratchImage image;

		HRESULT result = LoadFromDDSFile(wFileName.c_str(), GetDDSLoadFlag(loadFlags), &metadata, image);

		if (FAILED(result))
		{
			return false;
		}

		InitTextureData(metadata, image, textureDescription, subResource, decodedData);

		return true;
	}
}