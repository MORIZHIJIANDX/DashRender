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

		if (importedMeshData.hasNormal)
		{
			mNormalBuffer = FGraphicsCore::Device->CreateVertexBuffer<FVector3f>(meshPath + "NormalBuffer", importedMeshData.NormalData);
		}

		if (importedMeshData.hasTangent)
		{
			mTangentBuffer = FGraphicsCore::Device->CreateVertexBuffer<FVector3f>(meshPath + "TangentBuffer", importedMeshData.TangentData);
		}

		if (importedMeshData.hasVertexColor)
		{
			mVertexColorBuffer = FGraphicsCore::Device->CreateVertexBuffer<FVector4f>(meshPath + "VertexColorBuffer", importedMeshData.VertexColorData);
		}

		if (importedMeshData.hasUV)
		{
			mTexcoordBuffer = FGraphicsCore::Device->CreateVertexBuffer(meshPath + "TexcoordBuffer", importedMeshData.numVertexes, sizeof(FVector2f) * importedMeshData.numTexCoord, importedMeshData.VertexColorData.data());
		}

		for (size_t i = 0; i < importedMeshData.materialNames.size(); i++)
		{
			mDefaultMaterials[importedMeshData.materialNames[i]] = nullptr;
		}

		mMeshSectionData = importedMeshData.sectionData;
	}

	FStaticMesh::~FStaticMesh()
	{
		
	}

	FStaticMeshRef FStaticMesh::MakeStaticMesh(const std::string& meshPath)
	{
		if (!mStaticMeshResourceMap.contains(meshPath) && !mStaticMeshResourceMap[meshPath].lock())
		{
			FStaticMeshRef newStaticMesh = std::make_shared<FStaticMesh>(meshPath);
			mStaticMeshResourceMap.emplace(meshPath, newStaticMesh);
		}

		return mStaticMeshResourceMap[meshPath].lock();
	}

	void FStaticMesh::SetMaterial(const std::string& materialSlotName, FMaterialRef material)
	{ 
		if (mDefaultMaterials.contains(materialSlotName))
		{
			mDefaultMaterials[materialSlotName] = material;
		}
	}

	const FMaterialRef& FStaticMesh::GetMaterial(const std::string& materialSlotName) const
	{
		const auto& iter = mDefaultMaterials.find(materialSlotName);

		if (iter != mDefaultMaterials.end())
		{
			return iter->second;
		}

		return nullptr;
	}
}