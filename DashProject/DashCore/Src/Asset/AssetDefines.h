#pragma once

namespace Dash
{
	class FMaterial;
	using FMaterialRef = std::shared_ptr<FMaterial>;

	class FStaticMesh;
	using FStaticMeshRef = std::shared_ptr<FStaticMesh>;

	class FTexture;
	using FTextureRef = std::shared_ptr<FTexture>;
}