#include "PCH.h"
#include "StaticMeshActor.h"
#include "Framework/Component/StaticMeshComponent.h"

namespace Dash
{
	TStaticMeshActor::TStaticMeshActor(const std::string& name, const std::string meshPath)
		: TActor(name)
		, mStaticMeshComponent(nullptr)
	{
		mStaticMeshComponent = AddComponent<TStaticMeshComponent>("StaticMeshComponent");

		if (!meshPath.empty())
		{
			mStaticMeshComponent->SetStaticMesh(FStaticMesh::MakeStaticMesh(meshPath));
		}
	}

	TStaticMeshActor::~TStaticMeshActor()
	{
	}
}