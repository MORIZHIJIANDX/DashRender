#pragma once

#include "ScalarArray.h"

namespace Dash
{
	namespace FMath
	{
		template <typename Vector> typename Vector::ScalarType LengthSquared(const Vector& v);
		template <typename Vector> typename Vector::ScalarType Length(const Vector& v);
		template <typename Vector> typename Vector::ScalarType DistanceSquared(const Vector& p1, const Vector& p2);
		template <typename Vector> typename Vector::ScalarType Distance(const Vector& p1, const Vector& p2);
		template <typename Vector> Vector Normalize(const Vector& v);
		template <typename Vector, typename Scalar> Vector NLerp(const Vector& v1, const Vector& v2, Scalar t);

		template <typename Vector> typename Vector::ScalarType LengthSquared3(const Vector& v);
		template <typename Vector> typename Vector::ScalarType Length3(const Vector& v);
		template <typename Vector> typename Vector::ScalarType DistanceSquared3(const Vector& p1, const Vector& p2);
		template <typename Vector> typename Vector::ScalarType Distance3(const Vector& p1, const Vector& p2);
		template <typename Vector> Vector Normalize3(const Vector& v);
		template <typename Vector, typename Scalar> Vector NLerp3(const Vector& v1, const Vector& v2, Scalar t);

		template<typename Vector>
		FORCEINLINE typename Vector::ScalarType LengthSquared(const Vector& v)
		{
			return Dot(v, v);
		}

		template<typename Vector>
		FORCEINLINE typename Vector::ScalarType Length(const Vector& v)
		{
			return Sqrt(LengthSquared(v));
		}

		template<typename Vector>
		FORCEINLINE typename Vector::ScalarType DistanceSquared(const Vector& p1, const Vector& p2)
		{
			return LengthSquared(p1 - p2);
		}

		template<typename Vector>
		FORCEINLINE typename Vector::ScalarType Distance(const Vector& p1, const Vector& p2)
		{
			return Length(p1 - p2);
		}

		template<typename Vector>
		FORCEINLINE Vector Normalize(const Vector& v)
		{
			using ScalarType = typename Vector::ScalarType;
			ScalarType s = LengthSquared(v);
			return IsPositive(s) ? v * RSqrt(s) : v;
		}

		template<typename Vector, typename Scalar>
		FORCEINLINE Vector NLerp(const Vector& v1, const Vector& v2, Scalar t)
		{
			return Normalize(Lerp(v1, v2, t));
		}

		template<typename Vector>
		FORCEINLINE typename Vector::ScalarType LengthSquared3(const Vector& v)
		{
			return Dot3(v, v);
		}

		template<typename Vector>
		FORCEINLINE typename Vector::ScalarType Length3(const Vector& v)
		{
			return Sqrt(LengthSquared3(v));
		}

		template<typename Vector>
		FORCEINLINE typename Vector::ScalarType DistanceSquared3(const Vector& p1, const Vector& p2)
		{
			return LengthSquared3(p1 - p2);
		}

		template<typename Vector>
		FORCEINLINE typename Vector::ScalarType Distance3(const Vector& p1, const Vector& p2)
		{
			return Length(p1 - p2);
		}

		template<typename Vector>
		FORCEINLINE Vector Normalize3(const Vector& v)
		{
			using ScalarType = typename Vector::ScalarType;
			ScalarType s = LengthSquared3(v);
			return IsPositive(s) ? v * RSqrt(s) : v;
		}

		template<typename Vector, typename Scalar>
		FORCEINLINE Vector NLerp3(const Vector& v1, const Vector& v2, Scalar t)
		{
			return Normalize3(Lerp(v1, v2, t));
		}

	}
}

