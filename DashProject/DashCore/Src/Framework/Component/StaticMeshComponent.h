#pragma once

#include "Component.h"
#include "Asset/StaticMesh.h"
#include "Graphics/PipelineStateObject.h"

namespace Dash
{
	struct FMeshDrawCommand
	{
		std::vector<FGpuVertexBufferRef> VertexBuffers;
		FGpuIndexBufferRef IndexBuffer;

		const std::map<std::string, std::vector<uint8>>* ConstantBufferMapPtr;
		const std::map<std::string, FTextureRef>* TextureBufferMapPtr;

		FGraphicsPSORef PSO;

		uint32 VertexStart{ 0 };
		uint32 VertexCount{ 0 };
		uint32 IndexStart{ 0 };
		uint32 IndexCount{ 0 };
	};

	class TStaticMeshComponent : public TComponent
	{
	public:
		TStaticMeshComponent(const std::string& name, TActor* owner);
		~TStaticMeshComponent();

		void SetStaticMesh(FStaticMeshRef staticMesh);
		FStaticMeshRef GetStaticMesh() const;

		void SetMaterial(const std::string& materialSlotName, FMaterialRef material);
		FMaterialRef GetMaterial(const std::string& name) const;	

		const std::vector<FMeshDrawCommand>& GetMeshDrawCommands() const { return mCachedMeshDrawCommands; }

	private:

		EPerVertexSemantic GetVertexSemanticType(const std::string& semanticName);
		void BuildMeshDrawCommands();

	private:
		
		FStaticMeshRef mStaticMesh;
		std::map<std::string, FMaterialRef> mOverrideMaterials;
		std::vector<FMeshDrawCommand> mCachedMeshDrawCommands;
	};
}