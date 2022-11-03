#pragma once

namespace Dash
{
	template<typename Scalar>
	struct TScalarMatrix<Scalar, 3, 3>
	{
	public:
		using DataType = TScalarArray<Scalar, 3>;

		using ScalarType = Scalar;
		using SizeType = std::size_t;

		constexpr TScalarMatrix() noexcept;
		constexpr explicit TScalarMatrix(FZero) noexcept;
		constexpr explicit TScalarMatrix(FIdentity) noexcept;
		constexpr explicit TScalarMatrix(Scalar s) noexcept;
		template <typename Scalar2> constexpr explicit TScalarMatrix(const Scalar2* v) noexcept;
		constexpr TScalarMatrix(Scalar a00, Scalar a01, Scalar a02,
			Scalar a10, Scalar a11, Scalar a12,
			Scalar a20, Scalar a21, Scalar a22) noexcept;
		constexpr explicit TScalarMatrix(const TScalarArray<Scalar, 3>& r0,
			const TScalarArray<Scalar, 3>& r1,
			const TScalarArray<Scalar, 3>& r2) noexcept;

		operator const TScalarArray<Scalar, 3>* () const noexcept;
		operator TScalarArray<Scalar, 3>* () noexcept;

		TScalarMatrix<Scalar, 3, 3>& operator=(FZero) noexcept;
		TScalarMatrix<Scalar, 3, 3>& operator+=(FZero) noexcept;
		TScalarMatrix<Scalar, 3, 3>& operator-=(FZero) noexcept;
		TScalarMatrix<Scalar, 3, 3>& operator*=(FZero) noexcept;

		TScalarMatrix<Scalar, 3, 3>& operator=(FIdentity) noexcept;

		TScalarMatrix<Scalar, 3, 3>& operator=(Scalar s) noexcept;
		TScalarMatrix<Scalar, 3, 3>& operator*=(Scalar s) noexcept;
		TScalarMatrix<Scalar, 3, 3>& operator/=(Scalar s) noexcept;

		TScalarMatrix<Scalar, 3, 3>& operator+=(const TScalarMatrix<Scalar, 3, 3>& a) noexcept;
		TScalarMatrix<Scalar, 3, 3>& operator-=(const TScalarMatrix<Scalar, 3, 3>& a) noexcept;

		void SetValue(Scalar a00, Scalar a01, Scalar a02,
			Scalar a10, Scalar a11, Scalar a12,
			Scalar a20, Scalar a21, Scalar a22) noexcept;

		template<typename Scalar2>
		void SetValue(const Scalar2* v) noexcept;

		void SetRows(const TScalarArray<Scalar, 3>& r0,
			const TScalarArray<Scalar, 3>& r1,
			const TScalarArray<Scalar, 3>& r2) noexcept;

		void SetRow(int i, const TScalarArray<Scalar, 3>& v) noexcept;

		void SetColumns(const TScalarArray<Scalar, 3>& c0,
			const TScalarArray<Scalar, 3>& c1,
			const TScalarArray<Scalar, 3>& c2) noexcept;

		void SetColumn(int j, const TScalarArray<Scalar, 3>& v) noexcept;

	private:
		TScalarArray<Scalar, 3> mRows[3];
	};







	// Non-member Operators 

	// --Declaration-- //

#ifdef USE_OSTREAM

	template <typename CharT, typename Traits, typename Scalar>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TScalarMatrix<Scalar, 3, 3>& a);

#endif

#ifdef USE_ISTREAM

	template <typename CharT, typename Traits, typename Scalar>
	std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, TScalarMatrix<Scalar, 3, 3>& a);

#endif

	template <typename Scalar> bool operator==(const TScalarMatrix<Scalar, 3, 3>& a, const TScalarMatrix<Scalar, 3, 3>& b) noexcept;

	template <typename Scalar> TScalarMatrix<Scalar, 3, 3> operator-(const TScalarMatrix<Scalar, 3, 3>& a) noexcept;

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator+(const TScalarMatrix<Scalar1, 3, 3>& a, const TScalarMatrix<Scalar2, 3, 3>& b) noexcept;

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator-(const TScalarMatrix<Scalar1, 3, 3>& a, const TScalarMatrix<Scalar2, 3, 3>& b) noexcept;

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator*(const TScalarMatrix<Scalar1, 3, 3>& a, const TScalarMatrix<Scalar2, 3, 3>& b);

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator*(const TScalarMatrix<Scalar1, 3, 3>& a, Scalar2 s) noexcept;

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator*(Scalar1 s, const TScalarMatrix<Scalar2, 3, 3>& a) noexcept;

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator/(const TScalarMatrix<Scalar1, 3, 3>& a, Scalar2 s) noexcept;










	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		template <typename Scalar1, typename Scalar2>
		TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 3> Mul(const TScalarMatrix<Scalar1, 3, 3>& a, const TScalarArray<Scalar2, 3>& v) noexcept;

		template <typename Scalar1, typename Scalar2>
		TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 3> Mul(const TScalarArray<Scalar1, 3>& v, const TScalarMatrix<Scalar2, 3, 3>& a) noexcept;

		template<typename Scalar1, typename Scalar2>
		TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> Mul(const TScalarMatrix<Scalar1, 3, 3>& a, const TScalarMatrix<Scalar2, 3, 3>& b) noexcept;

		template <typename Scalar>
		TScalarArray<Scalar, 3> Row(const TScalarMatrix<Scalar, 3, 3>& a, std::size_t i) noexcept;

		template <typename Scalar>
		TScalarArray<Scalar, 3> Column(const TScalarMatrix<Scalar, 3, 3>& a, std::size_t i) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 3, 3> Transpose(const TScalarMatrix<Scalar, 3, 3>& a) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 3, 3> Determinate(const TScalarMatrix<Scalar, 3, 3>& a) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 3, 3> Inverse(const TScalarMatrix<Scalar, 3, 3>& a) noexcept;

		template <typename Scalar> Scalar TDot(const TScalarMatrix<Scalar, 3, 3>& a, int i, const TScalarArray<Scalar, 3>& v) noexcept;

		template <int I, int J, typename Scalar> Scalar Cofactor(const TScalarMatrix<Scalar, 3, 3>& a) noexcept;

		template <typename Scalar>
		Scalar Trace(const TScalarMatrix<Scalar, 3, 3>& a) noexcept;
	}









	// Member Function

	// --Implementation-- //

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 3, 3>::TScalarMatrix() noexcept
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 3, 3>::TScalarMatrix(FZero) noexcept
	{
		*this = FZero{};
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 3, 3>::TScalarMatrix(FIdentity) noexcept
	{
		*this = FIdentity{};
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 3, 3>::TScalarMatrix(Scalar s) noexcept
	{
		*this = s;
	}

	template<typename Scalar>
	template <typename Scalar2>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 3, 3>::TScalarMatrix(const Scalar2* v) noexcept
	{
		SetValue(v);
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 3, 3>::TScalarMatrix(Scalar a00, Scalar a01, Scalar a02,
		Scalar a10, Scalar a11, Scalar a12,
		Scalar a20, Scalar a21, Scalar a22) noexcept
	{
		SetValue(a00, a01, a02,
			a10, a11, a12,
			a20, a21, a22);
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 3, 3>::TScalarMatrix(const TScalarArray<Scalar, 3>& r0,
		const TScalarArray<Scalar, 3>& r1,
		const TScalarArray<Scalar, 3>& r2) noexcept
	{
		mRows[0] = r0;
		mRows[1] = r1;
		mRows[2] = r2;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>::operator const TScalarArray<Scalar, 3>* () const noexcept
	{
		return mRows;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>::operator TScalarArray<Scalar, 3>* () noexcept
	{
		return mRows;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>& TScalarMatrix<Scalar, 3, 3>::operator=(FZero zero) noexcept
	{
		mRows[0] = zero;
		mRows[1] = zero;
		mRows[2] = zero;

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>& TScalarMatrix<Scalar, 3, 3>::operator+=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>& TScalarMatrix<Scalar, 3, 3>::operator-=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>& TScalarMatrix<Scalar, 3, 3>::operator*=(FZero zero) noexcept
	{
		mRows[0] = zero;
		mRows[1] = zero;
		mRows[2] = zero;

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>& TScalarMatrix<Scalar, 3, 3>::operator=(FIdentity) noexcept
	{
		mRows[0] = FUnit<0>{};
		mRows[1] = FUnit<1>{};
		mRows[2] = FUnit<2>{};

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>& TScalarMatrix<Scalar, 3, 3>::operator=(Scalar s) noexcept
	{
		SetValue(s, Scalar{}, Scalar{},
			Scalar{}, s, Scalar{},
			Scalar{}, Scalar{}, s);

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>& TScalarMatrix<Scalar, 3, 3>::operator*=(Scalar s) noexcept
	{
		mRows[0] *= s;
		mRows[1] *= s;
		mRows[2] *= s;

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>& TScalarMatrix<Scalar, 3, 3>::operator/=(Scalar s) noexcept
	{
		ASSERT(!FMath::IsZero(s));

		return *this *= (Scalar{ 1 } / s);
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>& TScalarMatrix<Scalar, 3, 3>::operator+=(const TScalarMatrix<Scalar, 3, 3>& a) noexcept
	{
		mRows[0] += a[0];
		mRows[1] += a[1];
		mRows[2] += a[2];

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3>& TScalarMatrix<Scalar, 3, 3>::operator-=(const TScalarMatrix<Scalar, 3, 3>& a) noexcept
	{
		mRows[0] -= a[0];
		mRows[1] -= a[1];
		mRows[2] -= a[2];

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 3, 3>::SetValue(Scalar a00, Scalar a01, Scalar a02,
		Scalar a10, Scalar a11, Scalar a12,
		Scalar a20, Scalar a21, Scalar a22) noexcept
	{
		mRows[0] = TScalarArray<Scalar, 3>{ a00, a01, a02 };
		mRows[1] = TScalarArray<Scalar, 3>{ a10, a11, a12 };
		mRows[2] = TScalarArray<Scalar, 3>{ a20, a21, a22 };
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE void TScalarMatrix<Scalar, 3, 3>::SetValue(const Scalar2* v) noexcept
	{
		ASSERT(v != nullptr);

		mRows[0] = TScalarArray<Scalar, 3>{ Scalar{v[0]}, Scalar{v[1]}, Scalar{v[2]} };
		mRows[1] = TScalarArray<Scalar, 3>{ Scalar{v[3]}, Scalar{v[4]}, Scalar{v[5]} };
		mRows[2] = TScalarArray<Scalar, 3>{ Scalar{v[6]}, Scalar{v[7]}, Scalar{v[8]} };
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 3, 3>::SetRows(const TScalarArray<Scalar, 3>& r0, const TScalarArray<Scalar, 3>& r1, const TScalarArray<Scalar, 3>& r2) noexcept
	{
		mRows[0] = r0;
		mRows[1] = r1;
		mRows[2] = r2;
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 3, 3>::SetRow(int i, const TScalarArray<Scalar, 3>& v) noexcept
	{
		mRows[i] = v;
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 3, 3>::SetColumns(const TScalarArray<Scalar, 3>& c0, const TScalarArray<Scalar, 3>& c1, const TScalarArray<Scalar, 3>& c2) noexcept
	{
		mRows[0] = TScalarArray<Scalar, 3>{ c0.X, c1.X, c2.X };
		mRows[1] = TScalarArray<Scalar, 3>{ c0.Y, c1.Y, c2.Y };
		mRows[2] = TScalarArray<Scalar, 3>{ c0.Z, c1.Z, c2.Z };
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 3, 3>::SetColumn(int j, const TScalarArray<Scalar, 3>& v) noexcept
	{
		mRows[0][j] = v.X;
		mRows[1][j] = v.Y;
		mRows[2][j] = v.Z;
	}









	// Non-member Operators 

	// --Implementation-- //


#ifdef USE_OSTREAM

	template <typename CharT, typename Traits, typename Scalar>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TScalarMatrix<Scalar, 3, 3>& a)
	{
		return os << a[0] << ' ' << a[1] << ' ' << a[2];
	}

#endif

#ifdef USE_ISTREAM

	template <typename CharT, typename Traits, typename Scalar>
	std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, TScalarMatrix<Scalar, 3, 3>& a)
	{
		return is >> a[0] >> a[1] >> a[2];
	}

#endif

	template <typename Scalar>
	FORCEINLINE bool operator==(const TScalarMatrix<Scalar, 3, 3>& a, const TScalarMatrix<Scalar, 3, 3>& b) noexcept
	{
		return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
	}

	template <typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 3, 3> operator-(const TScalarMatrix<Scalar, 3, 3>& a) noexcept
	{
		return TScalarMatrix<Scalar, 3, 3>{ -a[0][0], -a[0][1], -a[0][2],
			-a[1][0], -a[1][1], -a[1][2],
			-a[2][0], -a[2][1], -a[2][2] };
	}

	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator+(const TScalarMatrix<Scalar1, 3, 3>& a, const TScalarMatrix<Scalar2, 3, 3>& b) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		return TScalarMatrix<RT, 3, 3>{a[0][0] + b[0][0], a[0][1] + b[0][1], a[0][2] + b[0][2],
			a[1][0] + b[1][0], a[1][1] + b[1][1], a[1][2] + b[1][2],
			a[2][0] + b[2][0], a[2][1] + b[2][1], a[2][2] + b[2][2]};
	}

	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator-(const TScalarMatrix<Scalar1, 3, 3>& a, const TScalarMatrix<Scalar2, 3, 3>& b) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		return TScalarMatrix<RT, 3, 3>{a[0][0] - b[0][0], a[0][1] - b[0][1], a[0][2] - b[0][2],
			a[1][0] - b[1][0], a[1][1] - b[1][1], a[1][2] - b[1][2],
			a[2][0] - b[2][0], a[2][1] - b[2][1], a[2][2] - b[2][2]};
	}

	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator*(const TScalarMatrix<Scalar1, 3, 3>& a, const TScalarMatrix<Scalar2, 3, 3>& b)
	{
		//using RT = typename TPromote<Scalar1, Scalar2>::RT;
		//return TScalarMatrix<RT, 3, 3>{a[0][0] * b[0][0], a[0][1] * b[0][1], a[0][2] * b[0][2],
		//	a[1][0] * b[1][0], a[1][1] * b[1][1], a[1][2] * b[1][2],
		//	a[2][0] * b[2][0], a[2][1] * b[2][1], a[2][2] * b[2][2]};

		return FMath::Mul(a, b);
	}

	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator*(const TScalarMatrix<Scalar1, 3, 3>& a, Scalar2 s) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		return TScalarMatrix<RT, 3, 3>{a[0][0] * s, a[0][1] * s, a[0][2] * s,
			a[1][0] * s, a[1][1] * s, a[1][2] * s,
			a[2][0] * s, a[2][1] * s, a[2][2] * s};
	}

	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator*(Scalar1 s, const TScalarMatrix<Scalar2, 3, 3>& a) noexcept
	{
		typedef typename TPromote<Scalar1, Scalar2>::RT RT;
		return a * s;
	}

	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> operator/(const TScalarMatrix<Scalar1, 3, 3>& a, Scalar2 s) noexcept
	{
		ASSERT(!FMath::IsZero(s));
		return a * (Scalar1{ 1 } / s);
	}








	// Non-member Function

	// --Implementation-- //

	namespace FMath
	{
		template <typename Scalar1, typename Scalar2>
		FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 3> Mul(const TScalarMatrix<Scalar1, 3, 3>& a, const TScalarArray<Scalar2, 3>& v) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			return TScalarArray<RT, 3>{ Dot(a[0], v),
				Dot(a[1], v),
				Dot(a[2], v) };
		}


		template <typename Scalar1, typename Scalar2>
		FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 3> Mul(const TScalarArray<Scalar1, 3>& v, const TScalarMatrix<Scalar2, 3, 3>& a) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			return TScalarArray<RT, 3>(TDot(a, 0, v),
				TDot(a, 1, v),
				TDot(a, 2, v));
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 3, 3> Mul(const TScalarMatrix<Scalar1, 3, 3>& a, const TScalarMatrix<Scalar2, 3, 3>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			return TScalarMatrix<RT, 3, 3>{TDot(b, 0, a[0]), TDot(b, 1, a[0]), TDot(b, 2, a[0]),
				TDot(b, 0, a[1]), TDot(b, 1, a[1]), TDot(b, 2, a[1]),
				TDot(b, 0, a[2]), TDot(b, 1, a[2]), TDot(b, 2, a[2])};
		}

		template <typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 3> Row(const TScalarMatrix<Scalar, 3, 3>& a, std::size_t i) noexcept
		{
			return a[i];
		}

		template <typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 3> Column(const TScalarMatrix<Scalar, 3, 3>& a, std::size_t i) noexcept
		{
			return TScalarArray<Scalar, 3>{ a[0][i], a[1][i], a[2][i] };
		}

		template <typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 3, 3> Transpose(const TScalarMatrix<Scalar, 3, 3>& a) noexcept
		{
			return TScalarMatrix<Scalar, 3, 3>{a[0][0], a[1][0], a[2][0],
				a[0][1], a[1][1], a[2][1],
				a[0][2], a[1][2], a[2][2]};
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 3, 3> Determinate(const TScalarMatrix<Scalar, 3, 3>& a) noexcept
		{
			return Triple(a[0], a[1], a[2]);
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 3, 3> Inverse(const TScalarMatrix<Scalar, 3, 3>& a) noexcept
		{
			TScalarArray<Scalar, 3> co{ Cofactor<0, 0>(a),
				Cofactor<0, 1>(a),
				Cofactor<0, 2>(a) };
			Scalar det = Dot(a[0], co);
			ASSERT(!IsZero(det));
			Scalar s = Scalar(1) / det;
			return TScalarMatrix<Scalar, 3, 3>{co[0] * s, Cofactor<1, 0>(a)* s, Cofactor<2, 0>(a)* s,
				co[1] * s, Cofactor<1, 1>(a)* s, Cofactor<2, 1>(a)* s,
				co[2] * s, Cofactor<1, 2>(a)* s, Cofactor<2, 2>(a)* s};
		}

		template<typename Scalar>
		FORCEINLINE Scalar TDot(const TScalarMatrix<Scalar, 3, 3>& a, int i, const TScalarArray<Scalar, 3>& v) noexcept
		{
			return a[0][i] * v.X + a[1][i] * v.Y + a[2][i] * v.Z;
		}

		template<int I, int J, typename Scalar>
		FORCEINLINE Scalar Cofactor(const TScalarMatrix<Scalar, 3, 3>& a) noexcept
		{
			return a[(I + 1) % 3][(J + 1) % 3] * a[(I + 2) % 3][(J + 2) % 3] -
				a[(I + 2) % 3][(J + 1) % 3] * a[(I + 1) % 3][(J + 2) % 3];
		}

		template<typename Scalar>
		FORCEINLINE Scalar Trace(const TScalarMatrix<Scalar, 3, 3>& a) noexcept
		{
			return a[0][0] + a[1][1] + a[2][2];
		}
	}
}