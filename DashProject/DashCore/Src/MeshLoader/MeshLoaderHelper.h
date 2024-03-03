#pragma once

#include "Math/MathType.h"
#include "Graphics/InputAssemblerLayout.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace Dash
{
    enum class EVertexAttribute : uint8_t
    {
        Position,
        Normal,
        UV,
        Tangent,
        VertexColor
    };

    struct FMeshSectionTextureInfo {
        std::vector<std::string> TexturePaths;
    };

    struct FMeshSectionData
    {
        uint32_t VertexStart{ 0 };
        uint32_t VertexCount{ 0 };
        uint32_t IndexStart{ 0 };
        uint32_t IndexCount{ 0 };

        std::string MaterialSlotName;
    };

    struct FImportedStaticMeshData
    {
        bool HasNormal = false;
        bool HasTangent = false;
        bool HasVertexColor = false;
        bool HasUV = false;

        uint32_t NumVertexes = 0;
        uint32_t NumTexCoord = 0;
        std::vector<uint32_t> Indices;

        std::vector<FVector3f> PositionData;
        std::vector<FVector3f> NormalData;
        std::vector<FVector3f> TangentData;
        std::vector<FVector2f> UVData;
        std::vector<FVector4f> VertexColorData;

        std::vector<FMeshSectionData> SectionData;
        std::vector<std::string> MaterialNames;
    };
}