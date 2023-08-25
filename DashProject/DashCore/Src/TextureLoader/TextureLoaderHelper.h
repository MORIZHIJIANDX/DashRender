#include "DirectXTex/DirectXTex.h"
#include "Graphics/ResourceDescription.h"

namespace DirectX
{
	void InitTextureData(const TexMetadata& metadata,
        ScratchImage& image,
        Dash::FTextureBufferDescription& textureDescription,
        D3D12_SUBRESOURCE_DATA& subResource,
        std::vector<uint8_t>& decodedData);
}

namespace Dash
{
	struct FImportedTextureData
	{
		friend class FTextureLoaderManager;

		std::string SourceTexturePath;
		FTextureBufferDescription TextureDescription;
		D3D12_SUBRESOURCE_DATA SubResource;
		std::vector<uint8_t> DecodedData;

	private:

		int32_t AddRef()
		{
			return ++RefCount;
		}

		int32_t Release()
		{
			return --RefCount;
		}

	private:

		int32_t RefCount = 0;
	};
}