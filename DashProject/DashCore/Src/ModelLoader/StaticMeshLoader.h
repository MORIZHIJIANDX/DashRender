#pragma once

#include "MeshLoaderHelper.h"

namespace Dash
{
	bool LoadStaticMeshFromFile(const std::string filePath, FImportedStaticMeshData& importedMeshData);
}