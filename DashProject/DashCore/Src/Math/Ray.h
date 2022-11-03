#pragma once

#include "ScalarArray.h"

namespace Dash
{
	template<typename Scalar>
	struct TScalarRay
	{
	public:
		TScalarRay() noexcept;
		TScalarRay(const TScalarArray<Scalar, 3>& origin, const TScalarArray<Scalar, 3>& dir, Scalar tmin = TScalarTraits<Scalar>::Epsilon(), Scalar tmax = TScalarTraits<Scalar>::Infinity()) noexcept;

		TScalarRay(const TScalarRay& r) noexcept;

		TScalarRay<Scalar>& operator=(const TScalarRay<Scalar>& r) noexcept;

		TScalarArray<Scalar, 3> operator()(Scalar t) const noexcept;

		TScalarArray<Scalar, 3> Origin;
		Scalar TMax;
		TScalarArray<Scalar, 3> Direction;
		Scalar TMin;
	};






	// Member Function

	// --Implementation-- //

	template<typename Scalar>
	FORCEINLINE TScalarRay<Scalar>::TScalarRay() noexcept
		: Origin(TScalarArray<Scalar, 3>{ FZero{} })
		, Direction(TScalarArray<Scalar, 3>{ FUnit<2>{} })
		, TMin(TScalarTraits<Scalar>::Epsilon())
		, TMax(TScalarTraits<Scalar>::Infinity())
	{
	}

	template<typename Scalar>
	FORCEINLINE TScalarRay<Scalar>::TScalarRay(const TScalarArray<Scalar, 3>& origin, const TScalarArray<Scalar, 3>& dir, Scalar tmin, Scalar tmax) noexcept
		: Origin(origin)
		, Direction(dir)
		, TMin(tmin)
		, TMax(tmax)
	{
	}

	template<typename Scalar>
	FORCEINLINE TScalarRay<Scalar>::TScalarRay(const TScalarRay& r) noexcept
		: Origin(r.Origin)
		, Direction(r.Direction)
		, TMin(r.TMin)
		, TMax(r.TMax)
	{
	}

	template<typename Scalar>
	FORCEINLINE TScalarRay<Scalar>& TScalarRay<Scalar>::operator=(const TScalarRay& r) noexcept
	{
		Origin = r.Origin;
		Direction = r.Direction;
		TMin = r.TMin;
		TMax = r.TMax;

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 3> TScalarRay<Scalar>::operator()(Scalar t) const noexcept
	{
		return Origin + Direction * t;
	}


}