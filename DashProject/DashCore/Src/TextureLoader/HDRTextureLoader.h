#pragma once

#include "Graphics/SubResourceData.h"
#include "Graphics/ResourceDescription.h"

namespace Dash
{
    bool LoadHDRTextureFromFile(
        const std::string& fileName,
        FTextureBufferDescription& textureDescription,
        std::vector<FSubResourceData>& subResource,
        std::vector<uint8_t>& decodedData);
}