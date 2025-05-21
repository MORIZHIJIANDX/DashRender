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
		std::vector<uint8> DecodedData;

	private:

		int32 AddRef()
		{
			return ++RefCount;
		}

		int32 Release()
		{
			return --RefCount;
		}

	private:

		int32 RefCount = 0;
	};

	void InitTextureData(const DirectX::TexMetadata& metadata,
		DirectX::ScratchImage& scratchImage,
		FTextureBufferDescription& textureDescription,
		std::vector<FSubResourceData>& subResources,
		std::vector<uint8>& decodedData);
}