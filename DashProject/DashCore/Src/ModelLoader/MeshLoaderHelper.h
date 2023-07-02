#pragma once

#include "Math/MathType.h"
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

    enum class EMeshVertexType
    {
        PositionNormalUV,
        PositionNormalVertexColorUV,
        PositionNormalTangentUV,
        PositionNormalTangentVertexColorUV,
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

        std::string sectionName;

        std::vector<int32_t> materialIndex;
    };

    struct FMeshVertexTypePNTVU
    {
        FVector3f position;
        FVector3f normal;
        FVector3f tangent;
        FVector4f vertexColor;
        FVector2f uv;
    };

    template<typename FVertexType>
    struct FStaticMeshData
    {
        std::vector<uint32_t> indices;
        std::vector<FVertexType> vertexes;
    };

    struct FImportedStaticMeshData
    {
        int32_t numVertexes;
        std::vector<uint32_t> indices;
        std::unordered_map<EVertexAttribute, std::vector<uint8_t>> vertexData;
        std::vector<FMeshSectionData> sectionData;
        std::vector<FMeshSectionTextureInfo> textureInfo;

        void Func()
        {
            FMeshVertexTypePNTVU vertex;

            uint32_t positionDataOffset = 0;

            vertex.position = reinterpret_cast<Scalar*>(vertexData[EVertexAttribute::Position].data() + positionDataOffset);
        }

        template<typename FVertexType>
        FStaticMeshData<FVertexType> GetMeshData() const;
    };

    template<typename FVertexType>
    FORCEINLINE FStaticMeshData<FVertexType> FImportedStaticMeshData::GetMeshData() const
    {
        ASSERT_FAIL("Invalid Vertex Type!");
        return FStaticMeshData<FVertexType>{};
    }

    template<>
    FORCEINLINE FStaticMeshData<FMeshVertexTypePNTVU> FImportedStaticMeshData::GetMeshData<FMeshVertexTypePNTVU>() const
    {
        FStaticMeshData<FMeshVertexTypePNTVU> meshData;

        meshData.indices = indices;
        meshData.vertexes.reserve(numVertexes);
        
        uint32_t positionDataOffset = 0;
        uint32_t normalDataOffset = 0;
        uint32_t tangentDataOffset = 0;
        uint32_t vertexColorDataOffset = 0;
        uint32_t uvDataOffset = 0;

        for (int32_t vertexIndex = 0; vertexIndex < numVertexes; vertexIndex++)
        {
            FMeshVertexTypePNTVU vertex;

            vertex.position = reinterpret_cast<const Scalar*>(vertexData.at(EVertexAttribute::Position).data() + positionDataOffset);
            vertex.normal = reinterpret_cast<const Scalar*>(vertexData.at(EVertexAttribute::Normal).data() + normalDataOffset);
            vertex.tangent = reinterpret_cast<const Scalar*>(vertexData.at(EVertexAttribute::Tangent).data() + tangentDataOffset);
            vertex.vertexColor = reinterpret_cast<const Scalar*>(vertexData.at(EVertexAttribute::VertexColor).data() + vertexColorDataOffset);
            vertex.uv = reinterpret_cast<const Scalar*>(vertexData.at(EVertexAttribute::UV).data() + uvDataOffset);

            positionDataOffset += sizeof vertex.position;
            normalDataOffset += sizeof vertex.normal;
            tangentDataOffset += sizeof vertex.tangent;
            vertexColorDataOffset += sizeof vertex.vertexColor;
            uvDataOffset += sizeof vertex.uv;
        }

        return meshData;
    }

}