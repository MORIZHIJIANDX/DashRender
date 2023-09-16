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
        std::vector<std::string> texturePaths;
    };

    struct FMeshSectionData
    {
        uint32_t vertexStart{ 0 };
        uint32_t vertexCount{ 0 };
        uint32_t indexStart{ 0 };
        uint32_t indexCount{ 0 };

        std::string materialSlotName;
    };

    struct FImportedStaticMeshData
    {
        bool hasNormal = false;
        bool hasTangent = false;
        bool hasVertexColor = false;
        bool hasUV = false;

        uint32_t numVertexes = 0;
        uint32_t numTexCoord = 0;
        std::vector<uint32_t> indices;

        std::vector<FVector3f> PositionData;
        std::vector<FVector3f> NormalData;
        std::vector<FVector3f> TangentData;
        std::vector<FVector2f> UVData;
        std::vector<FVector4f> VertexColorData;

        std::vector<FMeshSectionData> sectionData;
        std::vector<std::string> materialNames;
    };
}