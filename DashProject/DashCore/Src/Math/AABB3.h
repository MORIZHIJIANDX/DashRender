#pragma once

namespace Dash
{
	template<typename Scalar>
	class TAABB<Scalar, 3>
	{
	public:
		using ScalarType = Scalar;

		constexpr TAABB() noexcept;
		constexpr explicit TAABB(const TScalarArray<Scalar, 3>& p) noexcept;
		constexpr TAABB(const TScalarArray<Scalar, 3>& lower, const TScalarArray<Scalar, 3>& upper) noexcept;

		const TScalarArray<Scalar, 3>& operator[](int i) const noexcept;
		TScalarArray<Scalar, 3>& operator[](int i) noexcept;

		TScalarArray<Scalar, 3> Lower;
		TScalarArray<Scalar, 3> Upper;
	};







	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		template<typename Scalar> TScalarArray<Scalar, 3> Lerp(const TAABB<Scalar, 3>& b, const TScalarArray<Scalar, 3>& t) noexcept;
		template<typename Scalar> TScalarArray<Scalar, 3> Offset(const TAABB<Scalar, 3>& b, const TScalarArray<Scalar, 3>& p) noexcept;
		template<typename Scalar> void BoundingSphere(const TAABB<Scalar, 3>& b, TScalarArray<Scalar, 3>& center, Scalar& radius) noexcept;

		template<typename Scalar> TScalarArray<Scalar, 3> Diagonal(const TAABB<Scalar, 3>& b) noexcept;
		template<typename Scalar> TScalarArray<Scalar, 3> Center(const TAABB<Scalar, 3>& b) noexcept;

		template<typename Scalar> TAABB<Scalar, 3> Union(const TScalarArray<Scalar, 3>& p1,
			const TScalarArray<Scalar, 3>& p2) noexcept;

		template<typename Scalar> TAABB<Scalar, 3> Union(const TAABB<Scalar, 3>& b,
			const TScalarArray<Scalar, 3>& p) noexcept;

		template<typename Scalar> TAABB<Scalar, 3> Union(const TScalarArray<Scalar, 3>& p,
			const TAABB<Scalar, 3>& b) noexcept;

		template<typename Scalar> TAABB<Scalar, 3> Union(const TAABB<Scalar, 3>& b1,
			const TAABB<Scalar, 3>& b2) noexcept;

		template<typename Scalar> TAABB<Scalar, 3> Intersect(const TAABB<Scalar, 3>& b1,
			const TAABB<Scalar, 3>& b2) noexcept;

		template<typename Scalar> bool Overlaps(const TAABB<Scalar, 3>& b1,
			const TAABB<Scalar, 3>& b2) noexcept;

		template<typename Scalar> bool Inside(const TScalarArray<Scalar, 3>& p,
			const TAABB<Scalar, 3>& b) noexcept;

		template<typename Scalar> bool InsideExclusive(const TScalarArray<Scalar, 3>& p,
			const TAABB<Scalar, 3>& b) noexcept;

		template<typename Scalar> TAABB<Scalar, 3> Expand(const TAABB<Scalar, 3>& p,
			Scalar delta) noexcept;

		template<typename Scalar> Scalar Distance(const TScalarArray<Scalar, 3>& p,
			const TAABB<Scalar, 3>& b) noexcept;

		template<typename Scalar> Scalar DistanceSquared(const TScalarArray<Scalar, 3>& p,
			const TAABB<Scalar, 3>& b) noexcept;
	}











	// Member Function

	// --Implementation-- //

	template<typename Scalar>
	FORCEINLINE constexpr TAABB<Scalar, 3>::TAABB() noexcept
	{
		Lower = TScalarArray<Scalar, 3>{ TScalarTraits<Scalar>::Max() , TScalarTraits<Scalar>::Max() , TScalarTraits<Scalar>::Max() };
		Upper = TScalarArray<Scalar, 3>{ TScalarTraits<Scalar>::Lowest() , TScalarTraits<Scalar>::Lowest() , TScalarTraits<Scalar>::Lowest() };
	}

	template<typename Scalar>
	FORCEINLINE constexpr TAABB<Scalar, 3>::TAABB(const TScalarArray<Scalar, 3>& p) noexcept
		: Lower(p)
		, Upper(p)
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TAABB<Scalar, 3>::TAABB(const TScalarArray<Scalar, 3>& lower, const TScalarArray<Scalar, 3>& upper) noexcept
		: Lower(lower)
		, Upper(upper)
	{
	}


	template<typename Scalar>
	FORCEINLINE const TScalarArray<Scalar, 3>& TAABB<Scalar, 3>::operator[](int i) const noexcept
	{
		ASSERT(i == 0 || i == 1);
		return (i == 0) ? &Lower : &Upper;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 3>& TAABB<Scalar, 3>::operator[](int i) noexcept
	{
		ASSERT(i == 0 || i == 1);
		return (i == 0) ? &Lower : &Upper;
	}









	// Non-member Function

	// --Implementation-- //

	namespace FMath
	{
		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 3> Lerp(const TAABB<Scalar, 3>& b, const TScalarArray<Scalar, 3>& t) noexcept
		{
			return TScalarArray<Scalar, 3>{
				Lerp(b.Lower.X, b.Upper.X, t.X),
					Lerp(b.Lower.Y, b.Upper.Y, t.Y),
					Lerp(b.Lower.Z, b.Upper.Z, t.Z)
			};
		}

		template<typename Scalar>
		TScalarArray<Scalar, 3> Offset(const TAABB<Scalar, 3>& b, const TScalarArray<Scalar, 3>& p) noexcept
		{
			TScalarArray<Scalar, 3> result = p - b.Lower;
			if (b.Upper.X > b.Lower.X)	result.X /= b.Upper.X - b.Lower.X;
			if (b.Upper.Y > b.Lower.Y)	result.Y /= b.Upper.Y - b.Lower.Y;
			if (b.Upper.Z > b.Lower.Z)	result.Z /= b.Upper.Z - b.Lower.Z;

			return result;
		}

		template<typename Scalar>
		FORCEINLINE void BoundingSphere(const TAABB<Scalar, 3>& b, TScalarArray<Scalar, 3>& center, Scalar& radius) noexcept
		{
			center = (b.Lower + b.Upper) / Scalar{ 2 };
			radius = Inside(center, b) ? Distance(center, b.Upper) : Scalar{};
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 3> Diagonal(const TAABB<Scalar, 3>& b) noexcept
		{
			return b.Upper - b.Lower;
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 3> Center(const TAABB<Scalar, 3>& b) noexcept
		{
			return (b.Lower + b.Upper) / Scalar{ 2 };
		}

		template<typename Scalar>
		FORCEINLINE TAABB<Scalar, 3> Union(const TScalarArray<Scalar, 3>& p1, const TScalarArray<Scalar, 3>& p2) noexcept
		{
			TAABB<Scalar, 3> result;
			result.Lower = Min(p1, p2);
			result.Upper = Max(p1, p2);

			return result;
		}

		template<typename Scalar>
		FORCEINLINE TAABB<Scalar, 3> Union(const TAABB<Scalar, 3>& b, const TScalarArray<Scalar, 3>& p) noexcept
		{
			TAABB<Scalar, 3> result;
			result.Lower = Min(b.Lower, p);
			result.Upper = Max(b.Upper, p);

			return result;
		}

		template<typename Scalar>
		FORCEINLINE TAABB<Scalar, 3> Union(const TScalarArray<Scalar, 3>& p, const TAABB<Scalar, 3>& b) noexcept
		{
			return Union(b, p);
		}

		template<typename Scalar>
		FORCEINLINE TAABB<Scalar, 3> Union(const TAABB<Scalar, 3>& b1, const TAABB<Scalar, 3>& b2) noexcept
		{
			TAABB<Scalar, 3> result;
			result.Lower = Min(b1.Lower, b2.Lower);
			result.Upper = Max(b1.Upper, b2.Upper);

			return result;
		}

		template<typename Scalar>
		FORCEINLINE TAABB<Scalar, 3> Intersect(const TAABB<Scalar, 3>& b1, const TAABB<Scalar, 3>& b2) noexcept
		{
			TAABB<Scalar, 3> result;
			result.Lower = Max(b1.Lower, b2.Lower);
			result.Upper = Min(b1.Upper, b2.Upper);

			return result;
		}

		template<typename Scalar>
		FORCEINLINE bool Overlaps(const TAABB<Scalar, 3>& b1, const TAABB<Scalar, 3>& b2) noexcept
		{
			bool x = (b1.Upper.X >= b2.Lower.X) && (b1.Lower.X <= b2.Upper.X);
			bool y = (b1.Upper.Y >= b2.Lower.Y) && (b1.Lower.Y <= b2.Upper.Y);
			bool z = (b1.Upper.Z >= b2.Lower.Z) && (b1.Lower.Z <= b2.Upper.Z);

			return x && y && z;
		}

		template<typename Scalar>
		FORCEINLINE bool Inside(const TScalarArray<Scalar, 3>& p, const TAABB<Scalar, 3>& b) noexcept
		{
			bool x = (p.X <= b.Upper.X) && (p.X >= b.Lower.X);
			bool y = (p.Y <= b.Upper.Y) && (p.Y >= b.Lower.Y);
			bool z = (p.Z <= b.Upper.Z) && (p.Z >= b.Lower.Z);

			return x && y && z;
		}

		template<typename Scalar>
		FORCEINLINE bool InsideExclusive(const TScalarArray<Scalar, 3>& p, const TAABB<Scalar, 3>& b) noexcept
		{
			bool x = (p.X < b.Upper.X) && (p.X >= b.Lower.X);
			bool y = (p.Y < b.Upper.Y) && (p.Y >= b.Lower.Y);
			bool z = (p.Z < b.Upper.Z) && (p.Z >= b.Lower.Z);

			return x && y && z;
		}

		template<typename Scalar>
		FORCEINLINE TAABB<Scalar, 3> Expand(const TAABB<Scalar, 3>& b, Scalar delta) noexcept
		{
			TScalarArray<Scalar, 3> deltaVector{ delta, delta, delta };

			return TAABB<Scalar, 3>{ b.Lower - deltaVector, b.Upper + deltaVector };
		}

		template<typename Scalar>
		FORCEINLINE Scalar Distance(const TScalarArray<Scalar, 3>& p, const TAABB<Scalar, 3>& b) noexcept
		{
			TScalarArray<Scalar, 3> result{
				Max(Scalar{}, b.Lower.X - p.X, p.X - b.Upper.X),
				Max(Scalar{}, b.Lower.Y - p.Y, p.Y - b.Upper.Y),
				Max(Scalar{}, b.Lower.Z - p.Z, p.Z - b.Upper.Z)
			};

			return Dot(result, result);
		}

		template<typename Scalar>
		FORCEINLINE Scalar DistanceSquared(const TScalarArray<Scalar, 3>& p, const TAABB<Scalar, 3>& b) noexcept
		{
			return Sqrt(Distance(p, b));
		}
	}
}