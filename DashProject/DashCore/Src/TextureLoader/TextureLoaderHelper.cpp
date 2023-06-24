#include "PCH.h"
#include "TextureLoaderHelper.h"

namespace DirectX
{
	void InitTextureData(const TexMetadata& metadata, ScratchImage& image, Dash::FTextureBufferDescription& textureDescription, D3D12_SUBRESOURCE_DATA& subResource, std::vector<uint8_t>& decodedData)
	{
		Dash::EResourceFormat format = Dash::ResourceFormatFromD3DFormat(metadata.format);

		textureDescription = Dash::FTextureBufferDescription::Create2D(format, static_cast<uint32_t>(metadata.width), static_cast<uint32_t>(metadata.height), static_cast<uint32_t>(metadata.mipLevels));

		decodedData.resize(image.GetPixelsSize());
		memcpy_s(decodedData.data(), image.GetPixelsSize(), image.GetPixels(), image.GetPixelsSize());

		subResource.pData = decodedData.data();
		subResource.RowPitch = image.GetImages()->rowPitch;
		subResource.SlicePitch = image.GetImages()->slicePitch;
	}
}