#pragma once

#include "Graphics/SubResourceData.h"
#include "Graphics/ResourceDescription.h"

namespace Dash
{
    enum class EWIC_LOAD_FLAGS : unsigned long
    {
        WIC_FLAGS_NONE = 0x0,

        WIC_FLAGS_FORCE_RGB = 0x1,
        // Loads DXGI 1.1 BGR formats as DXGI_FORMAT_R8G8B8A8_UNORM to avoid use of optional WDDM 1.1 formats

        WIC_FLAGS_NO_X2_BIAS = 0x2,
        // Loads DXGI 1.1 X2 10:10:10:2 format as DXGI_FORMAT_R10G10B10A2_UNORM

        WIC_FLAGS_NO_16BPP = 0x4,
        // Loads 565, 5551, and 4444 formats as 8888 to avoid use of optional WDDM 1.2 formats

        WIC_FLAGS_ALLOW_MONO = 0x8,
        // Loads 1-bit monochrome (black & white) as R1_UNORM rather than 8-bit grayscale

        WIC_FLAGS_ALL_FRAMES = 0x10,
        // Loads all images in a multi-frame file, converting/resizing to match the first frame as needed, defaults to 0th frame otherwise

        WIC_FLAGS_IGNORE_SRGB = 0x20,
        // Ignores sRGB metadata if present in the file

        WIC_FLAGS_FORCE_SRGB = 0x40,
        // Writes sRGB metadata into the file reguardless of format

        WIC_FLAGS_FORCE_LINEAR = 0x80,
        // Writes linear gamma metadata into the file reguardless of format

        WIC_FLAGS_DEFAULT_SRGB = 0x100,
        // If no colorspace is specified, assume sRGB

        WIC_FLAGS_DITHER = 0x10000,
        // Use ordered 4x4 dithering for any required conversions

        WIC_FLAGS_DITHER_DIFFUSION = 0x20000,
        // Use error-diffusion dithering for any required conversions

        WIC_FLAGS_FILTER_POINT = 0x100000,
        WIC_FLAGS_FILTER_LINEAR = 0x200000,
        WIC_FLAGS_FILTER_CUBIC = 0x300000,
        WIC_FLAGS_FILTER_FANT = 0x400000, // Combination of Linear and Box filter
        // Filtering mode to use for any required image resizing (only needed when loading arrays of differently sized images; defaults to Fant)
    };

    ENABLE_BITMASK_OPERATORS(EWIC_LOAD_FLAGS);

    bool LoadWICTextureFromFile(
        const std::string& fileName,
        EWIC_LOAD_FLAGS loadFlags,
        FTextureBufferDescription& textureDescription,
        std::vector<FSubResourceData>& subResource,
        std::vector<uint8_t>& decodedData);
}