#pragma once

namespace Dash
{
	template<typename Scalar>
	class TScalarArray<Scalar, 3>
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
			struct { Scalar x, y, z; };
			DataType Data;
			TScalarArray<Scalar, 2> xy;
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
		: x()
		, y()
		, z()
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(FZero) noexcept
		: x(Scalar{})
		, y(Scalar{})
		, z(Scalar{})
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(FIdentity) noexcept
		: x(Scalar{ 1 })
		, y(Scalar{ 1 })
		, z(Scalar{ 1 })
	{
	}

	template<typename Scalar>
	template <std::size_t I>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(FUnit<I>) noexcept
		: x(Scalar{ I == 0 })
		, y(Scalar{ I == 1 })
		, z(Scalar{ I == 2 })
	{
		STATIC_ASSERT(I < 3);
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(const TScalarArray<Scalar2, 2>& v, Scalar2 z) noexcept
		: x(static_cast<Scalar>(v.x))
		, y(static_cast<Scalar>(v.y))
		, z(static_cast<Scalar>(z))
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(Scalar2 x, Scalar2 y, Scalar2 z) noexcept
		: x(static_cast<Scalar>(x))
		, y(static_cast<Scalar>(y))
		, z(static_cast<Scalar>(z))
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(const Scalar2* v) noexcept
		: x(Scalar{ v[0] })
		, y(Scalar{ v[1] })
		, z(Scalar{ v[2] })
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 3>::TScalarArray(const TScalarArray<Scalar2, 3>& v) noexcept
		: x(Scalar{ v.x })
		, y(Scalar{ v.y })
		, z(Scalar{ v.z })
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
		x = Scalar{};
		y = Scalar{};
		z = Scalar{};

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
		x = Scalar{};
		y = Scalar{};
		z = Scalar{};

		return *this;
	}

	template<typename Scalar>
	template<std::size_t I>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator=(FUnit<I>) noexcept
	{
		x = Scalar{ I == 0 };
		y = Scalar{ I == 1 };
		z = Scalar{ I == 2 };

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator=(const Scalar2* v) noexcept
	{
		ASSERT(v != nullptr);

		x = v[0];
		y = v[1];
		z = v[2];

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator=(const TScalarArray<Scalar2, 3>& v) noexcept
	{
		ASSERT(v != nullptr);

		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator+=(const TScalarArray<Scalar2, 3>& v) noexcept
	{
		ASSERT(v != nullptr);

		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator-=(const TScalarArray<Scalar2, 3>& v) noexcept
	{
		ASSERT(v != nullptr);

		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator*=(const TScalarArray<Scalar2, 3>& v) noexcept
	{
		ASSERT(v != nullptr);

		x *= v.x;
		y *= v.y;
		z *= v.z;

		return *this;
	}


	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 3>& TScalarArray<Scalar, 3>::operator*=(Scalar s) noexcept
	{
		x *= s;
		y *= s;
		z *= s;

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
		using Iterator = typename TScalarArray<Scalar, 3>::Iterator;
		return Data.begin();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 3>::ConstIterator TScalarArray<Scalar, 3>::Begin() const noexcept
	{
		using ConstIterator = typename TScalarArray<Scalar, 3>::ConstIterator;
		return Data.begin();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 3>::Iterator TScalarArray<Scalar, 3>::End() noexcept
	{
		using Iterator = typename TScalarArray<Scalar, 3>::Iterator;
		return Data.end();
	}

	template<typename Scalar>
	FORCEINLINE constexpr typename TScalarArray<Scalar, 3>::ConstIterator TScalarArray<Scalar, 3>::End() const noexcept
	{
		using ConstIterator = typename TScalarArray<Scalar, 3>::ConstIterator;
		return Data.end();
	}








	// Non-member Function

	// --Implementation-- //

	namespace FMath
	{
		template<typename Scalar1, typename Scalar2>
		FORCEINLINE typename TPromote<Scalar1, Scalar2>::RT Dot(const TScalarArray<Scalar1, 3>& a, const TScalarArray<Scalar2, 3>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;

			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 3> Cross(const TScalarArray<Scalar1, 3>& a, const TScalarArray<Scalar2, 3>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;

			typedef typename TPromote<Scalar1, Scalar2>::RT RT;
			return TScalarArray<RT, 3>{a.y* b.z - a.z * b.y,
				a.z* b.x - a.x * b.z,
				a.x* b.y - a.y * b.x};
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 3> Abs(const TScalarArray<Scalar, 3>& a) noexcept
		{
			return TScalarArray<Scalar, 3>{ Abs(a.x), Abs(a.y), Abs(a.z) };
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
			int c0 = IsLessnn(a.x, a.y);
			int c1 = IsLessnn(a.x, a.z);
			int c2 = IsLessnn(a.y, a.z);
			return (c0 & ~c2) | ((c1 & c2) << 1);
		}

		template<typename Scalar>
		FORCEINLINE std::size_t MinAxis(const TScalarArray<Scalar, 3>& a) noexcept
		{
			int c0 = IsLessnn(a.y, a.x);
			int c1 = IsLessnn(a.z, a.x);
			int c2 = IsLessnn(a.z, a.y);
			return (c0 & ~c2) | ((c1 & c2) << 1);
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
			return !IsLessnn(a.x, b.x) &&
				!IsLessnn(a.y, b.y) &&
				!IsLessnn(a.z, b.z);
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE void Convert(Scalar1* v, const TScalarArray<Scalar2, 3>& a) noexcept
		{
			ASSERT(v != nullptr);

			v[0] = Scalar1(a.x);
			v[1] = Scalar1(a.y);
			v[2] = Scalar1(a.z);
		}

		template<typename Scalar>
		FORCEINLINE bool Isfinite(const TScalarArray<Scalar, 3>& a) noexcept
		{
			return IsFinite(a.x) && IsFinite(a.y) && IsFinite(a.z);
		}

		template<typename Scalar>
		FORCEINLINE constexpr TScalarArray<Scalar, 3> Min(const TScalarArray<Scalar, 3> a, const TScalarArray<Scalar, 3> b) noexcept
		{
			return TScalarArray<Scalar, 3>{ Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z) };
		}

		template<typename Scalar>
		FORCEINLINE constexpr TScalarArray<Scalar, 3> Max(const TScalarArray<Scalar, 3> a, const TScalarArray<Scalar, 3> b) noexcept
		{
			return TScalarArray<Scalar, 3>{ Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z) };
		}
	}
}