#pragma once

#include "../Consolid/Consolid.h"

#ifdef USE_OSTREAM

#include <ostream>	

#endif // USE_OSTREAM


namespace Dash {
	

	struct FZero
	{
		template<typename Scalar> explicit operator Scalar() const noexcept;
	};

	struct FIdentity
	{
		template<typename Scalar> explicit operator Scalar() const noexcept;
	};

	template<std::size_t>
	struct FUnit
	{
	};




	// Non-member Operators 

	// --Declaration-- //

	FZero operator-(FZero);
	FZero operator+(FZero, FZero);
	FIdentity operator+(FZero, FIdentity);
	FIdentity operator+(FIdentity, FZero);

	template<typename Element> const Element& operator+(FZero, const Element& e);
	template<typename Element> const Element& operator+(const Element& e, FZero);


	FZero operator-(FZero, FZero);
	FZero operator-(FIdentity, FIdentity);
	FIdentity operator-(FIdentity, FZero);

	template<typename Element>       Element  operator-(FZero, const Element& e);
	template<typename Element> const Element& operator-(const Element& e, FZero);


	FZero operator*(FZero, FZero);
	FZero operator*(FZero, FIdentity);
	FZero operator*(FIdentity, FZero);

	template<typename Element> FZero operator*(FZero, const Element& e);
	template<typename Element> FZero operator*(const Element& e, FZero);

#ifdef USE_OSTREAM

	template<typename CharT, typename Traits>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, FZero);

	template<typename CharT, typename Traits>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, FIdentity);

	template<typename CharT, typename Traits, std::size_t I>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, FUnit<I>);

#endif // USE_OSTREAM







	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		FIdentity Inverse(FIdentity);

		FZero Mul(FZero, FZero);
		FZero Mul(FZero, FIdentity);
		FZero Mul(FIdentity, FZero);

		template<typename Element> FZero Mul(FZero, const Element& e);
		template<typename Element> FZero Mul(const Element& e, FZero);


		FIdentity Mul(FIdentity, FIdentity);

		template<typename Element> const Element& Mul(FIdentity, const Element& e);
		template<typename Element> const Element& Mul(const Element& e, FIdentity);


		template<typename Element> Element Mul(const Element& a, const Element& b, const Element& c);


		FZero Dot(FZero, FZero);

		template<typename Vector> FZero Dot(FZero, const Vector& v);
		template<typename Vector> FZero Dot(const Vector& v, FZero);


		template<std::size_t I> FIdentity Dot(FUnit<I>, FUnit<I>);
		template<std::size_t I, std::size_t J> FZero Dot(FUnit<I>, FUnit<J>);


		template<typename Vector, std::size_t I> typename Vector::ScalarType Dot(const Vector& v, FUnit<I>);
		template<typename Vector, std::size_t I> typename Vector::ScalarType Dot(FUnit<I>, const Vector& v);
	}







	// Member Function

	// --Implementation-- //

	template<typename Scalar>
	FORCEINLINE FZero::operator Scalar() const noexcept
	{
		return Scalar{};
	}

	template<typename Scalar>
	FORCEINLINE FIdentity::operator Scalar() const noexcept
	{
		return Scalar{ 1 };
	}





	// Non-member Operators

	// --Implementation-- //

	FORCEINLINE FZero operator-(FZero)
	{
		return FZero{};
	}

	FORCEINLINE FZero operator+(FZero, FZero)
	{
		return FZero{};
	}

	FORCEINLINE FIdentity operator+(FZero, FIdentity)
	{
		return FIdentity{};
	}

	FORCEINLINE FIdentity operator+(FIdentity, FZero)
	{
		return FIdentity{};
	}

	template<typename Element>
	FORCEINLINE const Element& operator+(FZero, const Element& e)
	{
		return e;
	}

	template<typename Element>
	FORCEINLINE const Element& operator+(const Element& e, FZero)
	{
		return e;
	}

	FORCEINLINE FZero operator-(FZero, FZero)
	{
		return FZero{};
	}

	FORCEINLINE FZero operator-(FIdentity, FIdentity)
	{
		return FZero{};
	}

	FORCEINLINE FIdentity operator-(FIdentity, FZero)
	{
		return FIdentity{};
	}

	template<typename Element>
	FORCEINLINE Element operator-(FZero, const Element& e)
	{
		return -e;
	}

	template<typename Element>
	FORCEINLINE const Element& operator-(const Element& e, FZero)
	{
		return e;
	}

	FORCEINLINE FZero operator*(FZero, FZero)
	{
		return FZero{};
	}

	FORCEINLINE FZero operator*(FZero, FIdentity)
	{
		return FZero{};
	}

	FORCEINLINE FZero operator*(FIdentity, FZero)
	{
		return FZero{};
	}

	template<typename Element>
	FORCEINLINE FZero operator*(FZero, const Element& e)
	{
		return FZero{};
	}

	template<typename Element>
	FORCEINLINE FZero operator*(const Element& e, FZero)
	{
		return FZero{};
	}

#ifdef USE_OSTREAM

	template<typename CharT, typename Traits>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, FZero)
	{
		return os << "FZero";
	}

	template<typename CharT, typename Traits>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, FIdentity)
	{
		return os << "FIdentity";
	}

	template<typename CharT, typename Traits, std::size_t I>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, FUnit<I>)
	{
		return os << "FUnit" << I;
	}

#endif // USE_OSTREAM








	// Nomember Function

	// --Implementation-- //

	namespace FMath
	{
		FORCEINLINE FIdentity Inverse(FIdentity)
		{
			return FIdentity{};
		}

		FORCEINLINE FZero Mul(FZero, FZero)
		{
			return FZero{};
		}

		FORCEINLINE FZero Mul(FZero, FIdentity)
		{
			return FZero{};
		}

		FORCEINLINE FZero Mul(FIdentity, FZero)
		{
			return FZero{};
		}

		template<typename Element>
		FORCEINLINE FZero Mul(FZero, const Element& e)
		{
			return FZero{};
		}

		template<typename Element>
		FORCEINLINE FZero Mul(const Element& e, FZero)
		{
			return FZero{};
		}

		FORCEINLINE FIdentity Mul(FIdentity, FIdentity)
		{
			return FIdentity{};
		}

		template<typename Element>
		FORCEINLINE const Element& Mul(FIdentity, const Element& e)
		{
			return e;
		}

		template<typename Element>
		FORCEINLINE const Element& Mul(const Element& e, FIdentity)
		{
			return e;
		}

		template<typename Element>
		FORCEINLINE Element Mul(const Element& a, const Element& b, const Element& c)
		{
			return Mul(Mul(a, b), c);
		}


		FORCEINLINE FZero Dot(FZero, FZero)
		{
			return FZero{};
		}

		template<typename Vector>
		FORCEINLINE FZero Dot(FZero, const Vector& v)
		{
			return FZero{};
		}

		template<typename Vector>
		FORCEINLINE FZero Dot(const Vector& v, FZero)
		{
			return FZero{};
		}

		template<std::size_t I>
		FORCEINLINE FIdentity Dot(FUnit<I>, FUnit<I>)
		{
			return FIdentity{};
		}

		template<std::size_t I, std::size_t J>
		FORCEINLINE FZero Dot(FUnit<I>, FUnit<J>)
		{
			return FZero{};
		}

		template<typename Vector, std::size_t I>
		FORCEINLINE typename Vector::ScalarType Dot(const Vector& v, FUnit<I>)
		{
			using RT = typename Vector::ScalarType;

			return v[I];
		}

		template<typename Vector, std::size_t I>
		FORCEINLINE typename Vector::ScalarType Dot(FUnit<I>, const Vector& v)
		{
			using RT = typename Vector::ScalarType;

			return v[I];
		}
	}
}