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
            DASH_LOG(LogTemp, Error, "Assimp load error : {}", import.GetErrorString());
            return false;
        }

        // Track material per submesh
        std::vector<uint32> submeshToMaterialIndex;
        std::vector<FMeshSectionData>& meshSections = importedMeshData.SectionData;

        // Load mesh
        {
            std::vector<uint32>& indices = importedMeshData.Indices;

            // Reserve space
            {
                uint32 totalVertexs{ 0 };
                uint32 maxTexCoord{ 0 };
                for (uint32 i = 0; i < scene->mNumMeshes; ++i)
                {
                    totalVertexs += scene->mMeshes[i]->mNumVertices;
                    maxTexCoord = FMath::Max(scene->mMeshes[i]->GetNumUVChannels(), maxTexCoord);
                }
                               
                ASSERT(totalVertexs != 0);

                importedMeshData.NumVertexes = totalVertexs;
                importedMeshData.NumTexCoord = maxTexCoord;

                importedMeshData.PositionData.reserve(totalVertexs);     
                importedMeshData.NormalData.reserve(totalVertexs);
                importedMeshData.TangentData.reserve(totalVertexs);
                importedMeshData.VertexColorData.reserve(totalVertexs);
                importedMeshData.UVData.reserve(totalVertexs * maxTexCoord);
            }

            // Go through all meshes
            for (uint32 mesh_idx = 0; mesh_idx < scene->mNumMeshes; ++mesh_idx)
            {
                aiMesh* mesh = scene->mMeshes[mesh_idx];

                // Track submesh
                FMeshSectionData sectionData{};
                sectionData.VertexStart = (uint32)importedMeshData.PositionData.size();
                sectionData.VertexCount = mesh->mNumVertices;
                sectionData.IndexStart = (uint32)indices.size();
                sectionData.IndexCount = 0;
                // Count indices
                for (uint32 faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
                {
                    const aiFace& face = mesh->mFaces[faceIndex];
                    for (uint32 index_idx = 0; index_idx < face.mNumIndices; ++index_idx)
                        indices.push_back(face.mIndices[index_idx]);
                    sectionData.IndexCount += face.mNumIndices;
                }

                // Grab per vertex data
                for (uint32 vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
                {
                    importedMeshData.PositionData.push_back(FVector3f{ mesh->mVertices[vertexIndex].x, mesh->mVertices[vertexIndex].y, mesh->mVertices[vertexIndex].z });

                    for (uint32 uvIndex = 0; uvIndex < importedMeshData.NumTexCoord; uvIndex++)
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

            importedMeshData.HasNormal = importedMeshData.NormalData.size() > 0;
            importedMeshData.HasTangent = importedMeshData.TangentData.size() > 0;
            importedMeshData.HasUV = importedMeshData.UVData.size() > 0;
            importedMeshData.HasVertexColor = importedMeshData.VertexColorData.size() > 0;

            ASSERT(importedMeshData.PositionData.size() == (importedMeshData.UVData.size() / importedMeshData.NumTexCoord));
            ASSERT(importedMeshData.PositionData.size() == importedMeshData.NormalData.size());
            ASSERT(importedMeshData.PositionData.size() == importedMeshData.TangentData.size());

            if (importedMeshData.HasVertexColor)
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
            importedMeshData.MaterialNames.push_back(materialName.C_Str());
        }

        for (size_t sectionId = 0; sectionId < meshSections.size(); sectionId++)
        {
            meshSections[sectionId].MaterialSlotName = importedMeshData.MaterialNames[submeshToMaterialIndex[sectionId]];
        }

        return true;
    }
}