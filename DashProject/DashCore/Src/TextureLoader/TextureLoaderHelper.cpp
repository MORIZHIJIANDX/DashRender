#include "PCH.h"
#include "TextureLoaderHelper.h"

using namespace DirectX;

namespace Dash
{
	void InitTextureData(const TexMetadata& metadata, ScratchImage& scratchImage, Dash::FTextureBufferDescription& textureDescription, std::vector<FSubResourceData>& subResources, std::vector<uint8_t>& decodedData)
	{
		Dash::EResourceFormat format = Dash::ResourceFormatFromD3DFormat(metadata.format);

		textureDescription = Dash::FTextureBufferDescription::Create2D(format, static_cast<uint32_t>(metadata.width), static_cast<uint32_t>(metadata.height), static_cast<uint32_t>(metadata.mipLevels));

		decodedData.resize(scratchImage.GetPixelsSize());
		memcpy_s(decodedData.data(), scratchImage.GetPixelsSize(), scratchImage.GetPixels(), scratchImage.GetPixelsSize());

		for (size_t i = 0; i < metadata.mipLevels; i++)
		{
			const Image* image = scratchImage.GetImage(i, 0, 0);
			
			subResources.emplace_back(image->pixels, image->rowPitch, image->slicePitch);
		}
	}
}