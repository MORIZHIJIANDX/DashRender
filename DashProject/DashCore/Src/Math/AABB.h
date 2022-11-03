#pragma once

#include "ScalarArray.h"
#include "ScalarTraits.h"

namespace Dash
{

	template<typename Scalar, std::size_t N>
	struct TAABB
	{
	public:
		using ScalarType = Scalar;

		constexpr TAABB() noexcept;
		constexpr explicit TAABB(const TScalarArray<Scalar, N>& p) noexcept;
		constexpr TAABB(const TScalarArray<Scalar, N>& lower, const TScalarArray<Scalar, N>& upper) noexcept;

		const TScalarArray<Scalar, N>& operator[](int i) const noexcept;
		TScalarArray<Scalar, N>& operator[](int i) noexcept;

		TScalarArray<Scalar, N> Lower;
		TScalarArray<Scalar, N> Upper;
	};









	// Non-member Operators 

	// --Declaration-- //
	
	template<typename Scalar, std::size_t N> bool operator==(const TAABB<Scalar, N>& b1, const TAABB<Scalar, N>& b2) noexcept;
	template<typename Scalar, std::size_t N> bool operator!=(const TAABB<Scalar, N>& b1, const TAABB<Scalar, N>& b2) noexcept;









	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Lerp(const TAABB<Scalar, N>& b, const TScalarArray<Scalar, N>& t) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Offset(const TAABB<Scalar, N>& b, const TScalarArray<Scalar, N>& p) noexcept;
		template<typename Scalar, std::size_t N> void BoundingSphere(const TAABB<Scalar, N>& b, TScalarArray<Scalar, N>& center, Scalar& radius) noexcept;

		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Diagonal(const TAABB<Scalar, N>& b) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Center(const TAABB<Scalar, N>& b) noexcept;

		template<typename Scalar, std::size_t N> TAABB<Scalar, N> Union(const TScalarArray<Scalar, N>& p1,
			const TScalarArray<Scalar, N>& p2) noexcept;

		template<typename Scalar, std::size_t N> TAABB<Scalar, N> Union(const TAABB<Scalar, N>& b,
			const TScalarArray<Scalar, N>& p) noexcept;

		template<typename Scalar, std::size_t N> TAABB<Scalar, N> Union(const TScalarArray<Scalar, N>& p,
			const TAABB<Scalar, N>& b) noexcept;

		template<typename Scalar, std::size_t N> TAABB<Scalar, N> Union(const TAABB<Scalar, N>& b1,
			const TAABB<Scalar, N>& b2) noexcept;

		template<typename Scalar, std::size_t N> TAABB<Scalar, N> Intersect(const TAABB<Scalar, N>& b1,
			const TAABB<Scalar, N>& b2) noexcept;

		template<typename Scalar, std::size_t N> bool Overlaps(const TAABB<Scalar, N>& b1,
			const TAABB<Scalar, N>& b2) noexcept;

		template<typename Scalar, std::size_t N> bool Inside(const TScalarArray<Scalar, N>& p,
			const TAABB<Scalar, N>& b) noexcept;

		template<typename Scalar, std::size_t N> bool InsideExclusive(const TScalarArray<Scalar, N>& p,
			const TAABB<Scalar, N>& b) noexcept;

		template<typename Scalar, std::size_t N> TAABB<Scalar, N> Expand(const TAABB<Scalar, N>& p,
			Scalar delta) noexcept;

		template<typename Scalar, std::size_t N> Scalar Distance(const TScalarArray<Scalar, N>& p,
			const TAABB<Scalar, N>& b) noexcept;

		template<typename Scalar, std::size_t N> Scalar DistanceSquared(const TScalarArray<Scalar, N>& p,
			const TAABB<Scalar, N>& b) noexcept;
	}










	// Member Function

	// --Implementation-- //


	template<typename Scalar, std::size_t N>
	FORCEINLINE constexpr TAABB<Scalar, N>::TAABB() noexcept
	{
		Lower.Fill(TScalarTraits<Scalar>::Max());
		Upper.Fill(TScalarTraits<Scalar>::Lowest());
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE constexpr TAABB<Scalar, N>::TAABB(const TScalarArray<Scalar, N>& p) noexcept
		: Lower(p)
		, Upper(p)
	{
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE constexpr TAABB<Scalar, N>::TAABB(const TScalarArray<Scalar, N>& lower, const TScalarArray<Scalar, N>& upper) noexcept
		: Lower(lower)
		, Upper(upper)
	{
	}


	template<typename Scalar, std::size_t N>
	FORCEINLINE const TScalarArray<Scalar, N>& TAABB<Scalar, N>::operator[](int i) const noexcept
	{
		ASSERT(i == 0 || i == 1);
		return (i == 0) ? &Lower : &Upper;
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE TScalarArray<Scalar, N>& TAABB<Scalar, N>::operator[](int i) noexcept
	{
		ASSERT(i == 0 || i == 1);
		return (i == 0) ? &Lower : &Upper;
	}










	// Non-member Operators 

	// --Implementation-- //

	template<typename Scalar, std::size_t N>
	bool operator==(const TAABB<Scalar, N>& b1, const TAABB<Scalar, N>& b2) noexcept
	{
		return b1.Lower == b2.Lower && b1.Upper == b2.Upper;
	}

	template<typename Scalar, std::size_t N>
	bool operator!=(const TAABB<Scalar, N>& b1, const TAABB<Scalar, N>& b2) noexcept
	{
		return !(b1 == b2);
	}









	// Non-member Function

	// --Implementation-- //

	namespace FMath
	{
		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Lerp(const TAABB<Scalar, N>& b, const TScalarArray<Scalar, N>& t) noexcept
		{
			TScalarArray<Scalar, N> result;
			for (std::size_t i = 0; i < N; i++)
			{
				result[i] = Lerp(b.Lower[i], b.Upper[i], t[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		TScalarArray<Scalar, N> Offset(const TAABB<Scalar, N>& b, const TScalarArray<Scalar, N>& p) noexcept
		{
			TScalarArray<Scalar, N> result = p - b.Lower;
			for (std::size_t i = 0; i < N; i++)
			{
				if (b.Upper[i] > b.Lower[i])
				{
					result[i] /= b.Upper[i] - b.Lower[i];
				}
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE void BoundingSphere(const TAABB<Scalar, N>& b, TScalarArray<Scalar, N>& center, Scalar& radius) noexcept
		{
			center = (b.Lower + b.Upper) / Scalar{ 2 };
			radius = Inside(center, b) ? Distance(center, b.Upper) : Scalar{};
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Diagonal(const TAABB<Scalar, N>& b) noexcept
		{
			return b.Upper - b.Lower;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Center(const TAABB<Scalar, N>& b) noexcept
		{
			return (b.Lower + b.Upper) / Scalar{ 2 };
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TAABB<Scalar, N> Union(const TScalarArray<Scalar, N>& p1, const TScalarArray<Scalar, N>& p2) noexcept
		{
			TAABB<Scalar, N> result;
			for (std::size_t i = 0; i < N; i++)
			{
				result.Lower[i] = Min(p1[i], p2[i]);
				result.Upper[i] = Max(p1[i], p2[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TAABB<Scalar, N> Union(const TAABB<Scalar, N>& b, const TScalarArray<Scalar, N>& p) noexcept
		{
			TAABB<Scalar, N> result;
			for (std::size_t i = 0; i < N; i++)
			{
				result.Lower[i] = Min(b.Lower[i], p[i]);
				result.Upper[i] = Max(b.Upper[i], p[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TAABB<Scalar, N> Union(const TScalarArray<Scalar, N>& p, const TAABB<Scalar, N>& b) noexcept
		{
			return Union(b, p);
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TAABB<Scalar, N> Union(const TAABB<Scalar, N>& b1, const TAABB<Scalar, N>& b2) noexcept
		{
			TScalarArray<Scalar, N> result;
			for (std::size_t i = 0; i < N; i++)
			{
				result.Lower[i] = Min(b1.Lower[i], b2.Lower[i]);
				result.Upper[i] = Max(b1.Upper[i], b2.Upper[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TAABB<Scalar, N> Intersect(const TAABB<Scalar, N>& b1, const TAABB<Scalar, N>& b2) noexcept
		{
			TAABB<Scalar, N> result;
			for (std::size_t i = 0; i < N; i++)
			{
				result.Lower[i] = Max(b1.Lower[i], b2.Lower[i]);
				result.Upper[i] = Min(b1.Upper[i], b2.Upper[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE bool Overlaps(const TAABB<Scalar, N>& b1, const TAABB<Scalar, N>& b2) noexcept
		{
			TScalarArray<bool, N> result;
			for (std::size_t i = 0; i < N; i++)
			{
				result[i] = (b1.Upper[i] >= b2.Lower[i]) && (b1.Lower[i] <= b2.Upper[i]);
			}

			for (std::size_t i = 1; i < N; i++)
			{
				result[0] = result[0] && result[i];
			}

			return result[0];
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE bool Inside(const TScalarArray<Scalar, N>& p, const TAABB<Scalar, N>& b) noexcept
		{
			TScalarArray<bool, N> result;
			for (std::size_t i = 0; i < N; i++)
			{
				result[i] = (p[i] <= b.Upper[i]) && (p[i] >= b.Lower[i]);
			}

			for (std::size_t i = 1; i < N; i++)
			{
				result[0] = result[0] && result[i];
			}

			return result[0];
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE bool InsideExclusive(const TScalarArray<Scalar, N>& p, const TAABB<Scalar, N>& b) noexcept
		{
			TScalarArray<bool, N> result;
			for (std::size_t i = 0; i < N; i++)
			{
				result[i] = (p[i] < b.Upper[i]) && (p[i] >= b.Lower[i]);
			}

			for (std::size_t i = 1; i < N; i++)
			{
				result[0] = result[0] && result[i];
			}

			return result[0];
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TAABB<Scalar, N> Expand(const TAABB<Scalar, N>& b, Scalar delta) noexcept
		{
			TScalarArray<Scalar, N> deltaVector;
			deltaVector.Fill(delta);

			return TAABB<Scalar, N>{ b.Lower - deltaVector, b.Upper + deltaVector };
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE Scalar Distance(const TScalarArray<Scalar, N>& p, const TAABB<Scalar, N>& b) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (std::size_t i = 0; i < N; i++)
			{
				result[i] = Max(Scalar{}, b.Lower[i] - p[i], p[i] - b.Upper[i]);
			}

			return Dot(result, result);
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE Scalar DistanceSquared(const TScalarArray<Scalar, N>& p, const TAABB<Scalar, N>& b) noexcept
		{
			return Sqrt(Distance(p, b));
		}
	}
}

#include "AABB2.h"
#include "AABB3.h"