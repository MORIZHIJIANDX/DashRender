#include "PCH.h"
#include "CameraActor.h"

namespace Dash
{
	TCameraActor::TCameraActor(const std::string& name, ECameraType cameraType)
		: TActor(name)
		, mCameraComponent(nullptr)
	{
		switch (cameraType)
		{
		case ECameraType::Orthographic:
			mCameraComponent = AddComponent<TOrthographicCameraComponent>("OrthographicCameraComponent");
			break;
		case ECameraType::Perspective:
			mCameraComponent = AddComponent<TPerspectiveCameraComponent>("PerspectiveCameraComponent");
			break;
		case ECameraType::FirstPerson:
			mCameraComponent = AddComponent<TFirstPersonCameraComponent>("FirstPersonCameraComponent");
			break;
		default:
			break;
		}
	}

	TCameraActor::~TCameraActor()
	{
	}
}