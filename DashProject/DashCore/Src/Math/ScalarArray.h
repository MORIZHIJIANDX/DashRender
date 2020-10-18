#pragma once

//#include "../Consolid/Consolid.h"
#include "../Utility/Assert.h"
#include "Algebra.h"
#include "Promote.h"
#include "Scalar.h"

#include <array>
#include <initializer_list>

#ifdef USE_OSTREAM
#include <ostream>	
#endif // USE_OSTREAM

#ifdef USE_ISTREAM
#include <istream>	
#endif // USE_ISTREAM

namespace Dash 
{
	template<typename Scalar, std::size_t N>
	class TScalarArray
	{
	public:
		using DataType = std::array<Scalar, N>;

		using ScalarType = Scalar;
		using SizeType = std::size_t;
		using DifferenceType = typename DataType::difference_type;
		using Pointer = ScalarType*;
		using ConstPointer = const ScalarType*;
		using Reference = ScalarType&;
		using ConstReference = const ScalarType&;

		using Iterator = typename DataType::iterator;
		using ConstIterator = typename DataType::const_iterator;

		using ReverseIterator = typename DataType::reverse_iterator;
		using ConstReverseIterator = typename DataType::const_reverse_iterator;

		constexpr TScalarArray() noexcept;
		constexpr explicit TScalarArray(FZero) noexcept;
		template <std::size_t I> constexpr explicit TScalarArray(FUnit<I>) noexcept;
		template <typename Scalar2> constexpr explicit TScalarArray(const Scalar2* v) noexcept;
		constexpr explicit TScalarArray(std::initializer_list<Scalar> list) noexcept;
		template <typename Scalar2> constexpr explicit TScalarArray(const TScalarArray<Scalar2, N - 1>& v) noexcept;
		template <typename Scalar2> constexpr TScalarArray(const TScalarArray<Scalar2, N>& v) noexcept;

		operator ConstPointer () const noexcept;
		operator Pointer () noexcept;

		TScalarArray<Scalar, N>& operator=(FZero) noexcept;
		TScalarArray<Scalar, N>& operator+=(FZero) noexcept;
		TScalarArray<Scalar, N>& operator-=(FZero) noexcept;
		TScalarArray<Scalar, N>& operator*=(FZero) noexcept;

		template<std::size_t I> TScalarArray<Scalar, N>& operator=(FUnit<I>) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, N>& operator=(const Scalar2* v) noexcept;

		template<typename Scalar2> TScalarArray<Scalar, N>& operator=(const TScalarArray<Scalar2, N>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, N>& operator+=(const TScalarArray<Scalar2, N>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, N>& operator-=(const TScalarArray<Scalar2, N>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, N>& operator*=(const TScalarArray<Scalar2, N>& v) noexcept;

		TScalarArray<Scalar, N>& operator*=(Scalar s) noexcept;
		TScalarArray<Scalar, N>& operator/=(Scalar s) noexcept;

		template<typename Scalar2> void Fill(Scalar2 s) noexcept;

		constexpr SizeType GetSize() const noexcept { return N; }

		constexpr Iterator Begin() noexcept;
		constexpr ConstIterator Begin() const noexcept;

		constexpr Iterator End() noexcept;
		constexpr ConstIterator End() const noexcept;

	private:
		DataType mData;
	};








	// Non-member Operators 

	// --Declaration-- //

#ifdef USE_OSTREAM

	template<typename CharT, typename Traits, typename Scalar, std::size_t N>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TScalarArray<Scalar, N>& v);

#endif // USE_OSTREAM

#ifdef USE_ISTREAM

	template<typename CharT, typename Traits, typename Scalar, std::size_t N>
	std::basic_istream<CharT, Traits>& operator>>(std::basic_ostream<CharT, Traits>& is, const TScalarArray<Scalar, N>& v);

#endif // USE_ISTREAM

	template<typename Scalar1, typename Scalar2, std::size_t N> bool operator==(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept;
	template<typename Scalar1, typename Scalar2, std::size_t N> bool operator!=(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept;

	template <typename Scalar, std::size_t N> TScalarArray<Scalar, N> operator-(const TScalarArray<Scalar, N>& a) noexcept;

	template<typename Scalar1, typename Scalar2, std::size_t N>
	TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator+(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept;

	template<typename Scalar1, typename Scalar2, std::size_t N>
	TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator-(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept;

	template<typename Scalar1, typename Scalar2, std::size_t N>
	TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator*(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept;

	template<typename Scalar1, typename Scalar2, std::size_t N>
	TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator*(const TScalarArray<Scalar1, N>& v, Scalar2 s) noexcept;

	template<typename Scalar1, typename Scalar2, std::size_t N>
	TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator*(Scalar2 s, const TScalarArray<Scalar1, N>& v) noexcept;

	template<typename Scalar1, typename Scalar2, std::size_t N>
	TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator/(const TScalarArray<Scalar1, N>& v, Scalar2 s) noexcept;








	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		template<typename Scalar, std::size_t N>
		std::size_t MaxDimension(const TScalarArray<Scalar, N>& v) noexcept;

		template<typename Scalar, std::size_t N>
		std::size_t MinDimension(const TScalarArray<Scalar, N>& v) noexcept;

		template<typename Scalar, std::size_t N>
		Scalar HorizontalAdd(const TScalarArray<Scalar, N>& v) noexcept;

		template<typename Scalar, std::size_t N>
		Scalar HorizontalMin(const TScalarArray<Scalar, N>& v) noexcept;

		template<typename Scalar, std::size_t N>
		Scalar HorizontalMax(const TScalarArray<Scalar, N>& v) noexcept;

		template<typename Scalar1, typename Scalar2, std::size_t N>
		typename TPromote<Scalar1, Scalar2>::RT Dot(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept;

		template<typename Scalar1, typename Scalar2, std::size_t N>
		typename TPromote<Scalar1, Scalar2>::RT AbsDot(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept;


		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Sin(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Cos(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Tan(const TScalarArray<Scalar, N>& v) noexcept;

		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> ASin(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> ACos(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> ATan(const TScalarArray<Scalar, N>& v) noexcept;

		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Square(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Cube(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Abs(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Saturate(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Clamp(const TScalarArray<Scalar, N>& v, Scalar a, Scalar b) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> SmoothStep(const TScalarArray<Scalar, N>& v) noexcept;

		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Exp(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Log(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Log2(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Log10(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Sqrt(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> RSqrt(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Cbrt(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Pow(const TScalarArray<Scalar, N>& base, Scalar exp) noexcept;

		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Min(const TScalarArray<Scalar, N>& a, const TScalarArray<Scalar, N>& b) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Max(const TScalarArray<Scalar, N>& a, const TScalarArray<Scalar, N>& b) noexcept;

		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Ceil(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Floor(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Trunc(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Round(const TScalarArray<Scalar, N>& v) noexcept;
		template<typename Scalar, std::size_t N> TScalarArray<Scalar, N> Frac(const TScalarArray<Scalar, N>& v) noexcept;
	}










	// Member Function

	// --Implementation-- //

	template<typename T, std::size_t N>
	FORCEINLINE constexpr TScalarArray<T, N>::TScalarArray() noexcept
		: mData{}
	{
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE constexpr TScalarArray<Scalar, N>::TScalarArray(FZero) noexcept
	{
		mData.fill(Scalar{ 0 });
	}

	template<typename Scalar, std::size_t N>
	template<std::size_t I>
	FORCEINLINE constexpr TScalarArray<Scalar, N>::TScalarArray(FUnit<I>) noexcept
	{
		ASSERT(GetSize() >= I);

		mData.fill(Scalar{ 0 });
		mData[I] = Scalar{ 1 };
	}

	template<typename Scalar, std::size_t N>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, N>::TScalarArray(const Scalar2* v) noexcept
	{
		ASSERT(v != nullptr);

		for (SizeType i = 0; i < GetSize(); ++i)
		{
			mData[i] = static_cast<Scalar>(v[i]);
		}
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE constexpr TScalarArray<Scalar, N>::TScalarArray(std::initializer_list<Scalar> list) noexcept
	{
		ASSERT(GetSize() >= list.size());

		auto listIter = list.begin();
		auto arrayIter = mData.begin();

		for (; listIter != list.end() && arrayIter != mData.end(); ++listIter, ++arrayIter)
		{
			*arrayIter = *listIter;
		}
	}

	template<typename Scalar, std::size_t N>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, N>::TScalarArray(const TScalarArray<Scalar2, N - 1>& v) noexcept
	{
		for (SizeType i = 0; i < GetSize() - 1; ++i)
		{
			mData[i] = static_cast<Scalar>(v[i]);
		}

		mData[GetSize() - 1] = Scalar{};
	}

	template<typename Scalar, std::size_t N>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, N>::TScalarArray(const TScalarArray<Scalar2, N>& v) noexcept
	{
		for (SizeType i = 0; i < GetSize(); ++i)
		{
			mData[i] = static_cast<Scalar>(v[i]);
		}
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE TScalarArray<Scalar, N>::operator ConstPointer() const noexcept
	{
		return mData.data();
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE TScalarArray<Scalar, N>::operator Pointer() noexcept
	{
		return mData.data();
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator=(FZero) noexcept
	{
		mData.fill(Scalar{});
		return *this;
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator+=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator-=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator*=(FZero) noexcept
	{
		mData.fill(Scalar{});
		return *this;
	}

	template<typename Scalar, std::size_t N>
	template<std::size_t I>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator=(FUnit<I>) noexcept
	{
		mData.fill(Scalar{});
		mData[I] = Scalar{ 1 };
		return *this;
	}

	template<typename Scalar, std::size_t N>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator=(const Scalar2* v) noexcept
	{
		ASSERT(v != nullptr);

		for (SizeType i = 0; i < GetSize(); ++i)
		{
			mData[i] = static_cast<Scalar>(v[i]);
		}

		return *this;
	}

	template<typename Scalar, std::size_t N>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator=(const TScalarArray<Scalar2, N>& v) noexcept
	{
		for (SizeType i = 0; i < GetSize(); ++i)
		{
			mData[i] = static_cast<Scalar>(v[i]);
		}

		return *this;
	}

	template<typename Scalar, std::size_t N>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator+=(const TScalarArray<Scalar2, N>& v) noexcept
	{
		for (SizeType i = 0; i < GetSize(); ++i)
		{
			mData[i] += static_cast<Scalar>(v[i]);
		}

		return *this;
	}

	template<typename Scalar, std::size_t N>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator-=(const TScalarArray<Scalar2, N>& v) noexcept
	{
		for (SizeType i = 0; i < GetSize(); ++i)
		{
			mData[i] -= static_cast<Scalar>(v[i]);
		}

		return *this;
	}

	template<typename Scalar, std::size_t N>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator*=(const TScalarArray<Scalar2, N>& v) noexcept
	{
		for (SizeType i = 0; i < GetSize(); ++i)
		{
			mData[i] *= static_cast<Scalar>(v[i]);
		}

		return *this;
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator*=(Scalar s) noexcept
	{
		for (SizeType i = 0; i < GetSize(); ++i)
		{
			mData[i] *= s;
		}

		return *this;
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE TScalarArray<Scalar, N>& TScalarArray<Scalar, N>::operator/=(Scalar s) noexcept
	{
		ASSERT(!FMath::IsZero(s));

		return *this *= Scalar{ 1 } / s;
	}

	template<typename Scalar, std::size_t N>
	template<typename Scalar2>
	FORCEINLINE void TScalarArray<Scalar, N>::Fill(Scalar2 s) noexcept
	{
		mData.fill(static_cast<Scalar>(s));
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE constexpr typename TScalarArray<Scalar, N>::Iterator TScalarArray<Scalar, N>::Begin() noexcept
	{
		using Iterator = typename TScalarArray<Scalar, N>::Iterator;
		return mData.begin();
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE constexpr typename TScalarArray<Scalar, N>::ConstIterator TScalarArray<Scalar, N>::Begin() const noexcept
	{
		using ConstIterator = typename TScalarArray<Scalar, N>::ConstIterator;
		return mData.begin();
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE constexpr typename TScalarArray<Scalar, N>::Iterator TScalarArray<Scalar, N>::End() noexcept
	{
		using Iterator = typename TScalarArray<Scalar, N>::Iterator;
		return mData.end();
	}

	template<typename Scalar, std::size_t N>
	FORCEINLINE constexpr typename TScalarArray<Scalar, N>::ConstIterator TScalarArray<Scalar, N>::End() const noexcept
	{
		using ConstIterator = typename TScalarArray<Scalar, N>::ConstIterator;
		return mData.end();
	}










	// Non-member Operators 

	// --Implementation-- //

#ifdef USE_OSTREAM

	template<typename CharT, typename Traits, typename Scalar, std::size_t N>
	FORCEINLINE std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TScalarArray<Scalar, N>& v)
	{
		for (std::size_t i = 0; i < N; ++i)
		{
			os << v[i];
			if (i != N - 1)
			{
				os << " ";
			}
		}

		return os;
	}

#endif

#ifdef USE_ISTREAM

	template<typename CharT, typename Traits, typename Scalar, std::size_t N>
	FORCEINLINE std::basic_istream<CharT, Traits>& operator>>(std::basic_ostream<CharT, Traits>& is, const TScalarArray<Scalar, N>& v)
	{
		for (std::size_t i = 0; i < N; ++i)
		{
			is >> v[i];
		}

		return is;
	}

#endif

	template<typename Scalar1, typename Scalar2, std::size_t N>
	FORCEINLINE bool operator==(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept
	{
		for (std::size_t i = 0; i < N; ++i)
		{
			if (v1[i] != v2[i])
			{
				return false;
			}
		}

		return true;
	}

	template<typename Scalar1, typename Scalar2, std::size_t N>
	FORCEINLINE bool operator!=(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept
	{
		return !(v1 == v2);
	}

	template<typename Scalar, std::size_t N>
	TScalarArray<Scalar, N> operator-(const TScalarArray<Scalar, N>& a) noexcept
	{
		TScalarArray<Scalar, N> result;

		for (std::size_t i = 0; i < N; ++i)
		{
			result[i] = -a[i];
		}

		return result;
	}

	template<typename Scalar1, typename Scalar2, std::size_t N>
	FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator+(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		TScalarArray<RT, N> result;

		for (std::size_t i = 0; i < N; ++i)
		{
			result[i] = static_cast<RT>(v1[i] + v2[i]);
		}

		return result;
	}

	template<typename Scalar1, typename Scalar2, std::size_t N>
	FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator-(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		TScalarArray<RT, N> result;

		for (std::size_t i = 0; i < N; ++i)
		{
			result[i] = static_cast<RT>(v1[i] - v2[i]);
		}

		return result;
	}

	template<typename Scalar1, typename Scalar2, std::size_t N>
	FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator*(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		TScalarArray<RT, N> result;

		for (std::size_t i = 0; i < N; ++i)
		{
			result[i] = static_cast<RT>(v1[i] * v2[i]);
		}

		return result;
	}

	template<typename Scalar1, typename Scalar2, std::size_t N>
	FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator*(const TScalarArray<Scalar1, N>& v, Scalar2 s) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		TScalarArray<RT, N> result;

		for (std::size_t i = 0; i < N; ++i)
		{
			result[i] = static_cast<RT>(v[i] * s);
		}

		return result;
	}

	template<typename Scalar1, typename Scalar2, std::size_t N>
	FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator*(Scalar2 s, const TScalarArray<Scalar1, N>& v) noexcept
	{
		return v * s;
	}

	template<typename Scalar1, typename Scalar2, std::size_t N>
	FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> operator/(const TScalarArray<Scalar1, N>& v, Scalar2 s) noexcept
	{
		ASSERT(!FMath::IsZero(s));
		using RT = typename TPromote<Scalar1, Scalar2>::RT;

		return v * RT{ RT{1} / s };
	}
			







	// Non-member Function

	// --Implementation-- //

	namespace FMath
	{
		template<typename Scalar, std::size_t N>
		FORCEINLINE std::size_t MaxDimension(const TScalarArray<Scalar, N>& v) noexcept
		{
			std::size_t maxDimension{};

			for (std::size_t i = 1; i < N; ++i)
			{
				if (v[i] > v[maxDimension])
				{
					maxDimension = i;
				}
			}

			return maxDimension;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE std::size_t MinDimension(const TScalarArray<Scalar, N>& v) noexcept
		{
			std::size_t minDimension{};

			for (std::size_t i = 1; i < N; ++i)
			{
				if (v[i] < v[minDimension])
				{
					minDimension = i;
				}
			}

			return minDimension;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE Scalar HorizontalAdd(const TScalarArray<Scalar, N>& v) noexcept
		{
			Scalar result{};

			for (size_t i = 0; i < N; i++)
			{
				result += v[i];
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE Scalar HorizontalMin(const TScalarArray<Scalar, N>& v) noexcept
		{
			Scalar minComponent = v[0];

			for (size_t i = 1; i < N; i++)
			{
				if (minComponent > v[i])
				{
					minComponent = v[i];
				}
			}

			return minComponent;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE Scalar HorizontalMax(const TScalarArray<Scalar, N>& v) noexcept
		{
			Scalar maxComponent = v[0];

			for (size_t i = 1; i < N; i++)
			{
				if (maxComponent < v[i])
				{
					maxComponent = v[i];
				}
			}

			return maxComponent;
		}

		template<typename Scalar1, typename Scalar2, std::size_t N>
		FORCEINLINE typename TPromote<Scalar1, Scalar2>::RT Dot(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			return HorizontalAdd(v1 * v2);
		}

		template<typename Scalar1, typename Scalar2, std::size_t N>
		FORCEINLINE typename TPromote<Scalar1, Scalar2>::RT AbsDot(const TScalarArray<Scalar1, N>& v1, const TScalarArray<Scalar2, N>& v2) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			return HorizontalAdd(v1 * v2);
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Sin(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Sin(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Cos(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Cos(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Tan(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Tan(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> ASin(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = ASin(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> ACos(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = ACos(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> ATan(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = ATan(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Square(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Square(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Cube(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Cube(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Abs(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Abs(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Saturate(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Saturate(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Clamp(const TScalarArray<Scalar, N>& v, Scalar a, Scalar b) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Clamp(v[i], a, b);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> SmoothStep(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = SmoothStep(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Exp(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Exp(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Log(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Log(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Log2(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Log2(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Log10(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Log10(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Sqrt(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Sqrt(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> RSqrt(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = RSqrt(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Cbrt(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Cbrt(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Pow(const TScalarArray<Scalar, N>& base, Scalar exp) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Pow(base[i], exp);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Min(const TScalarArray<Scalar, N>& a, const TScalarArray<Scalar, N>& b) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Min(a[i], b[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Max(const TScalarArray<Scalar, N>& a, const TScalarArray<Scalar, N>& b) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Max(a[i], b[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Ceil(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Ceil(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Floor(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Floor(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Trunc(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Trunc(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Round(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Round(v[i]);
			}

			return result;
		}

		template<typename Scalar, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Frac(const TScalarArray<Scalar, N>& v) noexcept
		{
			TScalarArray<Scalar, N> result;

			for (size_t i = 0; i < N; i++)
			{
				result[i] = Frac(v[i]);
			}

			return result;
		}
	}
}

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#ifdef USE_SSE
#include "Vector4_SSE.h"
#endif // USE_SSE

