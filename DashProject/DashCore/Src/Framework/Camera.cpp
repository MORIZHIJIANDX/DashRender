#include "PCH.h"

#include "Camera.h"

namespace Dash
{
	FCamera::FCamera(Scalar nearZ, Scalar farZ, const FViewport& vp)
		: mNear(nearZ)
		, mFar(farZ)
		, mViewPort(vp)
		, mProjectionMatrix(FIdentity{})
		, mViewProjectionMatrix(FIdentity{})
		, mTransform(FIdentity{})
		, mWorldMatrixDirty(true)
		, mProjectionMatrixDirty(true)
	{

	}

	FCamera::~FCamera()
	{
	}

	FMatrix4x4 FCamera::GetViewMatrix() const
	{
		return mTransform.GetInverseMatrix();
	}

	FMatrix4x4 FCamera::GetProjectionMatrix() const
	{
		if (mProjectionMatrixDirty)
		{
			UpdateProjectionMatrix();
		}

		return mProjectionMatrix;
	}

	FMatrix4x4 FCamera::GetViewProjectionMatrix() const
	{
		if (mProjectionMatrixDirty)
		{
			UpdateProjectionMatrix();
		}

		if (mWorldMatrixDirty)
		{
			UpdateViewProjectionMatrix();
		}

		return mViewProjectionMatrix;
	}

	FVector3f FCamera::GetPosition() const
	{
		return mTransform.GetPosition();
	}

	FQuaternion FCamera::GetRotation() const
	{
		return mTransform.GetRotation();
	}

	FVector3f FCamera::GetForward() const
	{
		return mTransform.GetUnitForwardAxis();
	}

	FVector3f FCamera::GetRight() const
	{
		return mTransform.GetUnitRightAxis();
	}

	FVector3f FCamera::GetUp() const
	{
		return mTransform.GetUnitUpAxis();
	}

	Scalar FCamera::GetFar() const
	{
		return mFar;
	}

	Scalar FCamera::GetNear() const
	{
		return mNear;
	}

	FViewport FCamera::GetViewPort() const
	{
		return mViewPort;
	}

	std::size_t FCamera::GetPixelWidth() const
	{
		return static_cast<std::size_t>(mViewPort.Width);
	}

	std::size_t FCamera::GetPixelHeight() const
	{
		return static_cast<std::size_t>(mViewPort.Height);
	}

	void FCamera::SetWorldMatrix(const FMatrix4x4& mat)
	{
		mTransform = FTransform{ mat };
	
		MakeWorldMatrixDirty();
	}

	void FCamera::SetProjectionMatrix(const FMatrix4x4& mat)
	{
		mProjectionMatrix = mat;
		MakeProjectionMatrixDirty();
	}

	void FCamera::SetPosition(const FVector3f& p)
	{
		mTransform.SetPosition(p);
		MakeWorldMatrixDirty();
	}

	void FCamera::SetRotation(const FQuaternion& q)
	{
		mTransform.SetRotation(q);
		MakeWorldMatrixDirty();
	}

	void FCamera::SetRotation(Scalar pitch, Scalar yaw, Scalar roll)
	{
		mTransform.SetEuler(pitch, yaw, roll);
		MakeWorldMatrixDirty();
	}

	void FCamera::SetFarClip(Scalar farZ)
	{
		mFar = farZ;
		MakeProjectionMatrixDirty();
	}

	void FCamera::SetNearClip(Scalar nearZ)
	{
		mNear = nearZ;
		MakeProjectionMatrixDirty();
	}

	void FCamera::SetViewPort(const FViewport& vp)
	{
		mViewPort = vp;
	}

	void FCamera::SetLookAt(const FVector3f& eye, const FVector3f& lookAt, const FVector3f& up)
	{
		SetLookTo(eye, lookAt - eye, up);
	}

	void FCamera::SetLookTo(const FVector3f& eye, const FVector3f& lookTo, const FVector3f& up)
	{
		mTransform.SetLookTo(eye, lookTo, up);
		MakeWorldMatrixDirty();
	}

	void FCamera::TranslateForward(Scalar speed)
	{
		mTransform.SetPosition(mTransform.GetPosition() + speed * mTransform.GetUnitForwardAxis());
		MakeWorldMatrixDirty();
	}

	void FCamera::TranslateRight(Scalar speed)
	{
		mTransform.SetPosition(mTransform.GetPosition() + speed * mTransform.GetUnitRightAxis());
		MakeWorldMatrixDirty();
	}

	void FCamera::TranslateUp(Scalar speed)
	{
		mTransform.SetPosition(mTransform.GetPosition() + speed * mTransform.GetUnitUpAxis());
		MakeWorldMatrixDirty();
	}

	void FCamera::TranslateBack(Scalar speed)
	{
		mTransform.SetPosition(mTransform.GetPosition() - speed * mTransform.GetUnitForwardAxis());
		MakeWorldMatrixDirty();
	}

	void FCamera::TranslateLeft(Scalar speed)
	{
		mTransform.SetPosition(mTransform.GetPosition() - speed * mTransform.GetUnitRightAxis());
		MakeWorldMatrixDirty();
	}

	void FCamera::TranslateDown(Scalar speed)
	{
		mTransform.SetPosition(mTransform.GetPosition() - speed * mTransform.GetUnitUpAxis());
		MakeWorldMatrixDirty();
	}

	void FCamera::AddPitch(Scalar angle)
	{
		//FQuaternion rotation = FMath::FromAxisAngle(FMath::Mul(mTransform.GetRotation(), FVector3f(FUnit<0>{})), FMath::Radians(angle)) * mTransform.GetRotation();

		FVector3f OriginEuler;
		FMath::ToEuler(OriginEuler, mTransform.GetRotation());
		OriginEuler.X += FMath::Radians(angle);
		FQuaternion Rotation = FMath::FromEuler(OriginEuler);
		
		mTransform.SetRotation(Rotation);

		MakeWorldMatrixDirty();
	}

	void FCamera::AddYaw(Scalar angle)
	{
		//FQuaternion rotation = FMath::FromAxisAngle(FMath::Mul(mTransform.GetRotation(), FVector3f(FUnit<1>{})), FMath::Radians(angle))* mTransform.GetRotation();

		FVector3f OriginEuler;
		FMath::ToEuler(OriginEuler, mTransform.GetRotation());
		OriginEuler.Y += FMath::Radians(angle);
		FQuaternion Rotation = FMath::FromEuler(OriginEuler);

		mTransform.SetRotation(Rotation);

		MakeWorldMatrixDirty();
	}

	void FCamera::AddRoll(Scalar angle)
	{
		FQuaternion rotation = FMath::FromAxisAngle(FMath::Mul(mTransform.GetRotation(), FVector3f(FUnit<2>{})), FMath::Radians(angle))* mTransform.GetRotation();
		mTransform.SetRotation(rotation);
		MakeWorldMatrixDirty();
	}




	//FOrthographicCamera

	FOrthographicCamera::FOrthographicCamera()
		: FCamera(1.0f, 1000.0f, FViewport{})
		, mXMin(-1.0f)
		, mXMax(1.0f)
		, mYMin(-1.0f)
		, mYMax(1.0f)
	{
		UpdateProjectionMatrix();
	}

	FOrthographicCamera::FOrthographicCamera(Scalar minX, Scalar maxX, Scalar minY, Scalar maxY, Scalar nearZ, Scalar farZ, const FViewport& vp)
		: FCamera(nearZ, farZ, vp)
		, mXMin(minX)
		, mXMax(maxX)
		, mYMin(minY)
		, mYMax(maxY)
	{
		UpdateProjectionMatrix();
	}

	FOrthographicCamera::~FOrthographicCamera()
	{
	}

	void FOrthographicCamera::SetMinX(Scalar minX)
	{
		mXMin = minX;
		MakeProjectionMatrixDirty();
	}

	void FOrthographicCamera::SetMinY(Scalar minY)
	{
		mYMin = minY;
		MakeProjectionMatrixDirty();
	}

	void FOrthographicCamera::SetMaxX(Scalar maxX)
	{
		mXMax = maxX;
		MakeProjectionMatrixDirty();
	}

	void FOrthographicCamera::SetMaxY(Scalar maxY)
	{
		mYMax = maxY;
		MakeProjectionMatrixDirty();
	}

	void FOrthographicCamera::SetCameraParams(Scalar minX, Scalar maxX, Scalar minY,  Scalar maxY, Scalar nearZ, Scalar farZ)
	{
		mXMin = minX;
		mXMax = maxX;

		mYMin = minY;
		mYMax = maxY;

		mNear = nearZ;
		mFar = farZ;

		MakeProjectionMatrixDirty();
	}

	void FOrthographicCamera::SetCameraParams(Scalar width, Scalar height, Scalar nearZ, Scalar farZ)
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

	void FOrthographicCamera::Zoom(Scalar factor)
	{
		factor = FMath::Max(factor, Scalar{ 0.01f });

		Scalar centerX = (mXMin + mXMax) * Scalar(0.5f);
		Scalar centerY = (mYMin + mYMax) * Scalar(0.5f);

		Scalar halfWidth = (mXMax - mXMin) * Scalar(0.5f);
		Scalar halfHeight = (mYMax - mYMin) * Scalar(0.5f);

		if (factor < Scalar{ 1 } && halfWidth < Scalar{ 0.005 } && halfHeight < Scalar{ 0.005 })
		{
			return;
		}

		Scalar zoomedHalfWidth = halfWidth * factor;
		Scalar zoomedHalfHeight = halfHeight * factor;
		SetCameraParams(centerX - zoomedHalfWidth, centerX + zoomedHalfWidth, centerY - zoomedHalfHeight, centerY + zoomedHalfHeight, mNear, mFar);
	}

	void FOrthographicCamera::CreateProjectionMatrix() const
	{
		mProjectionMatrix = FMath::Orthographic(mXMin, mXMax, mYMin, mYMax, mNear, mFar);
	}




	//FPerspectiveCamera

	FPerspectiveCamera::FPerspectiveCamera()
		: FCamera(1.0f, 100.0f, FViewport{})
		, mAspect(16.0f / 9.0f)
		, mFov(TScalarTraits<Scalar>::Pi()* Scalar { 0.25 })
	{
		UpdateProjectionMatrix();
	}

	FPerspectiveCamera::FPerspectiveCamera(Scalar aspect, Scalar fov, Scalar nearZ, Scalar farZ, const FViewport& vp)
		: FCamera(nearZ, farZ, vp)
		, mAspect(aspect)
		, mFov(fov)
	{
		UpdateProjectionMatrix();
	}

	FPerspectiveCamera::~FPerspectiveCamera()
	{
	}

	void FPerspectiveCamera::SetFieldOfView(Scalar fov)
	{
		mFov = fov;
		MakeProjectionMatrixDirty();
	}

	void FPerspectiveCamera::SetAspectRatio(Scalar aspect)
	{
		mAspect = aspect;
		MakeProjectionMatrixDirty();
	}

	FRay FPerspectiveCamera::GenerateRay(Scalar u, Scalar v) const
	{
		FVector3f camPos = GetPosition();
		FVector3f forward = GetForward();
		FVector3f right = GetRight();
		FVector3f up = GetUp();
		
		Scalar nearPlaneHeight = mNear * FMath::Tan(FMath::Radians(mFov) * Scalar{0.5});
		Scalar nearPlaneWidth = nearPlaneHeight * mAspect;

		const FVector3f horizon = right * nearPlaneWidth;
		const FVector3f vertical = up * nearPlaneHeight;

		const FVector3f topRightCorner = camPos + forward * mNear - horizon * 0.5f + vertical * 0.5f;

		return FRay{ camPos, FMath::Normalize(topRightCorner + u * horizon - v * vertical - camPos), 0.0f, 1000.0f };
	}

	void FPerspectiveCamera::SetCameraParams(Scalar aspect, Scalar fov, Scalar nearZ, Scalar farZ)
	{
		mAspect = aspect;
		mFov = fov;	
		mNear = nearZ;
		mFar = farZ;

		MakeProjectionMatrixDirty();
	}

	void FPerspectiveCamera::Zoom(Scalar factor)
	{
		factor = FMath::Max(factor, Scalar{ 0.01f });

		mFov *= factor;

		mFov = FMath::Clamp(mFov, Scalar{ 0.01f }, Scalar{179.99f});

		MakeProjectionMatrixDirty();
	}

	void FPerspectiveCamera::CreateProjectionMatrix() const
	{
		mProjectionMatrix = FMath::Frustum(mFov, mAspect, mNear, mFar);
	}



	// FirstPersonCamera

	FirstPersonCamera::FirstPersonCamera(Scalar aspect, Scalar fov, Scalar nearZ, Scalar farZ, const FViewport& vp)
		: FPerspectiveCamera(aspect, fov, nearZ, farZ, vp)
		, mXRot(0)
		, mYRot(0)
	{
	}

	void FirstPersonCamera::AddXAxisRotation(Scalar angle)
	{
		mXRot += FMath::Radians(angle);
		mXRot = FMath::Clamp(mXRot, -TScalarTraits<Scalar>::HalfPi(), TScalarTraits<Scalar>::HalfPi());
		SetRotation(FMath::FromEuler(mXRot, mYRot, Scalar{0}));
	}

	void FirstPersonCamera::AddYAxisRotation(Scalar angle)
	{
		mYRot += FMath::Radians(angle);
		mYRot = FMath::ModAngle(mYRot);
		SetRotation(FMath::FromEuler(mXRot, mYRot, Scalar{ 0 }));
	}

}
