#pragma once

#include "Graphics/RenderDevice.h"
#include "Material.h"
#include "MeshLoader/MeshLoaderHelper.h"
#include "Graphics/GpuBuffer.h"

namespace Dash
{
	class FStaticMesh
	{
	public:
		FStaticMesh(const std::string& meshPath);
		~FStaticMesh();

		void SetMaterial(const std::string& materialSlotName, FMaterialRef material);
		FMaterialRef GetMaterial(const std::string& materialSlotName) const;
		const std::map<std::string, FMaterialRef>& GetMaterials() const { return mDefaultMaterials; } 

		const FGpuVertexBufferRef& GetPositionBuffer() const { return mPostionBuffer; }
		const FGpuVertexBufferRef& GetNormalBuffer() const { return mNormalBuffer; }
		const FGpuVertexBufferRef& GetTangentBuffer() const { return mTangentBuffer; }
		const FGpuVertexBufferRef& GetVertexColorBuffer() const { return mVertexColorBuffer; }
		const FGpuVertexBufferRef& GetTexCoordBuffer() const { return mTexCoordBuffer; }

		const FGpuIndexBufferRef& GetIndexBuffer() const { return mIndexBuffer; }

		const std::vector<FMeshSectionData>& GetMeshSections() const { return mMeshSectionData; }

	private:
		FGpuVertexBufferRef mPostionBuffer;
		FGpuVertexBufferRef mNormalBuffer;
		FGpuVertexBufferRef mTangentBuffer;
		FGpuVertexBufferRef mVertexColorBuffer;
		FGpuVertexBufferRef mTexCoordBuffer;

		FGpuIndexBufferRef mIndexBuffer;

		std::map<std::string, FMaterialRef> mDefaultMaterials;
		std::vector<FMeshSectionData> mMeshSectionData;
	}; 
}