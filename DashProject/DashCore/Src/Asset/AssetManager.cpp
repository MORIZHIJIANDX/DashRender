#include "PCH.h"
#include "AssetManager.h"
#include "Texture.h"
#include "Material.h"
#include "StaticMesh.h"

namespace Dash
{
	FAssetManager& FAssetManager::Get()
	{
		static FAssetManager manager;
		return manager;
	}

	void FAssetManager::Init()
	{
	}

	void FAssetManager::Shutdown()
	{
		mStaticMeshResourceMap.clear();
		mMaterialResourceMap.clear();
		mTextureResourceMap.clear();
	}

	FMaterialRef FAssetManager::MakeMaterial(const std::string& name, const FShaderTechniqueRef& shaderTechnique)
	{
		FMaterialRef material = nullptr;

		if (!mMaterialResourceMap.contains(name) || !mMaterialResourceMap[name].lock())
		{
			material = std::make_shared<FMaterial>(name, shaderTechnique);
			mMaterialResourceMap[name] = material;
		}
		else
		{
			material = mMaterialResourceMap[name].lock();
		}

		return material;
	}

	FStaticMeshRef FAssetManager::MakeStaticMesh(const std::string& meshPath)
	{
		FStaticMeshRef staticMesh = nullptr;

		if (!mStaticMeshResourceMap.contains(meshPath) || !mStaticMeshResourceMap[meshPath].lock())
		{
			staticMesh = std::make_shared<FStaticMesh>(meshPath);
			mStaticMeshResourceMap[meshPath] = staticMesh;
		}
		else
		{
			staticMesh = mStaticMeshResourceMap[meshPath].lock();
		}

		return staticMesh;
	}

	FTextureRef FAssetManager::MakeTexture(const std::string& texturePath)
	{ 
		FTextureRef texture = nullptr;

		if (!mTextureResourceMap.contains(texturePath) || !mTextureResourceMap[texturePath].lock())
		{
			texture = std::make_shared<FTexture>(texturePath);
			mTextureResourceMap[texturePath] = texture;
		}
		else
		{
			texture = mTextureResourceMap[texturePath].lock();
		}

		return texture;
	}
}