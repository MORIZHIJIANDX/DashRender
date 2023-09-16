#pragma once

#include "Graphics/SubResourceData.h"
#include "Graphics/ResourceDescription.h"

namespace Dash
{
    enum class ETGA_LOAD_FLAGS : unsigned long
    {
        TGA_FLAGS_NONE = 0x0,

        TGA_FLAGS_BGR = 0x1,
        // 24bpp files are returned as BGRX; 32bpp files are returned as BGRA

        TGA_FLAGS_ALLOW_ALL_ZERO_ALPHA = 0x2,
        // If the loaded image has an all zero alpha channel, normally we assume it should be opaque. This flag leaves it alone.

        TGA_FLAGS_IGNORE_SRGB = 0x10,
        // Ignores sRGB TGA 2.0 metadata if present in the file

        TGA_FLAGS_FORCE_SRGB = 0x20,
        // Writes sRGB metadata into the file reguardless of format (TGA 2.0 only)

        TGA_FLAGS_FORCE_LINEAR = 0x40,
        // Writes linear gamma metadata into the file reguardless of format (TGA 2.0 only)

        TGA_FLAGS_DEFAULT_SRGB = 0x80,
        // If no colorspace is specified in TGA 2.0 metadata, assume sRGB
    };

    ENABLE_BITMASK_OPERATORS(ETGA_LOAD_FLAGS);

    bool LoadTGATextureFromFile(
        const std::string& fileName,
        ETGA_LOAD_FLAGS loadFlags,
        FTextureBufferDescription& textureDescription,
        std::vector<FSubResourceData>& subResource,
        std::vector<uint8_t>& decodedData);
}