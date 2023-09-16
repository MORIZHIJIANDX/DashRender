#include "DirectXTex/DirectXTex.h"
#include "Graphics/ResourceDescription.h"
#include "Graphics/SubResourceData.h"

namespace Dash
{
	struct FImportedTextureData
	{
		friend class FTextureLoaderManager;

		std::string SourceTexturePath;
		FTextureBufferDescription TextureDescription;
		std::vector<FSubResourceData> SubResource;
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

	void InitTextureData(const DirectX::TexMetadata& metadata,
		DirectX::ScratchImage& scratchImage,
		FTextureBufferDescription& textureDescription,
		std::vector<FSubResourceData>& subResources,
		std::vector<uint8_t>& decodedData);
}