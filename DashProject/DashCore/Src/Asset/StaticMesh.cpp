#include "PCH.h"
#include "StaticMesh.h"
#include "MeshLoader/MeshLoaderManager.h"
#include "Graphics/GraphicsCore.h"

namespace Dash
{
	FStaticMesh::FStaticMesh(const std::string& meshPath)
	{
		const FImportedMeshData& importedMeshData = FMeshLoaderManager::Get().LoadMesh(meshPath);

		mPostionBuffer = FGraphicsCore::Device->CreateVertexBuffer<FVector3f>(meshPath + "PositionBuffer", importedMeshData.PositionData);

		if (importedMeshData.HasNormal)
		{
			mNormalBuffer = FGraphicsCore::Device->CreateVertexBuffer<FVector3f>(meshPath + "NormalBuffer", importedMeshData.NormalData);
		}

		if (importedMeshData.HasTangent)
		{
			mTangentBuffer = FGraphicsCore::Device->CreateVertexBuffer<FVector3f>(meshPath + "TangentBuffer", importedMeshData.TangentData);
		}

		if (importedMeshData.HasVertexColor)
		{
			mVertexColorBuffer = FGraphicsCore::Device->CreateVertexBuffer<FVector4f>(meshPath + "VertexColorBuffer", importedMeshData.VertexColorData);
		}

		if (importedMeshData.HasUV)
		{
			mTexCoordBuffer = FGraphicsCore::Device->CreateVertexBuffer(meshPath + "TexcoordBuffer", importedMeshData.NumVertexes, sizeof(FVector2f) * importedMeshData.NumTexCoord, importedMeshData.UVData.data());
		}

		for (size_t i = 0; i < importedMeshData.MaterialNames.size(); i++)
		{
			mDefaultMaterials[importedMeshData.MaterialNames[i]] = nullptr;
		}

		mIndexBuffer = FGraphicsCore::Device->CreateIndexBuffer(meshPath + "IndexBuffer", importedMeshData.NumVertexes, importedMeshData.Indices.data(), true);

		mMeshSectionData = importedMeshData.SectionData;
	}

	FStaticMesh::~FStaticMesh()
	{
		
	}

	void FStaticMesh::SetMaterial(const std::string& materialSlotName, FMaterialRef material)
	{ 
		if (mDefaultMaterials.contains(materialSlotName))
		{
			mDefaultMaterials[materialSlotName] = material;
		}
	}

	FMaterialRef FStaticMesh::GetMaterial(const std::string& materialSlotName) const
	{
		const auto& iter = mDefaultMaterials.find(materialSlotName);

		if (iter != mDefaultMaterials.end())
		{
			return iter->second;
		}

		return nullptr;
	}
}