#pragma once

#include "Math/MathType.h"
#include "Graphics/InputAssemblerLayout.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace Dash
{
    enum class EVertexAttribute : uint8
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
        uint32 VertexStart{ 0 };
        uint32 VertexCount{ 0 };
        uint32 IndexStart{ 0 };
        uint32 IndexCount{ 0 };

        std::string MaterialSlotName;
    };

    struct FImportedStaticMeshData
    {
        bool HasNormal = false;
        bool HasTangent = false;
        bool HasVertexColor = false;
        bool HasUV = false;

        uint32 NumVertexes = 0;
        uint32 NumTexCoord = 0;
        std::vector<uint32> Indices;

        std::vector<FVector3f> PositionData;
        std::vector<FVector3f> NormalData;
        std::vector<FVector3f> TangentData;
        std::vector<FVector2f> UVData;
        std::vector<FVector4f> VertexColorData;

        std::vector<FMeshSectionData> SectionData;
        std::vector<std::string> MaterialNames;
    };
}