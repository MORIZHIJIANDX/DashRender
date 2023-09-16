#pragma once

#include "Graphics/RenderDevice.h"
#include "Material.h"
#include "MeshLoader/MeshLoaderHelper.h"

namespace Dash
{
	class FStaticMesh;
	using FStaticMeshRef = std::shared_ptr<FStaticMesh>;

	class FStaticMesh
	{
	public:
		FStaticMesh(const std::string& meshPath);
		~FStaticMesh();

		static FStaticMeshRef MakeStaticMesh(const std::string& texturePath);

		void SetMaterial(const std::string& materialSlotName, FMaterialRef material);
		const FMaterialRef& GetMaterial(const std::string& materialSlotName) const;
		const std::map<std::string, FMaterialRef>& GetMaterials() const { return mDefaultMaterials; } 

		const FGpuVertexBufferRef& GetPositionBuffer() const { return mPostionBuffer; }
		const FGpuVertexBufferRef& GetNormalBuffer() const { return mNormalBuffer; }
		const FGpuVertexBufferRef& GetVertexColorBuffer() const { return mVertexColorBuffer; }
		const FGpuVertexBufferRef& GetTexcoordBuffer() const { return mTexcoordBuffer; }

	private:
		FGpuVertexBufferRef mPostionBuffer;
		FGpuVertexBufferRef mNormalBuffer;
		FGpuVertexBufferRef mTangentBuffer;
		FGpuVertexBufferRef mVertexColorBuffer;
		FGpuVertexBufferRef mTexcoordBuffer;

		std::map<std::string, FMaterialRef> mDefaultMaterials;
		std::vector<FMeshSectionData> mMeshSectionData;

		static std::map<std::string, std::weak_ptr<FStaticMesh>> mStaticMeshResourceMap;
	}; 
}