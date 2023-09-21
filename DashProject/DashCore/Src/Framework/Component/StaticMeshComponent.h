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

		std::map<std::string, std::shared_ptr<std::vector<uint8_t>>> ConstantBufferMap;
		std::map<std::string, FTextureRef> TextureBufferMap;

		FGraphicsPSORef PSO;

		uint32_t vertexStart{ 0 };
		uint32_t vertexCount{ 0 };
		uint32_t indexStart{ 0 };
		uint32_t indexCount{ 0 };
	};

	class TStaticMeshComponent : public TComponent
	{
	public:
		TStaticMeshComponent(const std::string& name, TActor* owner);
		~TStaticMeshComponent();

		void SetStaticMesh(FStaticMeshRef staticMesh);
		FStaticMeshRef GetStaticMesh() const;

		const std::vector<FMeshDrawCommand>& GetMeshDrawCommands() const { return mCachedMeshDrawCommands; }

	private:

		EPerVertexSemantic GetVertexSemanticType(const std::string& semanticName);
		void BuildMeshDrawCommands();

	private:
		
		FStaticMeshRef mStaticMesh;
		std::vector<FMeshDrawCommand> mCachedMeshDrawCommands;
	};
}