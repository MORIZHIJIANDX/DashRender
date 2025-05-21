#include "PCH.h"
#include "HDRTextureLoader.h"
#include "DirectXTex/DirectXTex.h"
#include "Utility/StringUtility.h"
#include "TextureLoaderHelper.h"

using namespace DirectX;

namespace Dash
{
	bool LoadHDRTextureFromFile(const std::string& fileName, FTextureBufferDescription& textureDescription, std::vector<FSubResourceData>& subResource, std::vector<uint8>& decodedData)
	{
		std::wstring wFileName = FStringUtility::UTF8ToWideString(fileName);

		TexMetadata metadata;
		ScratchImage image;

		HRESULT result = LoadFromHDRFile(wFileName.c_str(), &metadata, image);

		if (FAILED(result))
		{
			return false;
		}

		InitTextureData(metadata, image, textureDescription, subResource, decodedData);

		return true;
	}
}