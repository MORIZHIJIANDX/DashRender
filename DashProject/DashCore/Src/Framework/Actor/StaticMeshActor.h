#pragma once

#include "Actor.h"

namespace Dash
{
	class TStaticMeshComponent;

	class TStaticMeshActor : public TActor
	{
	public:
		TStaticMeshActor(const std::string& name, const std::string meshPath = "");
		~TStaticMeshActor();

		TStaticMeshComponent* GetStaticMeshComponent() const { return mStaticMeshComponent; }

	private:
		TStaticMeshComponent* mStaticMeshComponent;
	};
}