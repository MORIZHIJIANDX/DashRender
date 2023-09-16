#pragma once

#include "Actor.h"
#include "Graphics/Viewport.h"
#include "Framework/Component/CameraComponent.h"

namespace Dash
{
	class TCameraActor : public TActor
	{
	public:
		TCameraActor(const std::string& name, ECameraType cameraType);
		virtual ~TCameraActor();
		
		TCameraComponent* GetCameraComponent() const { return mCameraComponent; }

	private:

		TCameraComponent* mCameraComponent;
	};

}