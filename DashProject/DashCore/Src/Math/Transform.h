#pragma once

#include "Quaternion.h"
#include "ScalarMatrix.h"
#include "Ray.h"
#include "AABB.h"
#include "../Utility/Assert.h"

namespace Dash
{
	struct FTransform
	{
	public:
		FTransform() noexcept;
		FTransform(FIdentity) noexcept;
		template<typename Scalar> FTransform(const TScalarArray<Scalar, 3>& scale, const TScalarQuaternion<Scalar>& rotation, const TScalarArray<Scalar, 3>& position) noexcept;
		template<typename Scalar> FTransform(const TScalarArray<Scalar, 3>& scale, const TScalarArray<Scalar, 3>& rotationEuler, const TScalarArray<Scalar, 3>& position) noexcept;
		explicit FTransform(const TScalarMatrix<float, 4, 4>& mat) noexcept;
		FTransform(const TScalarMatrix<float, 4, 4>& mat, const TScalarMatrix<float, 4, 4>& inverseMat) noexcept;
		FTransform(const FTransform& t) noexcept;

		FTransform& operator=(const FTransform& t) noexcept;
		FTransform& operator*=(const FTransform& t) noexcept;

		FTransform operator*(const FTransform& t) noexcept;

		operator const TScalarMatrix<float, 4, 4>& () const noexcept;
		operator TScalarMatrix<float, 4, 4>& () noexcept;

		TScalarArray<float, 3> GetScale() const noexcept;
		void SetScale(const TScalarArray<float, 3>& scale) noexcept;

		TScalarQuaternion<float> GetRotation() const;
		void SetRotation(const TScalarQuaternion<float>& rotation) noexcept;

		TScalarArray<float, 3> GetPosition() const noexcept;
		void SetPosition(const TScalarArray<float, 3>& pos) noexcept;

		TScalarArray<float, 3> GetEuler() const noexcept;
		void SetEuler(float pitch, float yaw, float roll) noexcept;
		void SetEuler(const TScalarArray<float, 3>& euler) noexcept;

		void SetLookAt(const TScalarArray<float, 3>& eye, const TScalarArray<float, 3>& lookAt, const TScalarArray<float, 3>& up) noexcept;
		void SetLookTo(const TScalarArray<float, 3>& eye, const TScalarArray<float, 3>& lookTo, const TScalarArray<float, 3>& up) noexcept;

		TScalarArray<float, 3> GetForwardAxis() const noexcept;
		TScalarArray<float, 3> GetUnitForwardAxis() const noexcept;

		TScalarArray<float, 3> GetRightAxis() const noexcept;
		TScalarArray<float, 3> GetUnitRightAxis() const noexcept;

		TScalarArray<float, 3> GetUpAxis() const noexcept;
		TScalarArray<float, 3> GetUnitUpAxis() const noexcept;

		TScalarMatrix<float, 4, 4> GetMatrix() const noexcept;
		TScalarMatrix<float, 4, 4> GetInverseMatrix() const noexcept;

		void Scale(const TScalarArray<float, 3>& r) noexcept;
		void Scale(float x, float y, float z) noexcept;

		void Rotate(const TScalarQuaternion<float>& r) noexcept;
		void RotateAxis(const TScalarArray<float, 3>& axis, float angle) noexcept;
		void RotateAround(const TScalarArray<float, 3>& point, const TScalarArray<float, 3>& axis, float angle) noexcept;

		void Rotate(const TScalarArray<float, 3>& euler) noexcept;
		void Rotate(float x, float y, float z) noexcept;

		void RotateLocal(const TScalarArray<float, 3>& euler) noexcept;
		void RotateLocal(float x, float y, float z) noexcept;

		void Translate(const TScalarArray<float, 3>& t) noexcept;
		void Translate(float x, float y, float z) noexcept;

		void TranslateLocal(const TScalarArray<float, 3>& t) noexcept;
		void TranslateLocal(float x, float y, float z) noexcept;

		TScalarArray<float, 3> TransformVector(const TScalarArray<float, 3>& v) const noexcept;
		TScalarArray<float, 3> TransformPoint(const TScalarArray<float, 3>& p) const noexcept;
		TScalarArray<float, 3> TransformNormal(const TScalarArray<float, 3>& n) const noexcept;

		TScalarArray<float, 4> TransformVector(const TScalarArray<float, 4>& v) const noexcept;
		TScalarArray<float, 4> TransformPoint(const TScalarArray<float, 4>& p) const noexcept;
		TScalarArray<float, 4> TransformNormal(const TScalarArray<float, 4>& n) const noexcept;

		TAABB<float, 3> TransformBoundingBox(const TAABB<float, 3>& b) const noexcept;
		TScalarRay<float> TransformRay(const TScalarRay<float>& r) const noexcept;

	private:
		void UpdateMatrix() const;

		void MakeDirty() { mDirty = true; }

		TScalarArray<float, 3> mScale;
		TScalarQuaternion<float> mRotation;
		TScalarArray<float, 3> mPosition;

		mutable TScalarMatrix<float, 4, 4> mMat;
		mutable TScalarMatrix<float, 4, 4> mInverseMat;

		mutable bool mDirty;
	};









	// Non-member Operators 

	// --Declaration-- //

	bool operator==(const FTransform& a, const FTransform& b) noexcept;








	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		FTransform Inverse(const FTransform& t) noexcept;

		FTransform Scale(const TScalarArray<float, 3>& s) noexcept;
		FTransform Scale(float x, float y, float z) noexcept;

		FTransform Rotate(const TScalarArray<float, 3>& r) noexcept;
		FTransform Rotate(float yaw, float roll, float pitch) noexcept;
		FTransform Rotate(const TScalarQuaternion<float>& r) noexcept;

		FTransform RotateAxis(const TScalarArray<float, 3>& axis, float angle) noexcept;
		FTransform RotateAround(const TScalarArray<float, 3>& point, const TScalarArray<float, 3>& axis, float angle) noexcept;

		FTransform Translate(const TScalarArray<float, 3>& t) noexcept;
		FTransform Translate(float x, float y, float z) noexcept;

		TScalarArray<float, 3> TransformVector(const FTransform& a, const TScalarArray<float, 3>& v) noexcept;
		TScalarArray<float, 3> TransformPoint(const FTransform& a, const TScalarArray<float, 3>& p) noexcept;
		TScalarArray<float, 3> TransformNormal(const FTransform& a, const TScalarArray<float, 3>& n) noexcept;

		TScalarArray<float, 4> TransformVector(const FTransform& a, const TScalarArray<float, 4>& v) noexcept;
		TScalarArray<float, 4> TransformPoint(const FTransform& a, const TScalarArray<float, 4>& p) noexcept;
		TScalarArray<float, 4> TransformNormal(const FTransform& a, const TScalarArray<float, 4>& n) noexcept;

		TAABB<float, 3> TransformBoundingBox(const FTransform& a, const TAABB<float, 3>& b) noexcept;
		TScalarRay<float> TransformRay(const FTransform& a, const TScalarRay<float>& r) noexcept;
	}










	// Member Function

	// --Implementation-- //

	FORCEINLINE FTransform::FTransform() noexcept
		: mScale()
		, mRotation()
		, mPosition()
		, mDirty(true)
		, mMat()
		, mInverseMat()
	{
	}

	FORCEINLINE FTransform::FTransform(FIdentity) noexcept
		: mScale(FIdentity{})
		, mRotation(FIdentity{})
		, mPosition(FIdentity{})
		, mDirty(false)
		, mMat(FIdentity{})
		, mInverseMat(FIdentity{})
	{
	}

	template<typename Scalar>
	FORCEINLINE FTransform::FTransform(const TScalarArray<Scalar, 3>& scale, const TScalarQuaternion<Scalar>& rotation, const TScalarArray<Scalar, 3>& position) noexcept
		: mScale(scale)
		, mRotation(rotation)
		, mPosition(position)
		, mDirty(true)
	{
		UpdateMatrix();
	}

	template<typename Scalar>
	FORCEINLINE FTransform::FTransform(const TScalarArray<Scalar, 3>& scale, const TScalarArray<Scalar, 3>& rotationEuler, const TScalarArray<Scalar, 3>& position) noexcept
		: mScale(scale)
		, mRotation(FMath::FromEuler(rotationEuler))
		, mPosition(position)
		, mDirty(true)
	{
		UpdateMatrix();
	}

	FORCEINLINE FTransform::FTransform(const TScalarMatrix<float, 4, 4>& mat) noexcept
		: mMat(mat)
		, mInverseMat(FMath::Inverse(mat))
		, mDirty(false)
	{
		FMath::DecomposeAffineMatrix4x4(mScale, mRotation, mPosition, mMat);
	}

	FORCEINLINE FTransform::FTransform(const TScalarMatrix<float, 4, 4>& mat, const TScalarMatrix<float, 4, 4>& inverseMat) noexcept
		: mMat(mat)
		, mInverseMat(inverseMat)
		, mDirty(false)
	{
		FMath::DecomposeAffineMatrix4x4(mScale, mRotation, mPosition, mMat);
	}

	FORCEINLINE FTransform::FTransform(const FTransform& t) noexcept
		: mScale(t.mScale)
		, mRotation(t.mRotation)
		, mPosition(t.mPosition)
		, mMat(t.mMat)
		, mInverseMat(t.mInverseMat)
		, mDirty(false)
	{
	}

	FORCEINLINE FTransform& FTransform::operator=(const FTransform& t) noexcept
	{
		mScale = t.mScale;
		mRotation = t.mRotation;
		mPosition = t.mPosition;
		mMat = t.mMat;
		mInverseMat = t.mInverseMat;
		mDirty = false;

		return *this;
	}

	FORCEINLINE FTransform& FTransform::operator*=(const FTransform& t) noexcept
	{
		mMat *= t.mMat;
		mInverseMat = FMath::Inverse(mMat);
		mDirty = false;

		FMath::DecomposeAffineMatrix4x4(mScale, mRotation, mPosition, mMat);

		return *this;
	}

	FORCEINLINE FTransform FTransform::operator*(const FTransform& t) noexcept
	{
		return FTransform{
			mMat * t.mMat,
			t.mInverseMat * mInverseMat
		};
	}

	FORCEINLINE FTransform::operator const TScalarMatrix<float, 4, 4>& () const noexcept
	{
		return mMat;
	}

	FORCEINLINE FTransform::operator TScalarMatrix<float, 4, 4>& () noexcept
	{
		return mMat;
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::GetScale() const noexcept
	{
		return mScale;
	}

	FORCEINLINE void FTransform::SetScale(const TScalarArray<float, 3>& scale) noexcept
	{
		mScale = scale;

		MakeDirty();
	}

	FORCEINLINE TScalarQuaternion<float> FTransform::GetRotation() const
	{
		return mRotation;
	}

	FORCEINLINE void FTransform::SetRotation(const TScalarQuaternion<float>& rotation) noexcept
	{
		mRotation = rotation;

		MakeDirty();
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::GetPosition() const noexcept
	{
		return mPosition;
	}

	FORCEINLINE void FTransform::SetPosition(const TScalarArray<float, 3>& pos) noexcept
	{
		mPosition = pos;

		MakeDirty();
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::GetEuler() const noexcept
	{
		TScalarArray<float, 3> euler;
		FMath::ToEuler(euler, mRotation);

		return euler;
	}

	FORCEINLINE void FTransform::SetEuler(float pitch, float yaw, float roll) noexcept
	{
		mRotation = FMath::FromEuler(pitch, yaw, roll);

		MakeDirty();
	}

	FORCEINLINE void FTransform::SetEuler(const TScalarArray<float, 3>& euler) noexcept
	{
		mRotation = FMath::FromEuler(euler);

		MakeDirty();
	}

	FORCEINLINE void FTransform::SetLookAt(const TScalarArray<float, 3>& eye, const TScalarArray<float, 3>& lookAt, const TScalarArray<float, 3>& up) noexcept
	{
		SetLookTo(eye, lookAt - eye, up);
	}

	FORCEINLINE void FTransform::SetLookTo(const TScalarArray<float, 3>& eye, const TScalarArray<float, 3>& lookTo, const TScalarArray<float, 3>& up) noexcept
	{
		TScalarArray<float, 3> look = FMath::Normalize(lookTo);
		TScalarArray<float, 3> right = FMath::Normalize(FMath::Cross(up, look));
		TScalarArray<float, 3> newUp = FMath::Cross(look, right);

		mMat.SetRow(0, right);
		mMat.SetRow(1, newUp);
		mMat.SetRow(2, look);
		mMat.SetRow(3, eye);

		mInverseMat = FMath::Inverse(mMat);

		FMath::DecomposeAffineMatrix4x4(mScale, mRotation, mPosition, mMat);

		mDirty = false;
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::GetForwardAxis() const noexcept
	{
		return mMat[2].XYZ;
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::GetUnitForwardAxis() const noexcept
	{
		return FMath::Normalize(mMat[2].XYZ);
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::GetRightAxis() const noexcept
	{
		return mMat[0].XYZ;
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::GetUnitRightAxis() const noexcept
	{
		return FMath::Normalize(mMat[0].XYZ);
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::GetUpAxis() const noexcept
	{
		return mMat[1].XYZ;
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::GetUnitUpAxis() const noexcept
	{
		return FMath::Normalize(mMat[1].XYZ);
	}

	FORCEINLINE TScalarMatrix<float, 4, 4> FTransform::GetMatrix() const noexcept
	{
		UpdateMatrix();
		return mMat;
	}

	FORCEINLINE TScalarMatrix<float, 4, 4> FTransform::GetInverseMatrix() const noexcept
	{
		UpdateMatrix();
		return mInverseMat;
	}

	FORCEINLINE void FTransform::Scale(const TScalarArray<float, 3>& r) noexcept
	{
		SetScale(mScale * r);
	}

	FORCEINLINE void FTransform::Scale(float x, float y, float z) noexcept
	{
		Scale(TScalarArray<float, 3>(x, y, z));
	}

	FORCEINLINE void FTransform::Rotate(const TScalarQuaternion<float>& r) noexcept
	{
		SetRotation(r * mRotation);
	}

	FORCEINLINE void FTransform::RotateAxis(const TScalarArray<float, 3>& axis, float angle) noexcept
	{
		SetRotation(TScalarQuaternion<float>{ axis, angle } *mRotation);
	}

	FORCEINLINE void FTransform::RotateAround(const TScalarArray<float, 3>& point, const TScalarArray<float, 3>& axis, float angle) noexcept
	{
		TScalarArray<float, 3> pos = mPosition;
		TScalarQuaternion<float> rot = FMath::FromAxisAngle(axis, angle);
		TScalarArray<float, 3> dir = pos - point;
		dir = FMath::Mul(rot, dir);

		mPosition = point + dir;
		mRotation = rot * mRotation;

		MakeDirty();
	}

	FORCEINLINE void FTransform::Rotate(const TScalarArray<float, 3>& euler) noexcept
	{
		Rotate(euler.X, euler.Y, euler.Z);
	}

	FORCEINLINE void FTransform::Rotate(float x, float y, float z) noexcept
	{
		TScalarQuaternion<float> rx = FMath::FromAxisAngle(0, x);
		TScalarQuaternion<float> ry = FMath::FromAxisAngle(1, y);
		TScalarQuaternion<float> rz = FMath::FromAxisAngle(2, z);

		SetRotation(rz * ry * rx * mRotation);
	}

	FORCEINLINE void FTransform::RotateLocal(const TScalarArray<float, 3>& euler) noexcept
	{
		RotateLocal(euler.X, euler.Y, euler.Z);
	}

	FORCEINLINE void FTransform::RotateLocal(float x, float y, float z) noexcept
	{
		TScalarQuaternion<float> rx = FMath::FromAxisAngle(GetUnitRightAxis(), x);
		TScalarQuaternion<float> ry = FMath::FromAxisAngle(GetUnitUpAxis(), y);
		TScalarQuaternion<float> rz = FMath::FromAxisAngle(GetUnitForwardAxis(), z);

		SetRotation(rz * ry * rx * mRotation);
	}

	FORCEINLINE void FTransform::Translate(const TScalarArray<float, 3>& t) noexcept
	{
		SetPosition(mPosition + t);
	}

	FORCEINLINE void FTransform::Translate(float x, float y, float z) noexcept
	{
		Translate(TScalarArray<float, 3>(x, y, z));
	}

	FORCEINLINE void FTransform::TranslateLocal(const TScalarArray<float, 3>& t) noexcept
	{
		TranslateLocal(t.X, t.Y, t.Z);
	}

	FORCEINLINE void FTransform::TranslateLocal(float x, float y, float z) noexcept
	{
		SetPosition(mPosition + GetUnitRightAxis() * x + GetUnitUpAxis() * y + GetUnitForwardAxis() * z);
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::TransformVector(const TScalarArray<float, 3>& v) const noexcept
	{
		return TScalarArray<float, 3>{
			FMath::Dot(v, FMath::Column(mMat, 0).XYZ),
			FMath::Dot(v, FMath::Column(mMat, 1).XYZ),
			FMath::Dot(v, FMath::Column(mMat, 2).XYZ)
		};
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::TransformPoint(const TScalarArray<float, 3>& p) const noexcept
	{
		TScalarArray<float, 4> hp{ p, float{1} };

		TScalarArray<float, 3> result{
			FMath::Dot(hp, FMath::Column(mMat, 0)),
			FMath::Dot(hp, FMath::Column(mMat, 1)),
			FMath::Dot(hp, FMath::Column(mMat, 2))
		};

		float w = FMath::Dot(hp, FMath::Column(mMat, 3));

		ASSERT(!FMath::IsZero(w));

		if (w == 1)
			return result;
		else
			return result / w;
	}

	FORCEINLINE TScalarArray<float, 3> FTransform::TransformNormal(const TScalarArray<float, 3>& n) const noexcept
	{
		return TScalarArray<float, 3>{
			FMath::Dot(n, FMath::Row(mInverseMat, 0).XYZ),
			FMath::Dot(n, FMath::Row(mInverseMat, 1).XYZ),
			FMath::Dot(n, FMath::Row(mInverseMat, 2).XYZ)
		};
	}

	FORCEINLINE TScalarArray<float, 4> FTransform::TransformVector(const TScalarArray<float, 4>& v) const noexcept
	{
		return TScalarArray<float, 4>{
			FMath::Dot(v.XYZ, FMath::Column(mMat, 0).XYZ),
			FMath::Dot(v.XYZ, FMath::Column(mMat, 1).XYZ),
			FMath::Dot(v.XYZ, FMath::Column(mMat, 2).XYZ),
			float{}
		};
	}

	FORCEINLINE TScalarArray<float, 4> FTransform::TransformPoint(const TScalarArray<float, 4>& p) const noexcept
	{
		TScalarArray<float, 4> hp = FMath::Mul(p, mMat);

		ASSERT(!FMath::IsZero(hp.W));

		if (hp.W == 1)
			return hp;
		else
			return hp / hp.W;
	}

	FORCEINLINE TScalarArray<float, 4> FTransform::TransformNormal(const TScalarArray<float, 4>& n) const noexcept
	{
		return TScalarArray<float, 4>{
			FMath::Dot(n.XYZ, FMath::Row(mInverseMat, 0).XYZ),
			FMath::Dot(n.XYZ, FMath::Row(mInverseMat, 1).XYZ),
			FMath::Dot(n.XYZ, FMath::Row(mInverseMat, 2).XYZ),
			float{}
		};
	}

	FORCEINLINE TAABB<float, 3> FTransform::TransformBoundingBox(const TAABB<float, 3>& b) const noexcept
	{
		const FTransform& M = *this;
		TAABB<float, 3> ret(M.TransformPoint(TScalarArray<float, 3>(b.Lower.X, b.Lower.Y, b.Lower.Z)));
		ret = FMath::Union(ret, (M.TransformPoint(TScalarArray<float, 3>{ b.Upper.X, b.Lower.Y, b.Lower.Z })));
		ret = FMath::Union(ret, (M.TransformPoint(TScalarArray<float, 3>{ b.Lower.X, b.Upper.Y, b.Lower.Z })));
		ret = FMath::Union(ret, (M.TransformPoint(TScalarArray<float, 3>{ b.Lower.X, b.Lower.Y, b.Upper.Z })));
		ret = FMath::Union(ret, (M.TransformPoint(TScalarArray<float, 3>{ b.Lower.X, b.Upper.Y, b.Upper.Z })));
		ret = FMath::Union(ret, (M.TransformPoint(TScalarArray<float, 3>{ b.Upper.X, b.Upper.Y, b.Lower.Z })));
		ret = FMath::Union(ret, (M.TransformPoint(TScalarArray<float, 3>{ b.Upper.X, b.Lower.Y, b.Upper.Z })));
		ret = FMath::Union(ret, (M.TransformPoint(TScalarArray<float, 3>{ b.Upper.X, b.Upper.Y, b.Upper.Z })));
		return ret;
	}

	FORCEINLINE TScalarRay<float> FTransform::TransformRay(const TScalarRay<float>& r) const noexcept
	{
		return TScalarRay<float>{ TransformPoint(r.Origin), FMath::Normalize(TransformVector(r.Direction)), r.TMin, r.TMax };
	}

	FORCEINLINE void FTransform::UpdateMatrix() const
	{
		if (mDirty)
		{
			mMat = FMath::ScaleMatrix4x4<float>(mScale) * FMath::RotateMatrix4x4<float>(mRotation) * FMath::TranslateMatrix4x4<float>(mPosition);
			mInverseMat = FMath::Inverse(mMat);
			mDirty = false;
		}
	}







	// Non-member Operators 

	// --Implementation-- //

	FORCEINLINE bool operator==(const FTransform& a, const FTransform& b) noexcept
	{
		return a.GetScale() == b.GetScale() && a.GetRotation() == b.GetRotation() && a.GetPosition() == b.GetPosition();
	}










	// Non-member Function

	// --Implementation-- //

	namespace FMath
	{
		FORCEINLINE FTransform Inverse(const FTransform& t) noexcept
		{
			return FTransform{ t.GetInverseMatrix(), t.GetMatrix() };
		}

		FORCEINLINE FTransform Scale(const TScalarArray<float, 3>& s) noexcept
		{
			return FTransform{ s, TScalarQuaternion<float>{FIdentity{}}, TScalarArray<float, 3>{FZero{}} };
		}

		FORCEINLINE FTransform Scale(float x, float y, float z) noexcept
		{
			return FTransform{ TScalarArray<float, 3>{x,y,z}, TScalarQuaternion<float>{FIdentity{}}, TScalarArray<float, 3>{FZero{}} };
		}

		FORCEINLINE FTransform Rotate(const TScalarArray<float, 3>& r) noexcept
		{
			return FTransform{ TScalarArray<float, 3>{FIdentity{}}, r, TScalarArray<float, 3>{FZero{}} };
		}

		FORCEINLINE FTransform Rotate(float yaw, float roll, float pitch) noexcept
		{
			return FTransform{ TScalarArray<float, 3>{FIdentity{}}, TScalarArray<float, 3>{yaw, roll, pitch}, TScalarArray<float, 3>{FZero{}} };
		}

		FORCEINLINE FTransform Rotate(const TScalarQuaternion<float>& r) noexcept
		{
			return FTransform{ TScalarArray<float, 3>{FIdentity{}}, r, TScalarArray<float, 3>{FZero{}} };
		}

		FORCEINLINE FTransform RotateAxis(const TScalarArray<float, 3>& axis, float angle) noexcept
		{
			return FTransform{ TScalarArray<float, 3>{FIdentity{}}, FMath::FromAxisAngle(axis, angle), TScalarArray<float, 3>{FZero{}} };
		}

		FORCEINLINE FTransform RotateAround(const TScalarArray<float, 3>& point, const TScalarArray<float, 3>& axis, float angle) noexcept
		{
			TScalarArray<float, 3> pos = TScalarArray<float, 3>{ FZero{} };
			TScalarQuaternion<float> rot = FMath::FromAxisAngle(axis, angle);
			TScalarArray<float, 3> dir = pos - point;
			dir = FMath::Mul(rot, dir);

			pos = point + dir;

			return FTransform{ TScalarArray<float, 3>{FIdentity{}}, rot, pos };
		}

		FORCEINLINE FTransform Translate(const TScalarArray<float, 3>& t) noexcept
		{
			return FTransform{ TScalarArray<float, 3>{FIdentity{}}, TScalarQuaternion<float>{FIdentity{}}, t };
		}

		FORCEINLINE FTransform Translate(float x, float y, float z) noexcept
		{
			return FTransform{ TScalarArray<float, 3>{FIdentity{}}, TScalarQuaternion<float>{FIdentity{}}, TScalarArray<float, 3>{x, y, z} };
		}

		FORCEINLINE TScalarArray<float, 3> TransformVector(const FTransform& a, const TScalarArray<float, 3>& v) noexcept
		{
			return a.TransformVector(v);
		}

		FORCEINLINE TScalarArray<float, 3> TransformPoint(const FTransform& a, const TScalarArray<float, 3>& p) noexcept
		{
			return a.TransformPoint(p);
		}

		FORCEINLINE TScalarArray<float, 3> TransformNormal(const FTransform& a, const TScalarArray<float, 3>& n) noexcept
		{
			return a.TransformNormal(n);
		}

		FORCEINLINE TScalarArray<float, 4> TransformVector(const FTransform& a, const TScalarArray<float, 4>& v) noexcept
		{
			return a.TransformVector(v);
		}

		FORCEINLINE TScalarArray<float, 4> TransformPoint(const FTransform& a, const TScalarArray<float, 4>& p) noexcept
		{
			return a.TransformPoint(p);
		}

		FORCEINLINE TScalarArray<float, 4> TransformNormal(const FTransform& a, const TScalarArray<float, 4>& n) noexcept
		{
			return a.TransformNormal(n);
		}

		FORCEINLINE TAABB<float, 3> TransformBoundingBox(const FTransform& a, const TAABB<float, 3>& b) noexcept
		{
			return a.TransformBoundingBox(b);
		}

		FORCEINLINE TScalarRay<float> TransformRay(const FTransform& a, const TScalarRay<float>& r) noexcept
		{
			return a.TransformRay(r);
		}
	}
}