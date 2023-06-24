#include "PCH.h"
#include "HDRTextureLoader.h"
#include "ThirdParty/DirectXTex/DirectXTex.h"
#include "Utility/StringUtility.h"
#include "TextureLoaderHelper.h"

using namespace DirectX;

namespace Dash
{
	bool LoadHDRTextureFromFile(const std::string& fileName, FTextureBufferDescription& textureDescription, D3D12_SUBRESOURCE_DATA& subResource, std::vector<uint8_t>& decodedData)
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