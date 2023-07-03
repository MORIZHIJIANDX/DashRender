#include "PCH.h"
#include "StaticMeshLoader.h"
#include "Utility/FileUtility.h"

/*
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
*/
//#include <Assimp/scene.h>           // Output data structure
//#include <Assimp/Importer.hpp>      // C++ importer interface
//#include <Assimp/postprocess.h>     // Post processing flags
//#include <Assimp/pbrmaterial.h>

#include "ThirdParty/assimp/Importer.hpp"
#include "ThirdParty/assimp/scene.h"
#include "ThirdParty/assimp/postprocess.h"


namespace Dash
{
    bool LoadStaticMeshFromFile(const std::string filePath, FImportedStaticMeshData& importedMeshData)
    {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(filePath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LOG_ERROR << "ERROR::ASSIMP::" << import.GetErrorString();
            return false;
        }

        importedMeshData.vertexData[EVertexAttribute::Position] = {};
        importedMeshData.vertexData[EVertexAttribute::UV] = {};
        importedMeshData.vertexData[EVertexAttribute::Normal] = {};
        importedMeshData.vertexData[EVertexAttribute::Tangent] = {};
        importedMeshData.vertexData[EVertexAttribute::VertexColor] = {};

        // Track material per submesh
        std::vector<uint32_t> submesh_to_material_idx;
        std::vector<FMeshSectionData>& meshSections = importedMeshData.sectionData;

        // Load mesh
        {
            std::vector<uint32_t>& indices = importedMeshData.indices;
            std::vector<FVector3f> positions, normals, tangents;
            std::vector<FVector4f> vertexColors;
            std::vector<FVector2f> uvs;

            // Reserve space
            {
                uint32_t total_verts{ 0 };
                for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
                    total_verts += scene->mMeshes[i]->mNumVertices;
                
                ASSERT(total_verts != 0);

                importedMeshData.numVertexes = total_verts;

                positions.reserve(total_verts);
                uvs.reserve(total_verts);
                normals.reserve(total_verts);
                tangents.reserve(total_verts);
                vertexColors.reserve(total_verts);
            }

            // Go through all meshes
            for (uint32_t mesh_idx = 0; mesh_idx < scene->mNumMeshes; ++mesh_idx)
            {
                aiMesh* mesh = scene->mMeshes[mesh_idx];

                // Track submesh
                FMeshSectionData sectionData{};
                sectionData.sectionName = mesh->mName.C_Str();
                sectionData.vertexStart = (uint32_t)positions.size();
                sectionData.vertexCount = mesh->mNumVertices;
                sectionData.indexStart = (uint32_t)indices.size();
                sectionData.indexCount = 0;
                // Count indices
                for (uint32_t face_idx = 0; face_idx < mesh->mNumFaces; ++face_idx)
                {
                    const aiFace& face = mesh->mFaces[face_idx];
                    for (uint32_t index_idx = 0; index_idx < face.mNumIndices; ++index_idx)
                        indices.push_back(face.mIndices[index_idx]);
                    sectionData.indexCount += face.mNumIndices;
                }

                // Grab per vertex data
                for (uint32_t vert_idx = 0; vert_idx < mesh->mNumVertices; ++vert_idx)
                {
                    positions.push_back(FVector3f{ mesh->mVertices[vert_idx].x, mesh->mVertices[vert_idx].y, mesh->mVertices[vert_idx].z });

                    if (mesh->HasTextureCoords(0))
                        uvs.push_back(FVector2f{ mesh->mTextureCoords[0][vert_idx].x, mesh->mTextureCoords[0][vert_idx].y });

                    if (mesh->HasNormals())
                        normals.push_back(FVector3f{ mesh->mNormals[vert_idx].x, mesh->mNormals[vert_idx].y, mesh->mNormals[vert_idx].z });

                    if (mesh->HasTangentsAndBitangents())
                    {
                        tangents.push_back(FVector3f{ mesh->mTangents[vert_idx].x, mesh->mTangents[vert_idx].y, mesh->mTangents[vert_idx].z });
                        //bitangents.push_back({ mesh->mBitangents[vert_idx].x, mesh->mBitangents[vert_idx].y, mesh->mBitangents[vert_idx].z });
                    }

                    if (mesh->HasVertexColors(0))
                    {
                        vertexColors.push_back(FVector4f{mesh->mColors[0]->r, mesh->mColors[0]->g, mesh->mColors[0]->b, mesh->mColors[0]->a});
                    }
                }

                // Track material
                submesh_to_material_idx.push_back(mesh->mMaterialIndex);

                // Track submesh
                meshSections.push_back(sectionData);
            }

            importedMeshData.hasNormal = normals.size() > 0;
            importedMeshData.hasTangent = tangents.size() > 0;
            importedMeshData.hasUV = uvs.size() > 0;
            importedMeshData.hasVertexColor = vertexColors.size() > 0;

            // Resize standardized buffers
            importedMeshData.vertexData[EVertexAttribute::Position].resize(positions.size() * sizeof(positions[0]));
            importedMeshData.vertexData[EVertexAttribute::UV].resize(uvs.size() * sizeof(uvs[0]));
            importedMeshData.vertexData[EVertexAttribute::Normal].resize(normals.size() * sizeof(normals[0]));
            importedMeshData.vertexData[EVertexAttribute::Tangent].resize(tangents.size() * sizeof(tangents[0]));
            importedMeshData.vertexData[EVertexAttribute::VertexColor].resize(vertexColors.size() * sizeof(vertexColors[0]));

            // Copy to standardized buffers
            std::memcpy(importedMeshData.vertexData[EVertexAttribute::Position].data(), positions.data(), positions.size() * sizeof(positions[0]));
            std::memcpy(importedMeshData.vertexData[EVertexAttribute::UV].data(), uvs.data(), uvs.size() * sizeof(uvs[0]));
            std::memcpy(importedMeshData.vertexData[EVertexAttribute::Normal].data(), normals.data(), normals.size() * sizeof(normals[0]));
            std::memcpy(importedMeshData.vertexData[EVertexAttribute::Tangent].data(), tangents.data(), tangents.size() * sizeof(tangents[0]));
            std::memcpy(importedMeshData.vertexData[EVertexAttribute::VertexColor].data(), vertexColors.data(), vertexColors.size() * sizeof(vertexColors[0]));

            ASSERT(positions.size() == uvs.size());
            ASSERT(uvs.size() == normals.size());
            ASSERT(normals.size() == tangents.size());

            if (importedMeshData.hasVertexColor)
            {
                ASSERT(tangents.size() == vertexColors.size());
            }
        }

        // Sanity check
        ASSERT(meshSections.size() == submesh_to_material_idx.size());

        // Extract material load data
        const auto directory = FFileUtility::GetParentPath(filePath);
        for (auto mat_id : submesh_to_material_idx)
        {
            const aiMaterial* material = scene->mMaterials[mat_id];

            // Save material
            //m_loadedModel->materials.push_back(ExtractMaterial(material, directory + "/"));
        }

        return true;
    }
}