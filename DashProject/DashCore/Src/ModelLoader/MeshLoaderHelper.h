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
    };

    struct FMeshVertexTypePNTVU
    {
        FVector3f position;
        FVector3f normal;
        FVector3f tangent;
        FVector4f vertexColor;
        FVector2f uv;
    };

    struct FMeshVertexTypePNTU
    {
        FVector3f position;
        FVector3f normal;
        FVector3f tangent;
        FVector2f uv;
    };

    /*
    template<typename FVertexType>
    FInputAssemblerLayout GetInputLayoutForVertexType()
    {
        ASSERT_FAIL("Invalid Vertex Type!");
        return FInputAssemblerLayout{};
    }

    template<>
    FInputAssemblerLayout GetInputLayoutForVertexType<FMeshVertexTypePNTVU>()
    {
        FInputAssemblerLayout inputLayout;
        inputLayout.AddPerVertexLayoutElement("POSITION", 0, EResourceFormat::RGB32_Float);
        inputLayout.AddPerVertexLayoutElement("NORMAL", 0, EResourceFormat::RGB32_Float);
        inputLayout.AddPerVertexLayoutElement("TANGENT", 0, EResourceFormat::RGB32_Float);
        inputLayout.AddPerVertexLayoutElement("COLOR", 0, EResourceFormat::RGBA32_Float);
        inputLayout.AddPerVertexLayoutElement("TEXCOORD", 0, EResourceFormat::RG32_Float);

        return inputLayout;
    }

    template<>
    FInputAssemblerLayout GetInputLayoutForVertexType<FMeshVertexTypePNTU>()
    {
        FInputAssemblerLayout inputLayout;
        inputLayout.AddPerVertexLayoutElement("POSITION", 0, EResourceFormat::RGB32_Float);
        inputLayout.AddPerVertexLayoutElement("NORMAL", 0, EResourceFormat::RGB32_Float);
        inputLayout.AddPerVertexLayoutElement("TANGENT", 0, EResourceFormat::RGB32_Float);
        inputLayout.AddPerVertexLayoutElement("TEXCOORD", 0, EResourceFormat::RG32_Float);

        return inputLayout;
    }
    */

    template<typename FVertexType>
    struct FStaticMeshData
    {
        std::vector<uint32_t> indices;
        std::vector<FVertexType> vertexes;
        std::vector<FMeshSectionData> sectionData;
    };

    struct FImportedStaticMeshData
    {
        bool hasNormal = false;
        bool hasTangent = false;
        bool hasVertexColor = false;
        bool hasUV = false;

        int32_t numVertexes;
        std::vector<uint32_t> indices;
        std::unordered_map<EVertexAttribute, std::vector<uint8_t>> vertexData;
        std::vector<FMeshSectionData> sectionData;
        std::vector<FMeshSectionTextureInfo> textureInfo;

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

        if (!hasVertexColor || !hasTangent || !hasUV || !hasNormal)
        {
            LOG_ERROR << "Missing properties to read!";
            return meshData;
        }

        meshData.indices = indices;
        meshData.sectionData = sectionData;
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

    template<>
    FORCEINLINE FStaticMeshData<FMeshVertexTypePNTU> FImportedStaticMeshData::GetMeshData<FMeshVertexTypePNTU>() const
    {
        FStaticMeshData<FMeshVertexTypePNTU> meshData;

        if (!hasTangent || !hasUV || !hasNormal)
        {
            LOG_ERROR << "Missing properties to read!";
            return meshData;
        }

        meshData.indices = indices;
        meshData.sectionData = sectionData;
        meshData.vertexes.reserve(numVertexes);

        uint32_t positionDataOffset = 0;
        uint32_t normalDataOffset = 0;
        uint32_t tangentDataOffset = 0;
        uint32_t uvDataOffset = 0;

        for (int32_t vertexIndex = 0; vertexIndex < numVertexes; vertexIndex++)
        {
            FMeshVertexTypePNTU vertex;

            vertex.position = reinterpret_cast<const Scalar*>(vertexData.at(EVertexAttribute::Position).data() + positionDataOffset);
            vertex.normal = reinterpret_cast<const Scalar*>(vertexData.at(EVertexAttribute::Normal).data() + normalDataOffset);
            vertex.tangent = reinterpret_cast<const Scalar*>(vertexData.at(EVertexAttribute::Tangent).data() + tangentDataOffset);
            vertex.uv = reinterpret_cast<const Scalar*>(vertexData.at(EVertexAttribute::UV).data() + uvDataOffset);

            positionDataOffset += sizeof vertex.position;
            normalDataOffset += sizeof vertex.normal;
            tangentDataOffset += sizeof vertex.tangent;
            uvDataOffset += sizeof vertex.uv;

            meshData.vertexes.push_back(vertex);
        }

        return meshData;
    }

}