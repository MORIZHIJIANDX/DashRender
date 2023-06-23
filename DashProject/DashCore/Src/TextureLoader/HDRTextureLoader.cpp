#include "PCH.h"
#include "HDRTextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Dash
{
	bool LoadHDRTextureFromFile(const std::string& fileName, FTextureBufferDescription& textureDescription, D3D12_SUBRESOURCE_DATA& subResource, std::vector<uint8_t>& decodedData)
	{
		stbi_set_flip_vertically_on_load(true);
		int width, height, components;
		int reqComponents = 4;
		float* Data = stbi_loadf(fileName.c_str(), &width, &height, &components, reqComponents);
		ASSERT((components == 3) || (components == 4));

		if (Data)
		{
			LONG RowBytes = width * components * sizeof(float);
			LONG NumBytes = width * height * components * sizeof(float);

			// decodedData
			decodedData.resize(NumBytes);
			memcpy_s(decodedData.data(), NumBytes, Data, NumBytes);

			stbi_image_free(Data);

			// subResource
			subResource.pData = decodedData.data();
			subResource.RowPitch = static_cast<LONG>(RowBytes);
			subResource.SlicePitch = static_cast<LONG>(NumBytes);

			// TextureInfo
			EResourceFormat format = (reqComponents == 3) ? EResourceFormat::RGB32_Float : EResourceFormat::RGBA32_Float;
			textureDescription = FTextureBufferDescription::Create2D(format, width, height);

			return true;
		}

		return false;
	}
}
