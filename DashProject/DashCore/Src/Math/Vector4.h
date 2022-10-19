#pragma once

namespace Dash
{
	template<typename Scalar>
	class TScalarArray<Scalar, 4>
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
		template <typename Scalar2> constexpr explicit TScalarArray(Scalar2 x, Scalar2 y, Scalar2 z, Scalar2 w = Scalar2{}) noexcept;
		template <typename Scalar2> constexpr explicit TScalarArray(const TScalarArray<Scalar2, 3> v, Scalar2 w = Scalar2{}) noexcept;
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
			struct { Scalar x, y, z, w; };
			DataType Data;
			TScalarArray<Scalar, 3> xyz;
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
		: x()
		, y()
		, z()
		, w()
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(FZero) noexcept
		: x()
		, y()
		, z()
		, w()
	{
	}

	template<typename Scalar>
	template <std::size_t I>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(FUnit<I>) noexcept
		: x(Scalar{ I == 0 })
		, y(Scalar{ I == 1 })
		, z(Scalar{ I == 2 })
		, w(Scalar{ I == 3 })
	{
		STATIC_ASSERT(I < 4);
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(Scalar2 x, Scalar2 y, Scalar2 z, Scalar2 w) noexcept
		: x(static_cast<Scalar>(x))
		, y(static_cast<Scalar>(y))
		, z(static_cast<Scalar>(z))
		, w(static_cast<Scalar>(w))
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(const TScalarArray<Scalar2, 3> v, Scalar2 w) noexcept
		: x(static_cast<Scalar>(v.x))
		, y(static_cast<Scalar>(v.y))
		, z(static_cast<Scalar>(v.z))
		, w(static_cast<Scalar>(w))
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(const Scalar2* v) noexcept
		: x(Scalar{ v[0] })
		, y(Scalar{ v[1] })
		, z(Scalar{ v[2] })
		, w(Scalar{ v[3] })
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarArray<Scalar, 4>::TScalarArray(const TScalarArray<Scalar2, 4>& v) noexcept
		: x(Scalar{ v.x })
		, y(Scalar{ v.y })
		, z(Scalar{ v.z })
		, w(Scalar{ v.w })
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
		x = Scalar{};
		y = Scalar{};
		z = Scalar{};
		w = Scalar{};

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
		x = Scalar{};
		y = Scalar{};
		z = Scalar{};
		w = Scalar{};

		return *this;
	}

	template<typename Scalar>
	template<std::size_t I>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator=(FUnit<I>) noexcept
	{
		x = Scalar{ I == 0 };
		y = Scalar{ I == 1 };
		z = Scalar{ I == 2 };
		w = Scalar{ I == 3 };

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator=(const Scalar2* v) noexcept
	{
		ASSERT(v != nullptr);

		x = v[0];
		y = v[1];
		z = v[2];
		w = v[3];

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator=(const TScalarArray<Scalar2, 4>& v) noexcept
	{
		ASSERT(v != nullptr);

		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator+=(const TScalarArray<Scalar2, 4>& v) noexcept
	{
		ASSERT(v != nullptr);

		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator-=(const TScalarArray<Scalar2, 4>& v) noexcept
	{
		ASSERT(v != nullptr);

		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator*=(const TScalarArray<Scalar2, 4>& v) noexcept
	{
		ASSERT(v != nullptr);

		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;

		return *this;
	}


	template<typename Scalar>
	FORCEINLINE TScalarArray<Scalar, 4>& TScalarArray<Scalar, 4>::operator*=(Scalar s) noexcept
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;

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

			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}

		template<typename Scalar1, typename Scalar2>
		typename TPromote<Scalar1, Scalar2>::RT Dot3(const TScalarArray<Scalar1, 4>& a, const TScalarArray<Scalar2, 4>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 4> Cross(const TScalarArray<Scalar1, 4>& a, const TScalarArray<Scalar2, 4>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;

			return TScalarArray<RT, 4>{ a.y* b.z - b.y * a.z, a.z* b.x - a.x * b.z, a.x* b.y - a.y * b.x };
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 4> Abs(const TScalarArray<Scalar, 4>& a) noexcept
		{
			return TScalarArray<Scalar, 4>{ Abs(a.x), Abs(a.y), Abs(a.z), Abs(a.w)};
		}

		template<typename Scalar>
		FORCEINLINE Scalar HorizontalAdd3(const TScalarArray<Scalar, 4>& a) noexcept
		{
			return a.x + a.y + a.z;
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE void Convert(Scalar1* v, const TScalarArray<Scalar2, 4>& a) noexcept
		{
			ASSERT(v != nullptr);

			v[0] = Scalar1(a.x);
			v[1] = Scalar1(a.y);
			v[2] = Scalar1(a.z);
			v[3] = Scalar1(a.w);
		}

		template<typename Scalar>
		FORCEINLINE bool Isfinite(const TScalarArray<Scalar, 4>& a) noexcept
		{
			return IsFinite(a.x) && IsFinite(a.y) && IsFinite(a.z) && IsFinite(a.w);
		}

		template<int X, int Y, int Z, int W, typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 4> Swizzle(const TScalarArray<Scalar, 4>& a) noexcept
		{
			return TScalarArray<Scalar, 4>{a[X], a[Y], a[Z], a[W]};
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 4> Homogenize(const TScalarArray<Scalar, 4>& a) noexcept
		{
			ASSERT(!IsZero(a.w));
			Scalar s = Scalar{ 1 } / a.w;
			return TScalarArray<Scalar, 4>{ a.x* s, a.y* s, a.z* s, Scalar{ 1 }};
		}
	}
}