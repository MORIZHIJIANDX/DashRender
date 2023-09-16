#pragma once

#include "Graphics/Viewport.h"
#include "Component.h"

namespace Dash
{
	enum class ECameraType
	{
		Orthographic,
		Perspective,
		FirstPerson,
	};

	class TCameraComponent : public TComponent
	{
	public:
		TCameraComponent(const std::string& name, TActor* owner, Scalar nearZ = 1.0f, Scalar farZ = 1000.0f, const FViewport& vp = FViewport{});
		virtual ~TCameraComponent();

		FMatrix4x4 GetViewMatrix() const;
		FMatrix4x4 GetProjectionMatrix() const;
		FMatrix4x4 GetViewProjectionMatrix() const;

		FVector3f GetForward() const;
		FVector3f GetRight() const;
		FVector3f GetUp() const;

		Scalar GetFar() const;
		Scalar GetNear() const;

		FViewport GetViewPort() const;

		std::size_t GetPixelWidth() const;
		std::size_t GetPixelHeight() const;

		void SetWorldMatrix(const FMatrix4x4& mat);
		void SetProjectionMatrix(const FMatrix4x4& mat);

		void SetFarClip(Scalar farZ);
		void SetNearClip(Scalar nearZ);

		void SetViewPort(const FViewport& vp);

		void SetLookAt(const FVector3f& eye, const FVector3f& lookAt, const FVector3f& up);
		void SetLookTo(const FVector3f& eye, const FVector3f& lookTo, const FVector3f& up);

		void TranslateForward(Scalar speed);
		void TranslateRight(Scalar speed);
		void TranslateUp(Scalar speed);
		void TranslateBack(Scalar speed);
		void TranslateLeft(Scalar speed);
		void TranslateDown(Scalar speed);

		// x axis
		void AddPitch(Scalar angle);
		// y axis
		void AddYaw(Scalar angle);
		// z axis
		void AddRoll(Scalar angle);

		virtual void Zoom(Scalar factor = Scalar{ 1.0f }) = 0;
		void ZoomIn() { Zoom(Scalar{ 0.9f }); }
		void ZoomOut() { Zoom(Scalar{ 1.1f }); }

	protected:

		void MakeProjectionMatrixDirty() const { mProjectionMatrixDirty = true; }

		void UpdateViewProjectionMatrix() const
		{
			mViewProjectionMatrix = mComponentToWorld.GetInverseMatrix() * mProjectionMatrix;
		}

		void UpdateProjectionMatrix() const
		{
			CreateProjectionMatrix();
			mProjectionMatrixDirty = false;
		}

		virtual void CreateProjectionMatrix() const = 0;

		mutable FMatrix4x4 mProjectionMatrix;
		mutable FMatrix4x4 mViewProjectionMatrix;

		Scalar mNear;
		Scalar mFar;

		FViewport mViewPort;

		mutable bool mProjectionMatrixDirty;
	};


	class TOrthographicCameraComponent : public TCameraComponent
	{
	public:
		TOrthographicCameraComponent(const std::string& name, TActor* owner);
		TOrthographicCameraComponent(const std::string& name, TActor* owner, Scalar minX, Scalar minY, Scalar maxX, Scalar maxY, Scalar nearZ, Scalar farZ, const FViewport& vp = FViewport{});
		virtual ~TOrthographicCameraComponent();

		Scalar GetMinX() const { return mXMin; }
		Scalar GetMinY() const { return mYMin; }
		Scalar GetMaxX() const { return mXMax; }
		Scalar GetMaxY() const { return mYMax; }

		Scalar GetAspectRatio() const { return (mXMax - mXMin) / (mYMax - mYMin); }

		void SetMinX(Scalar minX);
		void SetMinY(Scalar minY);
		void SetMaxX(Scalar maxX);
		void SetMaxY(Scalar maxY);

		void SetCameraParams(Scalar minX, Scalar minY, Scalar maxX, Scalar maxY, Scalar nearZ, Scalar farZ);
		void SetCameraParams(Scalar width, Scalar height, Scalar nearZ, Scalar farZ);

		void Zoom(Scalar factor = Scalar{ 1.0f }) override;

	protected:

		virtual void CreateProjectionMatrix() const override;

		Scalar mXMin;
		Scalar mXMax;
		Scalar mYMin;
		Scalar mYMax;
	};

	class TPerspectiveCameraComponent : public TCameraComponent
	{
	public:
		TPerspectiveCameraComponent(const std::string& name, TActor* owner);
		TPerspectiveCameraComponent(const std::string& name, TActor* owner, Scalar aspect, Scalar fov, Scalar nearZ, Scalar farZ, const FViewport& vp = FViewport{});
		virtual ~TPerspectiveCameraComponent();

		Scalar GetFieldOfView() const { return mFov; }
		Scalar GetAspectRatio() const { return mAspect; }

		void SetFieldOfView(Scalar fov);
		void SetAspectRatio(Scalar aspect);

		FRay GenerateRay(Scalar u, Scalar v) const;

		void SetCameraParams(Scalar aspect, Scalar fov, Scalar nearZ, Scalar farZ);

		void Zoom(Scalar factor = Scalar{ 1.0f }) override;

	protected:

		virtual void CreateProjectionMatrix() const override;

		Scalar mFov;
		Scalar mAspect;
	};

	class TFirstPersonCameraComponent : public TPerspectiveCameraComponent
	{
	public:
		TFirstPersonCameraComponent(const std::string& name, TActor* owner, Scalar aspect = 16.0f / 9.0f, Scalar fov = TScalarTraits<Scalar>::Pi() * Scalar { 0.25 }, Scalar nearZ = 0.1f, Scalar farZ = 100.0f, const FViewport& vp = FViewport{});

		float XAxisRotation() const { return mXRot; }
		float YAxisRotation() const { return mYRot; }

		void AddXAxisRotation(Scalar angle);
		void AddYAxisRotation(Scalar angle);

	private:
		Scalar mXRot;
		Scalar mYRot;
	};
}