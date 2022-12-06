#pragma once

namespace Dash
{
	template<typename Scalar>
	struct TScalarArray<Scalar, 4>
	{
	public:
		using DataType = std::array<Scalar, 4>;

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
		template <typename Scalar2> constexpr explicit TScalarArray(Scalar2 x, Scalar2 y, Scalar2 z, Scalar2 W = Scalar2{}) noexcept;
		template <typename Scalar2> constexpr explicit TScalarArray(const TScalarArray<Scalar2, 3> v, Scalar2 W = Scalar2{}) noexcept;
		template <typename Scalar2> constexpr explicit TScalarArray(const Scalar2* v) noexcept;
		template <typename Scalar2> constexpr TScalarArray(const TScalarArray<Scalar2, 4>& v) noexcept;

		operator ConstPointer () const noexcept;
		operator Pointer () noexcept;

		TScalarArray<Scalar, 4>& operator=(FZero) noexcept;
		TScalarArray<Scalar, 4>& operator+=(FZero) noexcept;
		TScalarArray<Scalar, 4>& operator-=(FZero) noexcept;
		TScalarArray<Scalar, 4>& operator*=(FZero) noexcept;

		template<std::size_t I> TScalarArray<Scalar, 4>& operator=(FUnit<I>) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 4>& operator=(const Scalar2* v) noexcept;

		template<typename Scalar2> TScalarArray<Scalar, 4>& operator=(const TScalarArray<Scalar2, 4>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 4>& operator+=(const TScalarArray<Scalar2, 4>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 4>& operator-=(const TScalarArray<Scalar2, 4>& v) noexcept;
		template<typename Scalar2> TScalarArray<Scalar, 4>& operator*=(const TScalarArray<Scalar2, 4>& v) noexcept;

		TScalarArray<Scalar, 4>& operator*=(Scalar s) noexcept;
		TScalarArray<Scalar, 4>& operator/=(Scalar s) noexcept;

		template<typename Scalar2> void Fill(Scalar2 s) noexcept;

		constexpr SizeType GetSize() const noexcept { return 2; }

		constexpr Iterator Begin() noexcept;
		constexpr ConstIterator Begin() const noexcept;

		constexpr Iterator End() noexcept;
		constexpr ConstIterator End() const noexcept;

		union
		{
			struct { Scalar X, Y, Z, W; };
			DataType Data;
			TScalarArray<Scalar, 3> XYZ;
		};
	};








	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		template <typename Scalar1, typename Scalar2>
		typename TPromote<Scalar1, Scalar2>::RT Dot(const TScalarArray<Scalar1, 4>& a, const TScalarArray<Scalar2, 4>& b) noexcept;

		template <typename Scalar1, typename Scalar2>
		typename TPromote<Scalar1, Scalar2>::RT Dot3(const TScalarArray<Scalar1, 4>& a, const TScalarArray<Scalar2, 4>& b) noexcept;

		template <typename Scalar1, typename Scalar2>
		TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 4> Cross(const TScalarArray<Scalar1, 4>& a, const TScalarArray<Scalar2, 4>& b) noexcept;

		template <typename Scalar> TScalarArray<Scalar, 4> Abs(const TScalarArray<Scalar, 4>& a) noexcept;

		template <typename Scalar> Scalar HorizontalAdd3(const TScalarArray<Scalar, 4>& a) noexcept;

		template <typename Scalar1, typename Scalar2>
		void Convert(Scalar1* v, const TScalarArray<Scalar2, 4>& a) noexcept;

		template <typename Scalar> bool Isfinite(const TScalarArray<Scalar, 4>& a) noexcept;

		template <int X, int Y, int Z, int W, typename Scalar>
		TScalarArray<Scalar, 4> Swizzle(const TScalarArray<Scalar, 4>& a) noexcept;

		template <typename Scalar>
		TScalarArray<Scalar, 4> Homogenize(const TScalarArray<Scalar, 4>& a) noexcept;
	}










	// Member Function

	// --Implementation-- //

	template<typename Scalar>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray() noexcept
		: X()
		, Y()
		, Z()
		, W()
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(FZero) noexcept
		: X()
		, Y()
		, Z()
		, W()
	{
	}

	template<typename Scalar>
	template <std::size_t I>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(FUnit<I>) noexcept
		: X(Scalar{ I == 0 })
		, Y(Scalar{ I == 1 })
		, Z(Scalar{ I == 2 })
		, W(Scalar{ I == 3 })
	{
		STATIC_ASSERT(I < 4);
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(Scalar2 x, Scalar2 y, Scalar2 z, Scalar2 w) noexcept
		: X(static_cast<Scalar>(x))
		, Y(static_cast<Scalar>(y))
		, Z(static_cast<Scalar>(z))
		, W(static_cast<Scalar>(w))
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(const TScalarArray<Scalar2, 3> v, Scalar2 w) noexcept
		: X(static_cast<Scalar>(v.X))
		, Y(static_cast<Scalar>(v.Y))
		, Z(static_cast<Scalar>(v.Z))
		, W(static_cast<Scalar>(w))
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(const Scalar2* v) noexcept
		: X(Scalar{ v[0] })
		, Y(Scalar{ v[1] })
		, Z(Scalar{ v[2] })
		, W(Scalar{ v[3] })
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(const TScalarArray<Scalar2, 4>& v) noexcept
		: X(Scalar{ v.X })
		, Y(Scalar{ v.Y })
		, Z(Scalar{ v.Z })
		, W(Scalar{ v.W })
	{
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 4>::operator ConstPointer() const noexcept
	{
		return Data.data();
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 4>::operator Pointer() noexcept
	{
		return Data.data();
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator=(FZero) noexcept
	{
		X = Scalar{};
		Y = Scalar{};
		Z = Scalar{};
		W = Scalar{};

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator+=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator-=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator*=(FZero) noexcept
	{
		X = Scalar{};
		Y = Scalar{};
		Z = Scalar{};
		W = Scalar{};

		return *this;
	}

	template<typename Scalar>
	template<std::size_t I>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator=(FUnit<I>) noexcept
	{
		X = Scalar{ I == 0 };
		Y = Scalar{ I == 1 };
		Z = Scalar{ I == 2 };
		W = Scalar{ I == 3 };

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator=(const Scalar2* v) noexcept
	{
		ASSERT(v != nullptr);

		X = v[0];
		Y = v[1];
		Z = v[2];
		W = v[3];

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator=(const TScalarArray<Scalar2, 4>& v) noexcept
	{
		ASSERT(v != nullptr);

		X = v.X;
		Y = v.Y;
		Z = v.Z;
		W = v.W;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator+=(const TScalarArray<Scalar2, 4>& v) noexcept
	{
		ASSERT(v != nullptr);

		X += v.X;
		Y += v.Y;
		Z += v.Z;
		W += v.W;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator-=(const TScalarArray<Scalar2, 4>& v) noexcept
	{
		ASSERT(v != nullptr);

		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;
		W -= v.W;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator*=(const TScalarArray<Scalar2, 4>& v) noexcept
	{
		ASSERT(v != nullptr);

		X *= v.X;
		Y *= v.Y;
		Z *= v.Z;
		W *= v.W;

		return *this;
	}


	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator*=(Scalar s) noexcept
	{
		X *= s;
		Y *= s;
		Z *= s;
		W *= s;

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator/=(Scalar s) noexcept
	{
		ASSERT(!FMath::IsZero(s));

		return *this *= (Scalar{ 1 } / s);
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE void TScalarArray<Scalar, 4>::Fill(Scalar2 s) noexcept
	{
		Data.fill(static_cast<Scalar>(s));
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 4>::Iterator TScalarArray<Scalar, 4>::Begin() noexcept
	{
		using Iterator = typename TScalarArray<Scalar, 4>::Iterator;
		return Data.begin();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 4>::ConstIterator TScalarArray<Scalar, 4>::Begin() const noexcept
	{
		using ConstIterator = typename TScalarArray<Scalar, 4>::ConstIterator;
		return Data.begin();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 4>::Iterator TScalarArray<Scalar, 4>::End() noexcept
	{
		using Iterator = typename TScalarArray<Scalar, 4>::Iterator;
		return Data.end();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 4>::ConstIterator TScalarArray<Scalar, 4>::End() const noexcept
	{
		using ConstIterator = typename TScalarArray<Scalar, 4>::ConstIterator;
		return Data.end();
	}








	// Non-member Function

	// --Implementation-- //

	namespace FMath
	{

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE typename TPromote<Scalar1, Scalar2>::RT Dot(const TScalarArray<Scalar1, 4>& a, const TScalarArray<Scalar2, 4>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;

			return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
		}

		template<typename Scalar1, typename Scalar2>
		typename TPromote<Scalar1, Scalar2>::RT Dot3(const TScalarArray<Scalar1, 4>& a, const TScalarArray<Scalar2, 4>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 4> Cross(const TScalarArray<Scalar1, 4>& a, const TScalarArray<Scalar2, 4>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;

			return TScalarArray<RT, 4>{ a.Y* b.Z - b.Y * a.Z, a.Z* b.X - a.X * b.Z, a.X* b.Y - a.Y * b.X };
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 4> Abs(const TScalarArray<Scalar, 4>& a) noexcept
		{
			return TScalarArray<Scalar, 4>{ Abs(a.X), Abs(a.Y), Abs(a.Z), Abs(a.W)};
		}

		template<typename Scalar>
		FORCEINLINE Scalar HorizontalAdd3(const TScalarArray<Scalar, 4>& a) noexcept
		{
			return a.X + a.Y + a.Z;
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE void Convert(Scalar1* v, const TScalarArray<Scalar2, 4>& a) noexcept
		{
			ASSERT(v != nullptr);

			v[0] = Scalar1(a.X);
			v[1] = Scalar1(a.Y);
			v[2] = Scalar1(a.Z);
			v[3] = Scalar1(a.W);
		}

		template<typename Scalar>
		FORCEINLINE bool Isfinite(const TScalarArray<Scalar, 4>& a) noexcept
		{
			return IsFinite(a.X) && IsFinite(a.Y) && IsFinite(a.Z) && IsFinite(a.W);
		}

		template<int X, int Y, int Z, int W, typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 4> Swizzle(const TScalarArray<Scalar, 4>& a) noexcept
		{
			return TScalarArray<Scalar, 4>{a[X], a[Y], a[Z], a[W]};
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 4> Homogenize(const TScalarArray<Scalar, 4>& a) noexcept
		{
			ASSERT(!IsZero(a.W));
			Scalar s = Scalar{ 1 } / a.W;
			return TScalarArray<Scalar, 4>{ a.X* s, a.Y* s, a.Z* s, Scalar{ 1 }};
		}
	}
}