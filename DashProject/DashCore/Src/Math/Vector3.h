#pragma once

namespace Dash
{
	template<typename Scalar>
	struct TScalarArray<Scalar, 3>
	{
	public:
		using DataType = std::array<Scalar, 3>;

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
		constexpr explicit TScalarArray(FIdentity) noexcept;
		template <std::size_t I> constexpr explicit TScalarArray(FUnit<I>) noexcept;
		template <typename Scalar2> constexpr explicit TScalarArray(const TScalarArray<Scalar2, 2>& v, Scalar2 z) noexcept;
		template <typename Scalar2> constexpr explicit TScalarArray(Scalar2 x, Scalar2 y, Scalar2 z) noexcept;
		template <typename Scalar2> constexpr explicit TScalarArray(const Scalar2* v) noexcept;
		template <typename Scalar2> constexpr TScalarArray(const TScalarArray<Scalar2, 3>& v) noexcept;

		operator ConstPointer () const noexcept;
		operator Pointer () noexcept;

		TScalarArray<Scalar, 3>& operator=(FZero) noexcept;
		TScalarArray<Scalar, 3>& operator+=(FZero) noexcept;
		TScalarArray<Scalar, 3>& operator-=(FZero) noexcept;
		TScalarArray<Scalar, 3>& operator*=(FZero) noexcept;

		template<std::size_t I> TScalarArray<Scalar, 3>& operator=(FUnit<I>) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 3>& operator=(const Scalar2* v) noexcept;

		template<typename Scalar2> TScalarArray<Scalar, 3>& operator=(const TScalarArray<Scalar2, 3>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 3>& operator+=(const TScalarArray<Scalar2, 3>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 3>& operator-=(const TScalarArray<Scalar2, 3>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 3>& operator*=(const TScalarArray<Scalar2, 3>& v) noexcept;

		TScalarArray<Scalar, 3>& operator*=(Scalar s) noexcept;
		TScalarArray<Scalar, 3>& operator/=(Scalar s) noexcept;

		template<typename Scalar2> void Fill(Scalar2 s) noexcept;

		constexpr SizeType GetSize() const noexcept { return 2; }

		constexpr Iterator Begin() noexcept;
		constexpr ConstIterator Begin() const noexcept;

		constexpr Iterator End() noexcept;
		constexpr ConstIterator End() const noexcept;

		union
		{
			struct { Scalar X, Y, Z; };
			DataType Data;
			TScalarArray<Scalar, 2> XY;
		};
	};








	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		template <typename Scalar1, typename Scalar2>
		typename TPromote<Scalar1, Scalar2>::RT Dot(const TScalarArray<Scalar1, 3>& a, const TScalarArray<Scalar2, 3>& b) noexcept;

		template <typename Scalar1, typename Scalar2>
		TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 3> Cross(const TScalarArray<Scalar1, 3>& a, const TScalarArray<Scalar2, 3>& b) noexcept;

		template <typename Scalar> TScalarArray<Scalar, 3> Abs(const TScalarArray<Scalar, 3>& a) noexcept;

		template <typename Scalar>
		Scalar Triple(const TScalarArray<Scalar, 3>& a, const TScalarArray<Scalar, 3>& b, const TScalarArray<Scalar, 3>& c) noexcept;

		template <typename Scalar>
		TScalarArray<Scalar, 3> Normal(const TScalarArray<Scalar, 3>& a, const TScalarArray<Scalar, 3>& b, const TScalarArray<Scalar, 3>& c) noexcept;

		template <typename Scalar> std::size_t MaxAxis(const TScalarArray<Scalar, 3>& a) noexcept;
		template <typename Scalar> std::size_t MinAxis(const TScalarArray<Scalar, 3>& a) noexcept;

		template <typename Scalar> std::size_t ClosestAxis(const TScalarArray<Scalar, 3>& a) noexcept;
		template <typename Scalar> std::size_t FurthestAxis(const TScalarArray<Scalar, 3>& a) noexcept;

		template <typename Scalar1, typename Scalar2>
		bool Dominates(const TScalarArray<Scalar1, 3>& a, const TScalarArray<Scalar2, 3>& b) noexcept;

		template <typename Scalar1, typename Scalar2>
		void Convert(Scalar1* v, const TScalarArray<Scalar2, 3>& a) noexcept;

		template <typename Scalar> bool Isfinite(const TScalarArray<Scalar, 3>& a) noexcept;

		template <typename Scalar> constexpr TScalarArray<Scalar, 3> Min(const TScalarArray<Scalar, 3> a, const TScalarArray<Scalar, 3> b) noexcept;
		template <typename Scalar> constexpr TScalarArray<Scalar, 3> Max(const TScalarArray<Scalar, 3> a, const TScalarArray<Scalar, 3> b) noexcept;
	}









	// Member Function

	// --Implementation-- //

	template<typename Scalar>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray() noexcept
		: X()
		, Y()
		, Z()
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(FZero) noexcept
		: X(Scalar{})
		, Y(Scalar{})
		, Z(Scalar{})
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(FIdentity) noexcept
		: X(Scalar{ 1 })
		, Y(Scalar{ 1 })
		, Z(Scalar{ 1 })
	{
	}

	template<typename Scalar>
	template <std::size_t I>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(FUnit<I>) noexcept
		: X(Scalar{ I == 0 })
		, Y(Scalar{ I == 1 })
		, Z(Scalar{ I == 2 })
	{
		STATIC_ASSERT(I < 3);
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(const TScalarArray<Scalar2, 2>& v, Scalar2 z) noexcept
		: X(static_cast<Scalar>(v.X))
		, Y(static_cast<Scalar>(v.Y))
		, Z(static_cast<Scalar>(z))
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(Scalar2 x, Scalar2 y, Scalar2 z) noexcept
		: X(static_cast<Scalar>(x))
		, Y(static_cast<Scalar>(y))
		, Z(static_cast<Scalar>(z))
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(const Scalar2* v) noexcept
		: X(Scalar{ v[0] })
		, Y(Scalar{ v[1] })
		, Z(Scalar{ v[2] })
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(const TScalarArray<Scalar2, 3>& v) noexcept
		: X(Scalar{ v.X })
		, Y(Scalar{ v.Y })
		, Z(Scalar{ v.Z })
	{
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 3>::operator ConstPointer() const noexcept
	{
		return Data.data();
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 3>::operator Pointer() noexcept
	{
		return Data.data();
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator=(FZero) noexcept
	{
		X = Scalar{};
		Y = Scalar{};
		Z = Scalar{};

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator+=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator-=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator*=(FZero) noexcept
	{
		X = Scalar{};
		Y = Scalar{};
		Z = Scalar{};

		return *this;
	}

	template<typename Scalar>
	template<std::size_t I>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator=(FUnit<I>) noexcept
	{
		X = Scalar{ I == 0 };
		Y = Scalar{ I == 1 };
		Z = Scalar{ I == 2 };

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator=(const Scalar2* v) noexcept
	{
		ASSERT(v != nullptr);

		X = v[0];
		Y = v[1];
		Z = v[2];

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator=(const TScalarArray<Scalar2, 3>& v) noexcept
	{
		ASSERT(v != nullptr);

		X = v.X;
		Y = v.Y;
		Z = v.Z;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator+=(const TScalarArray<Scalar2, 3>& v) noexcept
	{
		ASSERT(v != nullptr);

		X += v.X;
		Y += v.Y;
		Z += v.Z;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator-=(const TScalarArray<Scalar2, 3>& v) noexcept
	{
		ASSERT(v != nullptr);

		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator*=(const TScalarArray<Scalar2, 3>& v) noexcept
	{
		ASSERT(v != nullptr);

		X *= v.X;
		Y *= v.Y;
		Z *= v.Z;

		return *this;
	}


	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator*=(Scalar s) noexcept
	{
		X *= s;
		Y *= s;
		Z *= s;

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator/=(Scalar s) noexcept
	{
		ASSERT(!FMath::IsZero(s));

		return *this *= (Scalar{ 1 } / s);
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE void TScalarArray<Scalar, 3>::Fill(Scalar2 s) noexcept
	{
		Data.fill(static_cast<Scalar>(s));
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 3>::Iterator TScalarArray<Scalar, 3>::Begin() noexcept
	{
		return Data.begin();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 3>::ConstIterator TScalarArray<Scalar, 3>::Begin() const noexcept
	{
		return Data.begin();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 3>::Iterator TScalarArray<Scalar, 3>::End() noexcept
	{
		return Data.end();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 3>::ConstIterator TScalarArray<Scalar, 3>::End() const noexcept
	{
		return Data.end();
	}








	// Non-member Function

	// --Implementation-- //

	namespace FMath
	{
		template<typename Scalar1, typename Scalar2>
		FORCEINLINE typename TPromote<Scalar1, Scalar2>::RT Dot(const TScalarArray<Scalar1, 3>& a, const TScalarArray<Scalar2, 3>& b) noexcept
		{
			return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 3> Cross(const TScalarArray<Scalar1, 3>& a, const TScalarArray<Scalar2, 3>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;

			typedef typename TPromote<Scalar1, Scalar2>::RT RT;
			return TScalarArray<RT, 3>{a.Y* b.Z - a.Z * b.Y,
				a.Z* b.X - a.X * b.Z,
				a.X* b.Y - a.Y * b.X};
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 3> Abs(const TScalarArray<Scalar, 3>& a) noexcept
		{
			return TScalarArray<Scalar, 3>{ Abs(a.X), Abs(a.Y), Abs(a.Z) };
		}

		template<typename Scalar>
		Scalar Triple(const TScalarArray<Scalar, 3>& a, const TScalarArray<Scalar, 3>& b, const TScalarArray<Scalar, 3>& c) noexcept
		{
#if SPEED_OVER_ACCURACY
			return Dot(a, Cross(b, c));
#else
			TScalarArray<Scalar, 3> e[3];

			e[0] = a;
			e[1] = b;
			e[2] = c;

			TScalarArray<Scalar, 3> d{ Dot(e[0], e[0]),
				Dot(e[1], e[1]),
				Dot(e[2], e[2]) };

			int axis = MaxAxis(d);

			return Dot(e[axis], Cross(e[(axis + 1) % 3], e[(axis + 2) % 3]));
#endif
		}

		template<typename Scalar>
		TScalarArray<Scalar, 3> Normal(const TScalarArray<Scalar, 3>& a, const TScalarArray<Scalar, 3>& b, const TScalarArray<Scalar, 3>& c) noexcept
		{
#if SPEED_OVER_ACCURACY
			return Cross(b - a, c - b);
#else
			TScalarArray<Scalar, 3> e[3];

			e[0] = b - a;
			e[1] = c - b;
			e[2] = a - c;

			TScalarArray<Scalar, 3> d{ Dot(e[0], e[0]),
				Dot(e[1], e[1]),
				Dot(e[2], e[2]) };

			int axis = MaxAxis(d);

			return Cross(e[(axis + 1) % 3], e[(axis + 2) % 3]);
#endif
		}

		template<typename Scalar>
		FORCEINLINE std::size_t MaxAxis(const TScalarArray<Scalar, 3>& a) noexcept
		{
			int c0 = IsLessnn(a.X, a.Y);
			int c1 = IsLessnn(a.X, a.Z);
			int c2 = IsLessnn(a.Y, a.Z);
			return static_cast<size_t>((c0 & ~c2) | ((c1 & c2) << 1));
		}

		template<typename Scalar>
		FORCEINLINE std::size_t MinAxis(const TScalarArray<Scalar, 3>& a) noexcept
		{
			int c0 = IsLessnn(a.Y, a.X);
			int c1 = IsLessnn(a.Z, a.X);
			int c2 = IsLessnn(a.Z, a.Y);
			return static_cast<size_t>((c0 & ~c2) | ((c1 & c2) << 1));
		}

		template<typename Scalar>
		FORCEINLINE std::size_t ClosestAxis(const TScalarArray<Scalar, 3>& a) noexcept
		{
			return MaxAxis(a * a);
		}

		template<typename Scalar>
		FORCEINLINE std::size_t FurthestAxis(const TScalarArray<Scalar, 3>& a) noexcept
		{
			return MinAxis(a * a);
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE bool Dominates(const TScalarArray<Scalar1, 3>& a, const TScalarArray<Scalar2, 3>& b) noexcept
		{
			return !IsLessnn(a.X, b.X) &&
				!IsLessnn(a.Y, b.Y) &&
				!IsLessnn(a.Z, b.Z);
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE void Convert(Scalar1* v, const TScalarArray<Scalar2, 3>& a) noexcept
		{
			ASSERT(v != nullptr);

			v[0] = Scalar1(a.X);
			v[1] = Scalar1(a.Y);
			v[2] = Scalar1(a.Z);
		}

		template<typename Scalar>
		FORCEINLINE bool Isfinite(const TScalarArray<Scalar, 3>& a) noexcept
		{
			return IsFinite(a.X) && IsFinite(a.Y) && IsFinite(a.Z);
		}

		template<typename Scalar>
		FORCEINLINE constexpr TScalarArray<Scalar, 3> Min(const TScalarArray<Scalar, 3> a, const TScalarArray<Scalar, 3> b) noexcept
		{
			return TScalarArray<Scalar, 3>{ Min(a.X, b.X), Min(a.Y, b.Y), Min(a.Z, b.Z) };
		}

		template<typename Scalar>
		FORCEINLINE constexpr TScalarArray<Scalar, 3> Max(const TScalarArray<Scalar, 3> a, const TScalarArray<Scalar, 3> b) noexcept
		{
			return TScalarArray<Scalar, 3>{ Max(a.X, b.X), Max(a.Y, b.Y), Max(a.Z, b.Z) };
		}
	}
}