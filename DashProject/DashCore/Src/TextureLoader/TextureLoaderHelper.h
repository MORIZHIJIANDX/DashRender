#include "ThirdParty/DirectXTex/DirectXTex.h"
#include "Graphics/ResourceDescription.h"

namespace DirectX
{
	void InitTextureData(const TexMetadata& metadata,
        ScratchImage& image,
        Dash::FTextureBufferDescription& textureDescription,
        D3D12_SUBRESOURCE_DATA& subResource,
        std::vector<uint8_t>& decodedData);
}