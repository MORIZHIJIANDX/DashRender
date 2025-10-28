#pragma once

#include "AssetDefines.h"
#include "Graphics/ShaderTechnique.h"

namespace Dash
{
	class FAssetManager
	{
	public:
		static FAssetManager& Get();

		void Init();
		void Shutdown(); 

		FMaterialRef MakeMaterial(const std::string& name, const FShaderTechniqueRef& shaderTechnique);

		FStaticMeshRef MakeStaticMesh(const std::string& texturePath);

		FTextureRef MakeTexture(const std::string& texturePath);

	private:
		std::map<std::string, std::weak_ptr<FMaterial>> mMaterialResourceMap;
		std::map<std::string, std::weak_ptr<FStaticMesh>> mStaticMeshResourceMap;
		std::map<std::string, std::weak_ptr<FTexture>> mTextureResourceMap;
	};
}