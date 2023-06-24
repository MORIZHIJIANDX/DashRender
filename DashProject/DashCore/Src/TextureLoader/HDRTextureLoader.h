#pragma once

#include <string>
#include <vector>
#include "Graphics/ResourceDescription.h"

namespace Dash
{
    bool LoadHDRTextureFromFile(
        const std::string& fileName,
        FTextureBufferDescription& textureDescription,
        D3D12_SUBRESOURCE_DATA& subResource,
        std::vector<uint8_t>& decodedData);
}