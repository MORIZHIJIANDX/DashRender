#include "PCH.h"
#include "MeshLoaderManager.h"
#include "Utility/FileUtility.h"
#include "Utility/StringUtility.h"

namespace Dash
{
    FMeshLoaderManager& FMeshLoaderManager::Get()
    {
        static FMeshLoaderManager manager;
        return manager;
    }

    void FMeshLoaderManager::Init()
    {
        CreateDefaultMeshs();
    }

    void FMeshLoaderManager::Shutdown()
    {
        mImportMeshs.clear();
    }

    const FImportedMeshData& FMeshLoaderManager::LoadMesh(const std::string& meshPath)
    {
		if (!mImportMeshs.contains(meshPath))
		{
			bool loadSucceed = false;

			if (FFileUtility::IsPathExistent(meshPath))
			{
				FImportedMeshData importedMeshData;
				importedMeshData.SourceMeshPath = meshPath;

                loadSucceed = LoadStaticMeshFromFile(meshPath, importedMeshData);

				if (loadSucceed)
				{
					mImportMeshs.emplace(meshPath, importedMeshData);
				}
			}

			if (!loadSucceed)
			{
                DASH_LOG(LogTemp, Error, "Failed to load mesh : {}.", meshPath);
				return mImportMeshs["Cube"];
			}
		}

        mImportMeshs[meshPath].AddRef();
		return mImportMeshs[meshPath];
    }

    bool FMeshLoaderManager::UnloadMesh(const std::string& meshPath)
    {
		if (mImportMeshs.contains(meshPath))
		{
			int32 RefCount = mImportMeshs[meshPath].Release();
			if (RefCount <= 0)
			{
				mImportMeshs.erase(meshPath);
			}
			return true;
		}

		return false;
    }

    void FMeshLoaderManager::CreateDefaultMeshs()
    {
        mImportMeshs.emplace("Cube", CreateCube(1.0f, 1.0f, 1.0f, FVector4f{1.0f, 1.0f, 1.0f, 1.0f }));
    }

	FImportedMeshData FMeshLoaderManager::CreateCube(Scalar width, Scalar height, Scalar depth, FVector4f color)
	{
        const Scalar halfWidth = width * 0.5f;
        const Scalar halfHeight = height * 0.5f;
        const Scalar halfDepth = depth * 0.5f;

        const int numVertex = 24;

        FImportedMeshData data;
        data.HasNormal = true;
        data.HasTangent = true;
        data.HasUV = true;
        data.HasVertexColor = true;
        data.NumTexCoord = 1;
        data.NumVertexes = numVertex;
       
        data.PositionData.resize(numVertex);
        data.NormalData.resize(numVertex);
        data.TangentData.resize(numVertex);
        data.VertexColorData.resize(numVertex);
        data.UVData.resize(numVertex);

        // 右面(+X面)
        data.PositionData[0] = FVector3f(halfWidth, -halfHeight, -halfDepth);
        data.PositionData[1] = FVector3f(halfWidth, halfHeight, -halfDepth);
        data.PositionData[2] = FVector3f(halfWidth, halfHeight, halfDepth);
        data.PositionData[3] = FVector3f(halfWidth, -halfHeight, halfDepth);
        // 左面(-X面)
        data.PositionData[4] = FVector3f(-halfWidth, -halfHeight, halfDepth);
        data.PositionData[5] = FVector3f(-halfWidth, halfHeight, halfDepth);
        data.PositionData[6] = FVector3f(-halfWidth, halfHeight, -halfDepth);
        data.PositionData[7] = FVector3f(-halfWidth, -halfHeight, -halfDepth);
        // 顶面(+Y面)
        data.PositionData[8] = FVector3f(-halfWidth, halfHeight, -halfDepth);
        data.PositionData[9] = FVector3f(-halfWidth, halfHeight, halfDepth);
        data.PositionData[10] = FVector3f(halfWidth, halfHeight, halfDepth);
        data.PositionData[11] = FVector3f(halfWidth, halfHeight, -halfDepth);
        // 底面(-Y面)
        data.PositionData[12] = FVector3f(halfWidth, -halfHeight, -halfDepth);
        data.PositionData[13] = FVector3f(halfWidth, -halfHeight, halfDepth);
        data.PositionData[14] = FVector3f(-halfWidth, -halfHeight, halfDepth);
        data.PositionData[15] = FVector3f(-halfWidth, -halfHeight, -halfDepth);
        // 背面(+Z面)
        data.PositionData[16] = FVector3f(halfWidth, -halfHeight, halfDepth);
        data.PositionData[17] = FVector3f(halfWidth, halfHeight, halfDepth);
        data.PositionData[18] = FVector3f(-halfWidth, halfHeight, halfDepth);
        data.PositionData[19] = FVector3f(-halfWidth, -halfHeight, halfDepth);
        // 正面(-Z面)
        data.PositionData[20] = FVector3f(-halfWidth, -halfHeight, -halfDepth);
        data.PositionData[21] = FVector3f(-halfWidth, halfHeight, -halfDepth);
        data.PositionData[22] = FVector3f(halfWidth, halfHeight, -halfDepth);
        data.PositionData[23] = FVector3f(halfWidth, -halfHeight, -halfDepth);

        for (size_t i = 0; i < 4; ++i)
        {
            // 右面(+X面)
            data.NormalData[i] = FVector3f(1.0f, 0.0f, 0.0f);
            data.TangentData[i] = FVector3f(0.0f, 0.0f, 1.0f);
            data.VertexColorData[i] = color;
            // 左面(-X面)
            data.NormalData[i + 4] = FVector3f(-1.0f, 0.0f, 0.0f);
            data.TangentData[i + 4] = FVector3f(0.0f, 0.0f, -1.0f);
            data.VertexColorData[i + 4] = color;
            // 顶面(+Y面)
            data.NormalData[i + 8] = FVector3f(0.0f, 1.0f, 0.0f);
            data.TangentData[i + 8] = FVector3f(1.0f, 0.0f, 0.0f);
            data.VertexColorData[i + 8] = color;
            // 底面(-Y面)
            data.NormalData[i + 12] = FVector3f(0.0f, -1.0f, 0.0f);
            data.TangentData[i + 12] = FVector3f(-1.0f, 0.0f, 0.0f);
            data.VertexColorData[i + 12] = color;
            // 背面(+Z面)
            data.NormalData[i + 16] = FVector3f(0.0f, 0.0f, 1.0f);
            data.TangentData[i + 16] = FVector3f(-1.0f, 0.0f, 0.0f);
            data.VertexColorData[i + 16] = color;
            // 正面(-Z面)
            data.NormalData[i + 20] = FVector3f(0.0f, 0.0f, -1.0f);
            data.TangentData[i + 20] = FVector3f(1.0f, 0.0f, 0.0f);
            data.VertexColorData[i + 20] = color;
        }

        for (size_t i = 0; i < 6; ++i)
        {
            data.UVData[i * 4] = FVector2f(0.0f, 1.0f);
            data.UVData[i * 4 + 1] = FVector2f(0.0f, 0.0f);
            data.UVData[i * 4 + 2] = FVector2f(1.0f, 0.0f);
            data.UVData[i * 4 + 3] = FVector2f(1.0f, 1.0f);
        }

        data.Indices = {
            0, 1, 2, 2, 3, 0,        // 右面(+X面)
            4, 5, 6, 6, 7, 4,        // 左面(-X面)
            8, 9, 10, 10, 11, 8,    // 顶面(+Y面)
            12, 13, 14, 14, 15, 12,    // 底面(-Y面)
            16, 17, 18, 18, 19, 16, // 背面(+Z面)
            20, 21, 22, 22, 23, 20    // 正面(-Z面)
        };

        data.AddRef();

		return data;
	}
}

