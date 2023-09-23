#include "PCH.h"
#include "StaticMeshComponent.h"
#include "Graphics/GraphicsCore.h"
#include "Graphics/SwapChain.h"

namespace Dash
{
	TStaticMeshComponent::TStaticMeshComponent(const std::string& name, TActor* owner)
		: TComponent(name, owner)
	{
	}

	TStaticMeshComponent::~TStaticMeshComponent()
	{
	}

	void TStaticMeshComponent::SetStaticMesh(FStaticMeshRef staticMesh)
	{
		if (staticMesh)
		{
			mStaticMesh = staticMesh;

			BuildMeshDrawCommands();
		}
		else
		{
			mStaticMesh = nullptr;
			mCachedMeshDrawCommands.clear();
		}
	}

	FStaticMeshRef TStaticMeshComponent::GetStaticMesh() const
	{
		return mStaticMesh;
	}

	void TStaticMeshComponent::SetMaterial(const std::string& materialSlotName, FMaterialRef material)
	{
		if (mStaticMesh->GetMaterials().contains(materialSlotName))
		{
			mOverrideMaterials[materialSlotName] = material;
		}
	}

	FMaterialRef TStaticMeshComponent::GetMaterial(const std::string& name) const
	{
		if (mOverrideMaterials.contains(name))
		{
			return mOverrideMaterials.find(name)->second;
		}
		else if (mStaticMesh->GetMaterials().contains(name))
		{
			return mStaticMesh->GetMaterials().find(name)->second;
		}

		return nullptr;
	}

	EPerVertexSemantic TStaticMeshComponent::GetVertexSemanticType(const std::string& semanticName)
	{
		if (FStringUtility::Contains(semanticName, "POSITION"))
		{
			return EPerVertexSemantic::Position;
		}
		else if (FStringUtility::Contains(semanticName, "NORMAL"))
		{
			return EPerVertexSemantic::Normal;
		}
		else if (FStringUtility::Contains(semanticName, "TANGENT"))
		{
			return EPerVertexSemantic::Tangent;
		}
		else if (FStringUtility::Contains(semanticName, "COLOR"))
		{
			return EPerVertexSemantic::VertexColor;
		}
		else if (FStringUtility::Contains(semanticName, "TEXCOORD"))
		{
			return EPerVertexSemantic::TexCoord;
		}

		LOG_INFO << "Unknown Material Vertex Semantic : " << semanticName;

		return EPerVertexSemantic::Unknown;
	}

	void TStaticMeshComponent::BuildMeshDrawCommands()
	{
		mCachedMeshDrawCommands.clear();

		if (mStaticMesh == nullptr)
		{
			return;
		}

		const std::vector<FMeshSectionData>& meshSectionData = mStaticMesh->GetMeshSections();

		for (uint32_t sectionIndex = 0; sectionIndex < meshSectionData.size(); sectionIndex++)
		{
			const FMeshSectionData& sectionData = meshSectionData[sectionIndex];
			FMaterialRef material = mStaticMesh->GetMaterial(sectionData.materialSlotName);

			if (material == nullptr)
			{
				continue;
			}

			const std::map<std::string, FMaterial::FShaderPassParameter>& shaderPassParameters = material->GetShaderPassParameters();
			FShaderTechniqueRef shaderTechnique = material->GetShaderTechnique();

			if (shaderTechnique == nullptr)
			{
				continue;
			}

			const std::vector<FShaderPassRef>& shaderPasses = shaderTechnique->GetPasses();

			for (uint32_t passIndex = 0; passIndex < shaderPasses.size(); passIndex++)
			{
				FShaderPassRef shaderPass = shaderPasses[passIndex];

				FMeshDrawCommand meshDrawCommand;

				const std::vector<std::pair<uint32_t, std::string>>& perVertexSemantics = shaderPass->GetInputLayout().GetPerVertexSemantics();

				for (uint32_t semanticIndex = 0; semanticIndex < perVertexSemantics.size(); semanticIndex++)
				{	
					EPerVertexSemantic semantic = GetVertexSemanticType(perVertexSemantics[semanticIndex].second);
					if (semantic == EPerVertexSemantic::Position)
					{
						ASSERT(mStaticMesh->GetPositionBuffer() != nullptr);
						meshDrawCommand.VertexBuffers.emplace_back(mStaticMesh->GetPositionBuffer());
					}
					else if (semantic == EPerVertexSemantic::Normal)
					{
						ASSERT(mStaticMesh->GetNormalBuffer() != nullptr);
						meshDrawCommand.VertexBuffers.emplace_back(mStaticMesh->GetNormalBuffer());
					}
					else if (semantic == EPerVertexSemantic::Tangent)
					{
						ASSERT(mStaticMesh->GetTangentBuffer() != nullptr);
						meshDrawCommand.VertexBuffers.emplace_back(mStaticMesh->GetTangentBuffer());
					}
					else if (semantic == EPerVertexSemantic::VertexColor)
					{
						ASSERT(mStaticMesh->GetVertexColorBuffer() != nullptr);
						meshDrawCommand.VertexBuffers.emplace_back(mStaticMesh->GetVertexColorBuffer());
					}
					else if (semantic == EPerVertexSemantic::TexCoord)
					{
						ASSERT(mStaticMesh->GetTexCoordBuffer() != nullptr);
						meshDrawCommand.VertexBuffers.emplace_back(mStaticMesh->GetTexCoordBuffer());
					}
				}

				meshDrawCommand.IndexBuffer = mStaticMesh->GetIndexBuffer();

				auto iter = shaderPassParameters.find(shaderPass->GetPassName());
				meshDrawCommand.ConstantBufferMapPtr = &iter->second.ConstantBufferMap;
				meshDrawCommand.TextureBufferMapPtr = &iter->second.TextureBufferMap;

				meshDrawCommand.PSO = FGraphicsPSO::MakeGraphicsPSO(mName);
				meshDrawCommand.PSO->SetShaderPass(shaderPass);
				meshDrawCommand.PSO->SetPrimitiveTopologyType(EPrimitiveTopology::TriangleList);
				meshDrawCommand.PSO->SetSamplerMask(UINT_MAX);
				meshDrawCommand.PSO->SetRenderTargetFormat(FGraphicsCore::SwapChain->GetBackBufferFormat(), FGraphicsCore::SwapChain->GetDepthBuffer()->GetFormat());
				meshDrawCommand.PSO->Finalize();

				meshDrawCommand.vertexStart = sectionData.vertexStart;
				meshDrawCommand.vertexCount = sectionData.vertexCount;
				meshDrawCommand.indexStart = sectionData.indexStart;
				meshDrawCommand.indexCount = sectionData.indexCount;

				mCachedMeshDrawCommands.emplace_back(meshDrawCommand);
			}
		}
	}
}