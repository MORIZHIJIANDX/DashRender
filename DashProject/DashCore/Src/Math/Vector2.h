#pragma once

namespace Dash
{
	template<typename Scalar>
	struct TScalarArray<Scalar, 2>
	{
	public:
		using DataType = std::array<Scalar, 2>;

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
		template <typename Scalar2> constexpr explicit TScalarArray(Scalar2 x, Scalar2 y) noexcept;
		template <typename Scalar2> constexpr explicit TScalarArray(const Scalar2* v) noexcept;
		template <typename Scalar2> constexpr TScalarArray(const TScalarArray<Scalar2, 2>& v) noexcept;

		operator ConstPointer () const noexcept;
		operator Pointer () noexcept;

		TScalarArray<Scalar, 2>& operator=(FZero) noexcept;
		TScalarArray<Scalar, 2>& operator+=(FZero) noexcept;
		TScalarArray<Scalar, 2>& operator-=(FZero) noexcept;
		TScalarArray<Scalar, 2>& operator*=(FZero) noexcept;

		template<std::size_t I> TScalarArray<Scalar, 2>& operator=(FUnit<I>) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 2>& operator=(const Scalar2* v) noexcept;

		template<typename Scalar2> TScalarArray<Scalar, 2>& operator=(const TScalarArray<Scalar2, 2>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 2>& operator+=(const TScalarArray<Scalar2, 2>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 2>& operator-=(const TScalarArray<Scalar2, 2>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 2>& operator*=(const TScalarArray<Scalar2, 2>& v) noexcept;

		TScalarArray<Scalar, 2>& operator*=(Scalar s) noexcept;
		TScalarArray<Scalar, 2>& operator/=(Scalar s) noexcept;

		template<typename Scalar2> void Fill(Scalar2 s) noexcept;

		constexpr SizeType GetSize() const noexcept { return 2; }

		constexpr Iterator Begin() noexcept;
		constexpr ConstIterator Begin() const noexcept;

		constexpr Iterator End() noexcept;
		constexpr ConstIterator End() const noexcept;

		union
		{
			struct { Scalar U, V; };
			struct { Scalar X, Y; };
			DataType Data{};
		};
	};






	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		template <typename Scalar1, typename Scalar2>
		typename TPromote<Scalar1, Scalar2>::RT Dot(const TScalarArray<Scalar1, 2>& a, const TScalarArray<Scalar2, 2>& b) noexcept;

		template <typename Scalar1, typename Scalar2>
		typename TPromote<Scalar1, Scalar2>::RT PerpDot(const TScalarArray<Scalar1, 2>& a, const TScalarArray<Scalar2, 2>& b) noexcept;

		template <typename Scalar> TScalarArray<Scalar, 2> Abs(const TScalarArray<Scalar, 2>& a) noexcept;
		template <typename Scalar> TScalarArray<Scalar, 2> Perp(const TScalarArray<Scalar, 2>& a) noexcept;

		template <typename Scalar> std::size_t MaxAxis(const TScalarArray<Scalar, 2>& a) noexcept;
		template <typename Scalar> std::size_t MinAxis(const TScalarArray<Scalar, 2>& a) noexcept;

		template <typename Scalar> std::size_t ClosestAxis(const TScalarArray<Scalar, 2>& a) noexcept;
		template <typename Scalar> std::size_t FurthestAxis(const TScalarArray<Scalar, 2>& a) noexcept;

		template <typename Scalar1, typename Scalar2>
		bool Dominates(const TScalarArray<Scalar1, 2>& a, const TScalarArray<Scalar2, 2>& b) noexcept;

		template <typename Scalar1, typename Scalar2>
		void Convert(Scalar1* v, const TScalarArray<Scalar2, 2>& a) noexcept;

		template <typename Scalar> bool Isfinite(const TScalarArray<Scalar, 2>& a) noexcept;

		template <typename Scalar> constexpr TScalarArray<Scalar, 2> Min(const TScalarArray<Scalar, 2> a, const TScalarArray<Scalar, 2> b) noexcept;
		template <typename Scalar> constexpr TScalarArray<Scalar, 2> Max(const TScalarArray<Scalar, 2> a, const TScalarArray<Scalar, 2> b) noexcept;
	}








	// Member Function

	// --Implementation-- //

	template<typename Scalar>
	FORCEINLINE constexpr TScalarArray<Scalar, 2>::TScalarArray() noexcept
		: X()
		, Y()
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarArray<Scalar, 2>::TScalarArray(FZero) noexcept
		: X(0)
		, Y(0)
	{
	}

	template<typename Scalar>
	template <std::size_t I>
	FORCEINLINE constexpr TScalarArray<Scalar, 2>::TScalarArray(FUnit<I>) noexcept
		: X(Scalar{ I == 0 })
		, Y(Scalar{ I == 1 })
	{

	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 2>::TScalarArray(Scalar2 x, Scalar2 y) noexcept
		: X(static_cast<Scalar>(x))
		, Y(static_cast<Scalar>(y))
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 2>::TScalarArray(const Scalar2* v) noexcept
		: X(Scalar{ v[0] })
		, Y(Scalar{ v[1] })
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 2>::TScalarArray(const TScalarArray<Scalar2, 2>& v) noexcept
		: X(Scalar{ v.X })
		, Y(Scalar{ v.Y })
	{
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 2>::operator ConstPointer() const noexcept
	{
		return Data.data();
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 2>::operator Pointer() noexcept
	{
		return Data.data();
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator=(FZero) noexcept
	{
		X = Scalar{};
		Y = Scalar{};

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator+=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator-=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator*=(FZero) noexcept
	{
		X = Scalar{};
		Y = Scalar{};

		return *this;
	}

	template<typename Scalar>
	template<std::size_t I>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator=(FUnit<I>) noexcept
	{
		X = Scalar{ I == 0 };
		Y = Scalar{ I == 1 };

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator=(const Scalar2* v) noexcept
	{
		ASSERT(v != nullptr);

		X = v[0];
		Y = v[1];

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator=(const TScalarArray<Scalar2, 2>& v) noexcept
	{
		ASSERT(v != nullptr);

		X = v.X;
		Y = v.Y;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator+=(const TScalarArray<Scalar2, 2>& v) noexcept
	{
		ASSERT(v != nullptr);

		X += v.X;
		Y += v.Y;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator-=(const TScalarArray<Scalar2, 2>& v) noexcept
	{
		ASSERT(v != nullptr);

		X -= v.X;
		Y -= v.Y;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator*=(const TScalarArray<Scalar2, 2>& v) noexcept
	{
		ASSERT(v != nullptr);

		X *= v.X;
		Y *= v.Y;

		return *this;
	}


	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator*=(Scalar s) noexcept
	{
		X *= s;
		Y *= s;

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 2>& TScalarArray<Scalar, 2>::operator/=(Scalar s) noexcept
	{
		ASSERT(!FMath::IsZero(s));

		return *this *= (Scalar{ 1 } / s);
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE void TScalarArray<Scalar, 2>::Fill(Scalar2 s) noexcept
	{
		Data.fill(static_cast<Scalar>(s));
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 2>::Iterator TScalarArray<Scalar, 2>::Begin() noexcept
	{
		using Iterator = typename TScalarArray<Scalar, 2>::Iterator;
		return Data.begin();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 2>::ConstIterator TScalarArray<Scalar, 2>::Begin() const noexcept
	{
		using ConstIterator = typename TScalarArray<Scalar, 2>::ConstIterator;
		return Data.begin();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 2>::Iterator TScalarArray<Scalar, 2>::End() noexcept
	{
		using Iterator = typename TScalarArray<Scalar, 2>::Iterator;
		return Data.end();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 2>::ConstIterator TScalarArray<Scalar, 2>::End() const noexcept
	{
		using ConstIterator = typename TScalarArray<Scalar, 2>::ConstIterator;
		return Data.end();
	}









	// Non-member Function

	// --Implementation-- //

	namespace FMath
	{
		template<typename Scalar1, typename Scalar2>
		FORCEINLINE typename TPromote<Scalar1, Scalar2>::RT Dot(const TScalarArray<Scalar1, 2>& a, const TScalarArray<Scalar2, 2>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;

			return a.X * b.X + a.Y * b.Y;
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE typename TPromote<Scalar1, Scalar2>::RT PerpDot(const TScalarArray<Scalar1, 2>& a, const TScalarArray<Scalar2, 2>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;

			return a.X * b.Y - a.Y * b.X;
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 2> Abs(const TScalarArray<Scalar, 2>& a) noexcept
		{
			return TScalarArray<Scalar, 2>{ Abs(a.X), Abs(a.Y) };
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 2> Perp(const TScalarArray<Scalar, 2>& a) noexcept
		{
			return TScalarArray<Scalar, 2>{ -a.Y, a.X };
		}

		template<typename Scalar>
		FORCEINLINE std::size_t MaxAxis(const TScalarArray<Scalar, 2>& a) noexcept
		{
			return IsLessnn(a.X, a.Y);
		}

		template<typename Scalar>
		FORCEINLINE std::size_t MinAxis(const TScalarArray<Scalar, 2>& a) noexcept
		{
			return IsLessnn(a.Y, a.X);
		}

		template<typename Scalar>
		FORCEINLINE std::size_t ClosestAxis(const TScalarArray<Scalar, 2>& a) noexcept
		{
			return MaxAxis(a * a);
		}

		template<typename Scalar>
		FORCEINLINE std::size_t FurthestAxis(const TScalarArray<Scalar, 2>& a) noexcept
		{
			return MinAxis(a * a);
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE bool Dominates(const TScalarArray<Scalar1, 2>& a, const TScalarArray<Scalar2, 2>& b) noexcept
		{
			return !IsLessnn(a.X, a.X) && !IsLessnn(a.Y, a.Y);
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE void Convert(Scalar1* v, const TScalarArray<Scalar2, 2>& a) noexcept
		{
			ASSERT(v != nullptr);

			v[0] = a.X;
			v[1] = a.Y;
		}

		template<typename Scalar>
		FORCEINLINE bool Isfinite(const TScalarArray<Scalar, 2>& a) noexcept
		{
			return IsFinite(a.X) && IsFinite(a.Y);
		}

		template<typename Scalar>
		FORCEINLINE constexpr TScalarArray<Scalar, 2> Min(const TScalarArray<Scalar, 2> a, const TScalarArray<Scalar, 2> b) noexcept
		{
			return TScalarArray<Scalar, 2>{ Min(a.X, b.X), Min(a.Y, b.Y) };
		}

		template<typename Scalar>
		FORCEINLINE constexpr TScalarArray<Scalar, 2> Max(const TScalarArray<Scalar, 2> a, const TScalarArray<Scalar, 2> b) noexcept
		{
			return TScalarArray<Scalar, 2>{ Max(a.X, b.X), Max(a.Y, b.Y) };
		}
	}
}