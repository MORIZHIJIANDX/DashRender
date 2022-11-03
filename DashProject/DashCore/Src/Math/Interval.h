#pragma once

//#include "../consolid/consolid.h"
#include "../Consolid/Consolid.h"
#include "Scalar.h"
#include "Promote.h"

#ifdef USE_OSTREAM
#include <ostream>	
#endif // USE_OSTREAM

namespace Dash
{
	template<typename Scalar>
	struct TInterval
	{
	public:
		constexpr TInterval() noexcept;
		constexpr explicit TInterval(Scalar x) noexcept;
		constexpr TInterval(Scalar a, Scalar b) noexcept;
		template<typename Scalar2> TInterval(const TInterval<Scalar2> inter) noexcept;

		Scalar Lower;
		Scalar Upper;
	};







	// Non-member Operators 

	// --Declaration-- //

	namespace FMath
	{
#ifdef USE_OSTREAM

		template <typename CharT, typename Traits, typename Scalar>
		std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TInterval<Scalar>& z);

#endif
	}





	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		template <typename Scalar> Scalar Width(const TInterval<Scalar>& z) noexcept;
		template <typename Scalar> Scalar Median(const TInterval<Scalar>& z) noexcept;

		template <typename Scalar1, typename Scalar2> bool Overlap(const TInterval<Scalar1>& z1, const TInterval<Scalar2>& z2) noexcept;
		template <typename Scalar1, typename Scalar2> bool Subset(const TInterval<Scalar1>& z1, const TInterval<Scalar2>& z2) noexcept;
		template <typename Scalar> bool In(Scalar x, const TInterval<Scalar>& z) noexcept;

		template <typename Scalar> TInterval<Scalar> Union(Scalar x, Scalar y) noexcept;
		template <typename Scalar> TInterval<Scalar> Union(const TInterval<Scalar>& z, Scalar x) noexcept;
		template <typename Scalar> TInterval<Scalar> Union(Scalar x, const TInterval<Scalar>& z) noexcept;
		template <typename Scalar> TInterval<Scalar> Union(const TInterval<Scalar>& z1, const TInterval<Scalar>& z2) noexcept;

		template <typename Scalar> Scalar           Clamp(Scalar x, const TInterval<Scalar>& z) noexcept;
		template <typename Scalar> TInterval<Scalar> Clamp(const TInterval<Scalar>& z1, const TInterval<Scalar>& z2) noexcept;
	}










	// Member Function

	// --Implementation-- //

	template<typename Scalar>
	FORCEINLINE constexpr TInterval<Scalar>::TInterval() noexcept
		: Lower(TScalarTraits<Scalar>::Max())
		, Upper(TScalarTraits<Scalar>::Lowest())
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TInterval<Scalar>::TInterval(Scalar x) noexcept
		: Lower(x)
		, Upper(x)
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TInterval<Scalar>::TInterval(Scalar a, Scalar b) noexcept
		: Lower(Min(a, b))
		, Upper(Max(a, b))
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TInterval<Scalar>::TInterval(const TInterval<Scalar2> inter) noexcept
		: Lower(Scalar{ inter.Lower })
		, Upper(Scalar{ inter.Upper })
	{
	}








	// Non-member Operators 

	// --Implementation-- //

#ifdef USE_OSTREAM
	template<typename CharT, typename Traits, typename Scalar>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TInterval<Scalar>& z)
	{
		os << "[" << z.Lower << ", " << z.Upper << "]";
	}
#endif










	// Non-member Function

	// --Implementation-- //


	template<typename Scalar>
	FORCEINLINE Scalar Width(const TInterval<Scalar>& z) noexcept
	{
		return z.Upper - z.Lower;
	}

	template<typename Scalar>
	FORCEINLINE Scalar Median(const TInterval<Scalar>& z) noexcept
	{
		return (z.Upper + z.Lower) * Scalar { 0.5 };
	}



	template<typename Scalar1, typename Scalar2>
	FORCEINLINE bool Overlap(const TInterval<Scalar1>& z1, const TInterval<Scalar2>& z2) noexcept
	{
		return !(z1.Upper < z2.Lower || z2.Upper < z1.Lower);
	}

	template<typename Scalar1, typename Scalar2>
	FORCEINLINE bool Subset(const TInterval<Scalar1>& z1, const TInterval<Scalar2>& z2) noexcept
	{
		return !(z1.Lower < z2.Lower || (z2.Upper < z1.Upper));
	}

	template<typename Scalar>
	FORCEINLINE bool In(Scalar x, const TInterval<Scalar>& z) noexcept
	{
		return !(x < z.Lower || x > z.Upper);
	}

	template<typename Scalar>
	FORCEINLINE TInterval<Scalar> Union(Scalar x, Scalar y) noexcept
	{
		return x < y ? TInterval<Scalar>{ x, y } : TInterval<Scalar>{ y , x };
	}

	template<typename Scalar>
	FORCEINLINE TInterval<Scalar> Union(const TInterval<Scalar>& z, Scalar x) noexcept
	{
		return x < z.Lower ? TInterval<Scalar>{x, z.Upper} : (x > z.Upper ? TInterval<Scalar>{ z.Lower, x } : z);
	}

	template<typename Scalar>
	FORCEINLINE TInterval<Scalar> Union(Scalar x, const TInterval<Scalar>& z) noexcept
	{
		return Union(z, x);
	}

	template<typename Scalar>
	FORCEINLINE TInterval<Scalar> Union(const TInterval<Scalar>& z1, const TInterval<Scalar>& z2) noexcept
	{
		return TInterval<Scalar>{ Min(z1.Lower, z2.Lower), Max(z1.Upper, z2.Upper) };
	}

	template<typename Scalar>
	FORCEINLINE Scalar Clamp(Scalar x, const TInterval<Scalar>& z) noexcept
	{
		return Clamp(x, z.Lower, z.Upper);
	}

	template<typename Scalar>
	FORCEINLINE TInterval<Scalar> Clamp(const TInterval<Scalar>& z1, const TInterval<Scalar>& z2) noexcept
	{
		return z1.Upper < z2.Lower ? TInterval<Scalar>(z2.Lower, z2.Lower) :
			z2.Upper < z1.Lower ? TInterval<Scalar>(z2.Upper, z2.Upper) :
			TInterval<Scalar>{ Max(z1.Lower, z2.Lower), Min(z1.Upper, z2.Upper) };
	}



}