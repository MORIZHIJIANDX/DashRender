#include "PCH.h"
#include "StaticMeshLoader.h"
#include "Utility/FileUtility.h"

#include "assimp/Importer.hpp"   // C++ importer interface
#include "assimp/scene.h"        // Output data structure
#include "assimp/postprocess.h"  // Post processing flags


namespace Dash
{
    bool LoadStaticMeshFromFile(const std::string filePath, FImportedStaticMeshData& importedMeshData)
    {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(filePath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LOG_ERROR << "ERROR::ASSIMP::" << import.GetErrorString();
            return false;
        }

        // Track material per submesh
        std::vector<uint32_t> submeshToMaterialIndex;
        std::vector<FMeshSectionData>& meshSections = importedMeshData.sectionData;

        // Load mesh
        {
            std::vector<uint32_t>& indices = importedMeshData.indices;

            // Reserve space
            {
                uint32_t totalVertexs{ 0 };
                uint32_t maxTexCoord{ 0 };
                for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
                {
                    totalVertexs += scene->mMeshes[i]->mNumVertices;
                    maxTexCoord = FMath::Max(scene->mMeshes[i]->GetNumUVChannels(), maxTexCoord);
                }
                               
                ASSERT(totalVertexs != 0);

                importedMeshData.numVertexes = totalVertexs;
                importedMeshData.numTexCoord = maxTexCoord;

                importedMeshData.PositionData.reserve(totalVertexs);     
                importedMeshData.NormalData.reserve(totalVertexs);
                importedMeshData.TangentData.reserve(totalVertexs);
                importedMeshData.VertexColorData.reserve(totalVertexs);
                importedMeshData.UVData.reserve(totalVertexs * maxTexCoord);
            }

            // Go through all meshes
            for (uint32_t mesh_idx = 0; mesh_idx < scene->mNumMeshes; ++mesh_idx)
            {
                aiMesh* mesh = scene->mMeshes[mesh_idx];

                // Track submesh
                FMeshSectionData sectionData{};
                sectionData.vertexStart = (uint32_t)importedMeshData.PositionData.size();
                sectionData.vertexCount = mesh->mNumVertices;
                sectionData.indexStart = (uint32_t)indices.size();
                sectionData.indexCount = 0;
                // Count indices
                for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
                {
                    const aiFace& face = mesh->mFaces[faceIndex];
                    for (uint32_t index_idx = 0; index_idx < face.mNumIndices; ++index_idx)
                        indices.push_back(face.mIndices[index_idx]);
                    sectionData.indexCount += face.mNumIndices;
                }

                // Grab per vertex data
                for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
                {
                    importedMeshData.PositionData.push_back(FVector3f{ mesh->mVertices[vertexIndex].x, mesh->mVertices[vertexIndex].y, mesh->mVertices[vertexIndex].z });

                    for (uint32_t uvIndex = 0; uvIndex < importedMeshData.numTexCoord; uvIndex++)
                    {
                        if (mesh->HasTextureCoords(uvIndex))
                        {
                            importedMeshData.UVData.push_back(FVector2f{ mesh->mTextureCoords[uvIndex][vertexIndex].x, mesh->mTextureCoords[uvIndex][vertexIndex].y });
                        }
                        else
                        {
                            importedMeshData.UVData.push_back(FVector2f{});
                        }
                    }

                    if (mesh->HasNormals())
                    {
                        importedMeshData.NormalData.push_back(FVector3f{ mesh->mNormals[vertexIndex].x, mesh->mNormals[vertexIndex].y, mesh->mNormals[vertexIndex].z });
                    }

                    if (mesh->HasTangentsAndBitangents())
                    {
                        importedMeshData.TangentData.push_back(FVector3f{ mesh->mTangents[vertexIndex].x, mesh->mTangents[vertexIndex].y, mesh->mTangents[vertexIndex].z });
                    }

                    if (mesh->HasVertexColors(0))
                    {
                        importedMeshData.VertexColorData.push_back(FVector4f{mesh->mColors[0]->r, mesh->mColors[0]->g, mesh->mColors[0]->b, mesh->mColors[0]->a});
                    }
                    else
                    {
                        importedMeshData.VertexColorData.push_back(FVector4f{});
                    }
                }

                // Track material
                submeshToMaterialIndex.push_back(mesh->mMaterialIndex);

                // Track submesh
                meshSections.push_back(sectionData);
            }

            importedMeshData.hasNormal = importedMeshData.NormalData.size() > 0;
            importedMeshData.hasTangent = importedMeshData.TangentData.size() > 0;
            importedMeshData.hasUV = importedMeshData.UVData.size() > 0;
            importedMeshData.hasVertexColor = importedMeshData.VertexColorData.size() > 0;

            ASSERT(importedMeshData.PositionData.size() == (importedMeshData.UVData.size() / importedMeshData.numTexCoord));
            ASSERT(importedMeshData.PositionData.size() == importedMeshData.NormalData.size());
            ASSERT(importedMeshData.PositionData.size() == importedMeshData.TangentData.size());

            if (importedMeshData.hasVertexColor)
            {
                ASSERT(importedMeshData.TangentData.size() == importedMeshData.VertexColorData.size());
            }
        }

        // Sanity check
        ASSERT(meshSections.size() == submeshToMaterialIndex.size());

        // Extract material load data
        const auto directory = FFileUtility::GetParentPath(filePath);

        for (size_t matId = 0; matId < scene->mNumMaterials; matId++)
        {
            aiString materialName = scene->mMaterials[matId]->GetName();
            importedMeshData.materialNames.push_back(materialName.C_Str());
        }

        for (size_t sectionId = 0; sectionId < meshSections.size(); sectionId++)
        {
            meshSections[sectionId].materialSlotName = importedMeshData.materialNames[submeshToMaterialIndex[sectionId]];
        }

        return true;
    }
}