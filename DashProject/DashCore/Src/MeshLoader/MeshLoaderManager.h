#pragma once

#include "StaticMeshLoader.h"

namespace Dash
{
	struct FImportedMeshData : public FImportedStaticMeshData
	{
		friend class FMeshLoaderManager;

		std::string SourceMeshPath;

	private:

		int32_t AddRef()
		{
			return ++RefCount;
		}

		int32_t Release()
		{
			return --RefCount;
		}

	private:

		int32_t RefCount = 0;
	};

	class FMeshLoaderManager
	{
	public:
		static FMeshLoaderManager& Get();

		void Init();
		void Shutdown();

		const FImportedMeshData& LoadMesh(const std::string& meshPath);

		bool UnloadMesh(const std::string& meshPath);

	private:
		void CreateDefaultMeshs();
		FImportedMeshData CreateCube(Scalar width, Scalar height, Scalar depth, FVector4f color);

	private:
		std::map<std::string, FImportedMeshData> mImportMeshs;
	};
}