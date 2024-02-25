#include "PCH.h"
#include "CameraComponent.h"

namespace Dash
{
	TCameraComponent::TCameraComponent(const std::string& name, TActor* owner, Scalar nearZ, Scalar farZ, const FViewport& vp)
		: TComponent(name, owner)
		, mProjectionMatrix(FIdentity{})
		, mViewProjectionMatrix(FIdentity{})
		, mNear(nearZ)
		, mFar(farZ)
		, mViewPort(vp)
		, mProjectionMatrixDirty(true)
	{
	}

	TCameraComponent::~TCameraComponent()
	{
	}

	FMatrix4x4 TCameraComponent::GetViewMatrix() const
	{
		return mComponentToWorld.GetInverseMatrix();
	}

	FMatrix4x4 TCameraComponent::GetProjectionMatrix() const
	{
		if (mProjectionMatrixDirty)
		{
			UpdateProjectionMatrix();
		}

		return mProjectionMatrix;
	}

	FMatrix4x4 TCameraComponent::GetViewProjectionMatrix() const
	{
		if (mProjectionMatrixDirty)
		{
			UpdateProjectionMatrix();
		}

		UpdateViewProjectionMatrix();

		return mViewProjectionMatrix;
	}

	FVector3f TCameraComponent::GetForward() const
	{
		return mComponentToWorld.GetUnitForwardAxis();
	}

	FVector3f TCameraComponent::GetRight() const
	{
		return mComponentToWorld.GetUnitRightAxis();
	}

	FVector3f TCameraComponent::GetUp() const
	{
		return mComponentToWorld.GetUnitUpAxis();
	}

	Scalar TCameraComponent::GetFar() const
	{
		return mFar;
	}

	Scalar TCameraComponent::GetNear() const
	{
		return mNear;
	}

	FViewport TCameraComponent::GetViewPort() const
	{
		return mViewPort;
	}

	std::size_t TCameraComponent::GetPixelWidth() const
	{
		return static_cast<std::size_t>(mViewPort.Width);
	}

	std::size_t TCameraComponent::GetPixelHeight() const
	{
		return static_cast<std::size_t>(mViewPort.Height);
	}

	void TCameraComponent::SetWorldMatrix(const FMatrix4x4& mat)
	{
		mComponentToWorld = FTransform{ mat };
	}

	void TCameraComponent::SetProjectionMatrix(const FMatrix4x4& mat)
	{
		mProjectionMatrix = mat;
		MakeProjectionMatrixDirty();
	}

	void TCameraComponent::SetFarClip(Scalar farZ)
	{
		mFar = farZ;
		MakeProjectionMatrixDirty();
	}

	void TCameraComponent::SetNearClip(Scalar nearZ)
	{
		mNear = nearZ;
		MakeProjectionMatrixDirty();
	}

	void TCameraComponent::SetViewPort(const FViewport& vp)
	{
		mViewPort = vp;
	}

	void TCameraComponent::SetLookAt(const FVector3f& eye, const FVector3f& lookAt, const FVector3f& up)
	{
		SetLookTo(eye, lookAt - eye, up);
	}

	void TCameraComponent::SetLookTo(const FVector3f& eye, const FVector3f& lookTo, const FVector3f& up)
	{
		mComponentToWorld.SetLookTo(eye, lookTo, up);
	}

	void TCameraComponent::TranslateForward(Scalar speed)
	{
		mComponentToWorld.SetPosition(mComponentToWorld.GetPosition() + speed * mComponentToWorld.GetUnitForwardAxis());
	}

	void TCameraComponent::TranslateRight(Scalar speed)
	{
		mComponentToWorld.SetPosition(mComponentToWorld.GetPosition() + speed * mComponentToWorld.GetUnitRightAxis());
	}

	void TCameraComponent::TranslateUp(Scalar speed)
	{
		mComponentToWorld.SetPosition(mComponentToWorld.GetPosition() + speed * mComponentToWorld.GetUnitUpAxis());
	}

	void TCameraComponent::TranslateBack(Scalar speed)
	{
		mComponentToWorld.SetPosition(mComponentToWorld.GetPosition() - speed * mComponentToWorld.GetUnitForwardAxis());
	}

	void TCameraComponent::TranslateLeft(Scalar speed)
	{
		mComponentToWorld.SetPosition(mComponentToWorld.GetPosition() - speed * mComponentToWorld.GetUnitRightAxis());
	}

	void TCameraComponent::TranslateDown(Scalar speed)
	{
		mComponentToWorld.SetPosition(mComponentToWorld.GetPosition() - speed * mComponentToWorld.GetUnitUpAxis());
	}

	void TCameraComponent::AddPitch(Scalar angle)
	{
		FVector3f OriginEuler;
		FMath::ToEuler(OriginEuler, mComponentToWorld.GetRotation());
		OriginEuler.X += FMath::Radians(angle);
		FQuaternion Rotation = FMath::FromEuler(OriginEuler);

		mComponentToWorld.SetRotation(Rotation);
	}

	void TCameraComponent::AddYaw(Scalar angle)
	{
		FVector3f OriginEuler;
		FMath::ToEuler(OriginEuler, mComponentToWorld.GetRotation());
		OriginEuler.Y += FMath::Radians(angle);
		FQuaternion Rotation = FMath::FromEuler(OriginEuler);

		mComponentToWorld.SetRotation(Rotation);
	}

	void TCameraComponent::AddRoll(Scalar angle)
	{
		FQuaternion rotation = FMath::FromAxisAngle(FMath::Mul(mComponentToWorld.GetRotation(), FVector3f(FUnit<2>{})), FMath::Radians(angle))* mComponentToWorld.GetRotation();
		mComponentToWorld.SetRotation(rotation);
	}




	//TOrthographicCameraComponent

	TOrthographicCameraComponent::TOrthographicCameraComponent(const std::string& name, TActor* owner)
		: TCameraComponent(name, owner, 1.0f, 1000.0f, FViewport{})
		, mXMin(-1.0f)
		, mXMax(1.0f)
		, mYMin(-1.0f)
		, mYMax(1.0f)
	{
		UpdateProjectionMatrix();
	}

	TOrthographicCameraComponent::TOrthographicCameraComponent(const std::string& name, TActor* owner, Scalar minX, Scalar maxX, Scalar minY, Scalar maxY, Scalar nearZ, Scalar farZ, const FViewport& vp)
		: TCameraComponent(name, owner, nearZ, farZ, vp)
		, mXMin(minX)
		, mXMax(maxX)
		, mYMin(minY)
		, mYMax(maxY)
	{
		UpdateProjectionMatrix();
	}

	TOrthographicCameraComponent::~TOrthographicCameraComponent()
	{
	}

	void TOrthographicCameraComponent::SetMinX(Scalar minX)
	{
		mXMin = minX;
		MakeProjectionMatrixDirty();
	}

	void TOrthographicCameraComponent::SetMinY(Scalar minY)
	{
		mYMin = minY;
		MakeProjectionMatrixDirty();
	}

	void TOrthographicCameraComponent::SetMaxX(Scalar maxX)
	{
		mXMax = maxX;
		MakeProjectionMatrixDirty();
	}

	void TOrthographicCameraComponent::SetMaxY(Scalar maxY)
	{
		mYMax = maxY;
		MakeProjectionMatrixDirty();
	}

	void TOrthographicCameraComponent::SetCameraParams(Scalar minX, Scalar maxX, Scalar minY, Scalar maxY, Scalar nearZ, Scalar farZ)
	{
		mXMin = minX;
		mXMax = maxX;

		mYMin = minY;
		mYMax = maxY;

		mNear = nearZ;
		mFar = farZ;

		MakeProjectionMatrixDirty();
	}

	void TOrthographicCameraComponent::SetCameraParams(Scalar width, Scalar height, Scalar nearZ, Scalar farZ)
	{
		Scalar halfWidth = 0.5f * width;
		Scalar halfHeight = 0.5F * height;

		mXMin = -halfWidth;
		mXMax = halfWidth;

		mYMin = -halfHeight;
		mYMax = halfHeight;

		mNear = nearZ;
		mFar = farZ;

		MakeProjectionMatrixDirty();
	}

	void TOrthographicCameraComponent::Zoom(Scalar factor)
	{
		factor = FMath::Max(factor, Scalar{ 0.01f });

		Scalar centerX = (mXMin + mXMax) * Scalar(0.5f);
		Scalar centerY = (mYMin + mYMax) * Scalar(0.5f);

		Scalar halfWidth = (mXMax - mXMin) * Scalar(0.5f);
		Scalar halfHeight = (mYMax - mYMin) * Scalar(0.5f);

		if (factor < Scalar{ 1 }&& halfWidth < Scalar{ 0.005 }&& halfHeight < Scalar{ 0.005 })
		{
			return;
		}

		Scalar zoomedHalfWidth = halfWidth * factor;
		Scalar zoomedHalfHeight = halfHeight * factor;
		SetCameraParams(centerX - zoomedHalfWidth, centerX + zoomedHalfWidth, centerY - zoomedHalfHeight, centerY + zoomedHalfHeight, mNear, mFar);
	}

	void TOrthographicCameraComponent::CreateProjectionMatrix() const
	{
		mProjectionMatrix = FMath::Orthographic(mXMin, mXMax, mYMin, mYMax, mNear, mFar);
	}




	//TPerspectiveCameraComponent

	TPerspectiveCameraComponent::TPerspectiveCameraComponent(const std::string& name, TActor* owner)
		: TCameraComponent(name, owner, 1.0f, 100.0f, FViewport{})
		, mFov(TScalarTraits<Scalar>::Pi()* Scalar { 0.25 })
		, mAspect(16.0f / 9.0f)
	{
		UpdateProjectionMatrix();
	}

	TPerspectiveCameraComponent::TPerspectiveCameraComponent(const std::string& name, TActor* owner, Scalar aspect, Scalar fov, Scalar nearZ, Scalar farZ, const FViewport& vp)
		: TCameraComponent(name, owner, nearZ, farZ, vp)
		, mFov(fov)
		, mAspect(aspect)
	{
		UpdateProjectionMatrix();
	}

	TPerspectiveCameraComponent::~TPerspectiveCameraComponent()
	{
	}

	void TPerspectiveCameraComponent::SetFieldOfView(Scalar fov)
	{
		mFov = fov;
		MakeProjectionMatrixDirty();
	}

	void TPerspectiveCameraComponent::SetAspectRatio(Scalar aspect)
	{
		mAspect = aspect;
		MakeProjectionMatrixDirty();
	}

	FRay TPerspectiveCameraComponent::GenerateRay(Scalar u, Scalar v) const
	{
		FVector3f camPos = GetWorldPosition();
		FVector3f forward = GetForward();
		FVector3f right = GetRight();
		FVector3f up = GetUp();

		Scalar nearPlaneHeight = mNear * FMath::Tan(FMath::Radians(mFov) * Scalar { 0.5 });
		Scalar nearPlaneWidth = nearPlaneHeight * mAspect;

		const FVector3f horizon = right * nearPlaneWidth;
		const FVector3f vertical = up * nearPlaneHeight;

		const FVector3f topRightCorner = camPos + forward * mNear - horizon * 0.5f + vertical * 0.5f;

		return FRay{ camPos, FMath::Normalize(topRightCorner + u * horizon - v * vertical - camPos), 0.0f, 1000.0f };
	}

	void TPerspectiveCameraComponent::SetCameraParams(Scalar aspect, Scalar fov, Scalar nearZ, Scalar farZ)
	{
		mAspect = aspect;
		mFov = fov;
		mNear = nearZ;
		mFar = farZ;

		MakeProjectionMatrixDirty();
	}

	void TPerspectiveCameraComponent::Zoom(Scalar factor)
	{
		factor = FMath::Max(factor, Scalar{ 0.01f });

		mFov *= factor;

		mFov = FMath::Clamp(mFov, Scalar{ 0.01f }, Scalar{ 179.99f });

		MakeProjectionMatrixDirty();
	}

	void TPerspectiveCameraComponent::CreateProjectionMatrix() const
	{
		mProjectionMatrix = FMath::Frustum(mFov, mAspect, mNear, mFar);
	}



	// TFirstPersonCameraComponent

	TFirstPersonCameraComponent::TFirstPersonCameraComponent(const std::string& name, TActor* owner, Scalar aspect, Scalar fov, Scalar nearZ, Scalar farZ, const FViewport& vp)
		: TPerspectiveCameraComponent(name, owner, aspect, fov, nearZ, farZ, vp)
		, mXRot(0)
		, mYRot(0)
	{
	}

	void TFirstPersonCameraComponent::AddXAxisRotation(Scalar angle)
	{
		mXRot += FMath::Radians(angle);
		mXRot = FMath::Clamp(mXRot, -TScalarTraits<Scalar>::HalfPi(), TScalarTraits<Scalar>::HalfPi());
		SetWorldRotation(FMath::FromEuler(mXRot, mYRot, Scalar{ 0 }));
	}

	void TFirstPersonCameraComponent::AddYAxisRotation(Scalar angle)
	{
		mYRot += FMath::Radians(angle);
		mYRot = FMath::ModAngle(mYRot);
		SetWorldRotation(FMath::FromEuler(mXRot, mYRot, Scalar{ 0 }));
	}
}