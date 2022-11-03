#pragma once

#include "../Utility/Assert.h"

namespace Dash
{
	template <typename Scalar>
	struct TScalarQuaternion;

	template<typename Scalar>
	struct TScalarMatrix<Scalar, 4, 4>
	{
	public:
		using DataType = TScalarArray<Scalar, 4>;

		using ScalarType = Scalar;
		using SizeType = std::size_t;

		constexpr TScalarMatrix() noexcept;
		constexpr explicit TScalarMatrix(FZero) noexcept;
		constexpr explicit TScalarMatrix(FIdentity) noexcept;
		constexpr explicit TScalarMatrix(Scalar s) noexcept;
		template <typename Scalar2> constexpr explicit TScalarMatrix(const Scalar2* v) noexcept;
		constexpr TScalarMatrix(Scalar a00, Scalar a01, Scalar a02, Scalar a03,
			Scalar a10, Scalar a11, Scalar a12, Scalar a13,
			Scalar a20, Scalar a21, Scalar a22, Scalar a23,
			Scalar a30, Scalar a31, Scalar a32, Scalar a33) noexcept;
		constexpr explicit TScalarMatrix(const TScalarArray<Scalar, 4>& r0,
			const TScalarArray<Scalar, 4>& r1,
			const TScalarArray<Scalar, 4>& r2,
			const TScalarArray<Scalar, 4>& r3) noexcept;

		operator const TScalarArray<Scalar, 4>* () const noexcept;
		operator TScalarArray<Scalar, 4>* () noexcept;

		TScalarMatrix<Scalar, 4, 4>& operator=(FZero) noexcept;
		TScalarMatrix<Scalar, 4, 4>& operator+=(FZero) noexcept;
		TScalarMatrix<Scalar, 4, 4>& operator-=(FZero) noexcept;
		TScalarMatrix<Scalar, 4, 4>& operator*=(FZero) noexcept;

		TScalarMatrix<Scalar, 4, 4>& operator=(FIdentity) noexcept;

		TScalarMatrix<Scalar, 4, 4>& operator=(Scalar s) noexcept;
		TScalarMatrix<Scalar, 4, 4>& operator*=(Scalar s) noexcept;
		TScalarMatrix<Scalar, 4, 4>& operator/=(Scalar s) noexcept;

		TScalarMatrix<Scalar, 4, 4>& operator+=(const TScalarMatrix<Scalar, 4, 4>& a) noexcept;
		TScalarMatrix<Scalar, 4, 4>& operator-=(const TScalarMatrix<Scalar, 4, 4>& a) noexcept;

		TScalarMatrix<Scalar, 4, 4>& operator*=(TScalarMatrix<Scalar, 4, 4> a) noexcept;

		void SetValue(Scalar a00, Scalar a01, Scalar a02, Scalar a03,
			Scalar a10, Scalar a11, Scalar a12, Scalar a13,
			Scalar a20, Scalar a21, Scalar a22, Scalar a23,
			Scalar a30, Scalar a31, Scalar a32, Scalar a33) noexcept;

		template<typename Scalar2>
		void SetValue(const Scalar2* v) noexcept;

		void SetRows(const TScalarArray<Scalar, 4>& r0,
			const TScalarArray<Scalar, 4>& r1,
			const TScalarArray<Scalar, 4>& r2,
			const TScalarArray<Scalar, 4>& r3) noexcept;

		void SetRow(int i, const TScalarArray<Scalar, 4>& v) noexcept;
		void SetRow(int i, const TScalarArray<Scalar, 3>& v) noexcept;

		void SetColumns(const TScalarArray<Scalar, 4>& c0,
			const TScalarArray<Scalar, 4>& c1,
			const TScalarArray<Scalar, 4>& c2,
			const TScalarArray<Scalar, 4>& c3) noexcept;

		void SetColumn(int j, const TScalarArray<Scalar, 4>& v) noexcept;

		void SetBasis(const TScalarMatrix<Scalar, 3, 3>& basis) noexcept;

		void SetOrigin(const TScalarArray<Scalar, 3>& origin) noexcept;

	private:
		TScalarArray<Scalar, 4> mRows[4];
	};








	// Non-member Operators 

	// --Declaration-- //


#ifdef USE_OSTREAM

	template <typename CharT, typename Traits, typename Scalar>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TScalarMatrix<Scalar, 4, 4>& a);

#endif

#ifdef USE_ISTREAM

	template <typename CharT, typename Traits, typename Scalar>
	std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, TScalarMatrix<Scalar, 4, 4>& a);

#endif

	template <typename Scalar> bool operator==(const TScalarMatrix<Scalar, 4, 4>& a, const TScalarMatrix<Scalar, 4, 4>& b) noexcept;

	template <typename Scalar> TScalarMatrix<Scalar, 4, 4> operator-(const TScalarMatrix<Scalar, 4, 4>& a) noexcept;

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator+(const TScalarMatrix<Scalar1, 4, 4>& a, const TScalarMatrix<Scalar2, 4, 4>& b) noexcept;

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator-(const TScalarMatrix<Scalar1, 4, 4>& a, const TScalarMatrix<Scalar2, 4, 4>& b) noexcept;

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator*(const TScalarMatrix<Scalar1, 4, 4>& a, const TScalarMatrix<Scalar2, 4, 4>& b);

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator*(const TScalarMatrix<Scalar1, 4, 4>& a, Scalar2 s) noexcept;

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator*(Scalar1 s, const TScalarMatrix<Scalar2, 4, 4>& a) noexcept;

	template <typename Scalar1, typename Scalar2>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator/(const TScalarMatrix<Scalar1, 4, 4>& a, Scalar2 s) noexcept;








	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		template <typename Scalar1, typename Scalar2>
		TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 4> Mul(const TScalarMatrix<Scalar1, 4, 4>& a, const TScalarArray<Scalar2, 4>& v) noexcept;

		template <typename Scalar1, typename Scalar2>
		TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 4> Mul(const TScalarArray<Scalar1, 4>& v, const TScalarMatrix<Scalar2, 4, 4>& a) noexcept;

		template<typename Scalar1, typename Scalar2>
		TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> Mul(const TScalarMatrix<Scalar1, 4, 4>& a, const TScalarMatrix<Scalar2, 4, 4>& b) noexcept;

		template <typename Scalar>
		TScalarArray<Scalar, 4> Row(const TScalarMatrix<Scalar, 4, 4>& a, std::size_t i) noexcept;

		template <typename Scalar>
		TScalarArray<Scalar, 4> Column(const TScalarMatrix<Scalar, 4, 4>& a, std::size_t i) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> Transpose(const TScalarMatrix<Scalar, 4, 4>& a) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> Determinate(const TScalarMatrix<Scalar, 4, 4>& a) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> Inverse(const TScalarMatrix<Scalar, 4, 4>& a) noexcept;

		template <typename Scalar>
		TScalarArray<Scalar, 3> Origin(const TScalarMatrix<Scalar, 4, 4>& a) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 3, 3> Basis(const TScalarMatrix<Scalar, 4, 4>& a) noexcept;

		template <typename Scalar> Scalar TDot(const TScalarMatrix<Scalar, 4, 4>& a, int i, const TScalarArray<Scalar, 4>& v) noexcept;

		template <int I, int J, typename Scalar> Scalar Cofactor(const TScalarMatrix<Scalar, 4, 4>& a) noexcept;

		template <typename Scalar>
		Scalar Trace(const TScalarMatrix<Scalar, 4, 4>& a) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> ScaleMatrix4x4(const TScalarArray<Scalar, 3>& a) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> ScaleMatrix4x4(Scalar x, Scalar y, Scalar z) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> RotateMatrix4x4(const TScalarArray<Scalar, 3>& a) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> RotateMatrix4x4(Scalar yaw, Scalar roll, Scalar pitch) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> RotateMatrix4x4(const TScalarQuaternion<Scalar>& q) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> TranslateMatrix4x4(const TScalarArray<Scalar, 3>& a) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> Frustum(Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar zNear, Scalar zFar) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> Frustum(Scalar fov, Scalar aspect, Scalar zNear, Scalar zFar) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> Orthographic(Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar zNear, Scalar zFar) noexcept;

		template <typename Scalar>
		TScalarMatrix<Scalar, 4, 4> OriginCenteredOrthographic(Scalar width, Scalar height, Scalar zNear, Scalar zFar) noexcept;
	}








	// Member Function

	// --Implementation-- //

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 4, 4>::TScalarMatrix() noexcept
	{
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 4, 4>::TScalarMatrix(FZero) noexcept
	{
		*this = FZero{};
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 4, 4>::TScalarMatrix(FIdentity) noexcept
	{
		*this = FIdentity{};
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 4, 4>::TScalarMatrix(Scalar s) noexcept
	{
		*this = s;
	}

	template<typename Scalar>
	template <typename Scalar2>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 4, 4>::TScalarMatrix(const Scalar2* v) noexcept
	{
		SetValue(v);
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 4, 4>::TScalarMatrix(Scalar a00, Scalar a01, Scalar a02, Scalar a03,
		Scalar a10, Scalar a11, Scalar a12, Scalar a13,
		Scalar a20, Scalar a21, Scalar a22, Scalar a23,
		Scalar a30, Scalar a31, Scalar a32, Scalar a33) noexcept
	{
		SetValue(a00, a01, a02, a03,
			a10, a11, a12, a13,
			a20, a21, a22, a23,
			a30, a31, a32, a33);
	}

	template<typename Scalar>
	FORCEINLINE constexpr TScalarMatrix<Scalar, 4, 4>::TScalarMatrix(const TScalarArray<Scalar, 4>& r0,
		const TScalarArray<Scalar, 4>& r1,
		const TScalarArray<Scalar, 4>& r2,
		const TScalarArray<Scalar, 4>& r3) noexcept
	{
		mRows[0] = r0;
		mRows[1] = r1;
		mRows[2] = r2;
		mRows[3] = r3;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>::operator const TScalarArray<Scalar, 4>* () const noexcept
	{
		return mRows;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>::operator TScalarArray<Scalar, 4>* () noexcept
	{
		return mRows;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>& TScalarMatrix<Scalar, 4, 4>::operator=(FZero zero) noexcept
	{
		mRows[0] = zero;
		mRows[1] = zero;
		mRows[2] = zero;
		mRows[3] = zero;

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>& TScalarMatrix<Scalar, 4, 4>::operator+=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>& TScalarMatrix<Scalar, 4, 4>::operator-=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>& TScalarMatrix<Scalar, 4, 4>::operator*=(FZero zero) noexcept
	{
		mRows[0] = zero;
		mRows[1] = zero;
		mRows[2] = zero;
		mRows[3] = zero;

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>& TScalarMatrix<Scalar, 4, 4>::operator=(FIdentity) noexcept
	{
		mRows[0] = FUnit<0>{};
		mRows[1] = FUnit<1>{};
		mRows[2] = FUnit<2>{};
		mRows[3] = FUnit<3>{};

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>& TScalarMatrix<Scalar, 4, 4>::operator=(Scalar s) noexcept
	{
		SetValue(s, Scalar{}, Scalar{}, Scalar{},
			Scalar{}, s, Scalar{}, Scalar{},
			Scalar{}, Scalar{}, s, Scalar{},
			Scalar{}, Scalar{}, Scalar{}, s);

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>& TScalarMatrix<Scalar, 4, 4>::operator*=(Scalar s) noexcept
	{
		mRows[0] *= s;
		mRows[1] *= s;
		mRows[2] *= s;
		mRows[3] *= s;

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>& TScalarMatrix<Scalar, 4, 4>::operator/=(Scalar s) noexcept
	{
		ASSERT(!FMath::IsZero(s));
		return *this *= (Scalar{ 1 } / s);
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>& TScalarMatrix<Scalar, 4, 4>::operator+=(const TScalarMatrix<Scalar, 4, 4>& a) noexcept
	{
		mRows[0] += a[0];
		mRows[1] += a[1];
		mRows[2] += a[2];
		mRows[3] += a[3];

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>& TScalarMatrix<Scalar, 4, 4>::operator-=(const TScalarMatrix<Scalar, 4, 4>& a) noexcept
	{
		mRows[0] -= a[0];
		mRows[1] -= a[1];
		mRows[2] -= a[2];
		mRows[3] -= a[3];

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4>& TScalarMatrix<Scalar, 4, 4>::operator*=(TScalarMatrix<Scalar, 4, 4> a) noexcept
	{
		mRows[0][0] = FMath::Dot(mRows[0], FMath::Column(a, 0));
		mRows[0][1] = FMath::Dot(mRows[0], FMath::Column(a, 1));
		mRows[0][2] = FMath::Dot(mRows[0], FMath::Column(a, 2));
		mRows[0][3] = FMath::Dot(mRows[0], FMath::Column(a, 3));

		mRows[1][0] = FMath::Dot(mRows[1], FMath::Column(a, 0));
		mRows[1][1] = FMath::Dot(mRows[1], FMath::Column(a, 1));
		mRows[1][2] = FMath::Dot(mRows[1], FMath::Column(a, 2));
		mRows[1][3] = FMath::Dot(mRows[1], FMath::Column(a, 3));

		mRows[2][0] = FMath::Dot(mRows[2], FMath::Column(a, 0));
		mRows[2][1] = FMath::Dot(mRows[2], FMath::Column(a, 1));
		mRows[2][2] = FMath::Dot(mRows[2], FMath::Column(a, 2));
		mRows[2][3] = FMath::Dot(mRows[2], FMath::Column(a, 3));

		mRows[3][0] = FMath::Dot(mRows[3], FMath::Column(a, 0));
		mRows[3][1] = FMath::Dot(mRows[3], FMath::Column(a, 1));
		mRows[3][2] = FMath::Dot(mRows[3], FMath::Column(a, 2));
		mRows[3][3] = FMath::Dot(mRows[3], FMath::Column(a, 3));

		return *this;
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 4, 4>::SetValue(Scalar a00, Scalar a01, Scalar a02, Scalar a03,
		Scalar a10, Scalar a11, Scalar a12, Scalar a13,
		Scalar a20, Scalar a21, Scalar a22, Scalar a23,
		Scalar a30, Scalar a31, Scalar a32, Scalar a33) noexcept
	{
		mRows[0] = TScalarArray<Scalar, 4>{ a00, a01, a02, a03 };
		mRows[1] = TScalarArray<Scalar, 4>{ a10, a11, a12, a13 };
		mRows[2] = TScalarArray<Scalar, 4>{ a20, a21, a22, a23 };
		mRows[3] = TScalarArray<Scalar, 4>{ a30, a31, a32, a33 };
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE void TScalarMatrix<Scalar, 4, 4>::SetValue(const Scalar2* v) noexcept
	{
		ASSERT(v != nullptr);

		mRows[0] = TScalarArray<Scalar, 4>{ Scalar{v[0]}, Scalar{v[1]}, Scalar{v[2]}, Scalar{v[3]} };
		mRows[1] = TScalarArray<Scalar, 4>{ Scalar{v[4]}, Scalar{v[5]}, Scalar{v[6]}, Scalar{v[7]} };
		mRows[2] = TScalarArray<Scalar, 4>{ Scalar{v[8]}, Scalar{v[9]}, Scalar{v[10]}, Scalar{v[11]} };
		mRows[3] = TScalarArray<Scalar, 4>{ Scalar{v[12]}, Scalar{v[13]}, Scalar{v[14]}, Scalar{v[15]} };
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 4, 4>::SetRows(const TScalarArray<Scalar, 4>& r0, const TScalarArray<Scalar, 4>& r1, const TScalarArray<Scalar, 4>& r2, const TScalarArray<Scalar, 4>& r3) noexcept
	{
		mRows[0] = r0;
		mRows[1] = r1;
		mRows[2] = r2;
		mRows[3] = r3;
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 4, 4>::SetRow(int i, const TScalarArray<Scalar, 4>& v) noexcept
	{
		mRows[i] = v;
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 4, 4>::SetRow(int i, const TScalarArray<Scalar, 3>& v) noexcept
	{
		mRows[i] = TScalarArray<Scalar, 4>{ v.X, v.Y, v.Z, mRows[i].W };
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 4, 4>::SetColumns(const TScalarArray<Scalar, 4>& c0, const TScalarArray<Scalar, 4>& c1, const TScalarArray<Scalar, 4>& c2, const TScalarArray<Scalar, 4>& c3) noexcept
	{
		mRows[0] = TScalarArray<Scalar, 4>{ c0.X, c1.X, c2.X, c3.X };
		mRows[1] = TScalarArray<Scalar, 4>{ c0.Y, c1.Y, c2.Y, c3.Y };
		mRows[2] = TScalarArray<Scalar, 4>{ c0.Z, c1.Z, c2.Z, c3.Z };
		mRows[3] = TScalarArray<Scalar, 4>{ c0.W, c1.W, c2.W, c3.W };
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 4, 4>::SetColumn(int j, const TScalarArray<Scalar, 4>& v) noexcept
	{
		mRows[0][j] = v.X;
		mRows[1][j] = v.Y;
		mRows[2][j] = v.Z;
		mRows[3][j] = v.W;
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 4, 4>::SetBasis(const TScalarMatrix<Scalar, 3, 3>& basis) noexcept
	{
		SetRows(0, basis[0]);
		SetRows(1, basis[1]);
		SetRows(2, basis[2]);
	}

	template<typename Scalar>
	FORCEINLINE void TScalarMatrix<Scalar, 4, 4>::SetOrigin(const TScalarArray<Scalar, 3>& origin) noexcept
	{
		SetRows(3, origin);
	}









	// Non-member Operators 

	// --Implementation-- //

#ifdef USE_OSTREAM

	template <typename CharT, typename Traits, typename Scalar>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TScalarMatrix<Scalar, 4, 4>& a)
	{
		return os << a[0] << ' ' << a[1] << ' ' << a[2] << ' ' << a[3];
	}

#endif

#ifdef USE_ISTREAM

	template <typename CharT, typename Traits, typename Scalar>
	std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, TScalarMatrix<Scalar, 4, 4>& a)
	{
		return is >> a[0] >> a[1] >> a[2] >> a[3];
	}

#endif

	template <typename Scalar>
	FORCEINLINE bool operator==(const TScalarMatrix<Scalar, 4, 4>& a, const TScalarMatrix<Scalar, 4, 4>& b) noexcept
	{
		return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
	}

	template <typename Scalar>
	FORCEINLINE TScalarMatrix<Scalar, 4, 4> operator-(const TScalarMatrix<Scalar, 4, 4>& a) noexcept
	{
		return TScalarMatrix<Scalar, 4, 4>{ -a[0][0], -a[0][1], -a[0][2], -a[0][3],
			-a[1][0], -a[1][1], -a[1][2], -a[1][3],
			-a[2][0], -a[2][1], -a[2][2], -a[2][3],
			-a[3][0], -a[3][1], -a[3][2], -a[3][3] };
	}

	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator+(const TScalarMatrix<Scalar1, 4, 4>& a, const TScalarMatrix<Scalar2, 4, 4>& b) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		return TScalarMatrix<RT, 4, 4>{ a[0][0] + b[0][0], a[0][1] + b[0][1], a[0][2] + b[0][2], a[0][3] + b[0][3],
			a[1][0] + b[1][0], a[1][1] + b[1][1], a[1][2] + b[1][2], a[1][3] + b[1][3],
			a[2][0] + b[2][0], a[2][1] + b[2][1], a[2][2] + b[2][2], a[2][3] + b[2][3],
			a[3][0] + b[3][0], a[3][1] + b[3][1], a[3][2] + b[3][2], a[3][3] + b[3][3] };
	}

	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator-(const TScalarMatrix<Scalar1, 4, 4>& a, const TScalarMatrix<Scalar2, 4, 4>& b) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		return TScalarMatrix<RT, 4, 4>{ a[0][0] - b[0][0], a[0][1] - b[0][1], a[0][2] - b[0][2], a[0][3] - b[0][3],
			a[1][0] - b[1][0], a[1][1] - b[1][1], a[1][2] - b[1][2], a[1][3] - b[1][3],
			a[2][0] - b[2][0], a[2][1] - b[2][1], a[2][2] - b[2][2], a[2][3] - b[2][3],
			a[3][0] - b[3][0], a[3][1] - b[3][1], a[3][2] - b[3][2], a[3][3] - b[3][3] };
	}

	//#if USE_MATRIX_COMP_MULT
	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator*(const TScalarMatrix<Scalar1, 4, 4>& a, const TScalarMatrix<Scalar2, 4, 4>& b)
	{
		//using RT = typename TPromote<Scalar1, Scalar2>::RT;
		//return TScalarMatrix<RT, 4, 4>{ a[0][0] * b[0][0], a[0][1] * b[0][1], a[0][2] * b[0][2], a[0][3] * b[0][3],
		//	a[1][0] * b[1][0], a[1][1] * b[1][1], a[1][2] * b[1][2], a[1][3] * b[1][3],
		//	a[2][0] * b[2][0], a[2][1] * b[2][1], a[2][2] * b[2][2], a[2][3] * b[2][3],
		//	a[3][0] * b[3][0], a[3][1] * b[3][1], a[3][2] * b[3][2], a[3][3] * b[3][3] };

		return FMath::Mul(a, b);
	}
	//#endif

	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator*(const TScalarMatrix<Scalar1, 4, 4>& a, Scalar2 s) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		return TScalarMatrix<RT, 4, 4>{ a[0][0] * s, a[0][1] * s, a[0][2] * s, a[0][3] * s,
			a[1][0] * s, a[1][1] * s, a[1][2] * s, a[1][3] * s,
			a[2][0] * s, a[2][1] * s, a[2][2] * s, a[2][3] * s,
			a[3][0] * s, a[3][1] * s, a[3][2] * s, a[3][3] * s };
	}

	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator*(Scalar1 s, const TScalarMatrix<Scalar2, 4, 4>& a) noexcept
	{
		return a * s;
	}

	template <typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> operator/(const TScalarMatrix<Scalar1, 4, 4>& a, Scalar2 s) noexcept
	{
		ASSERT(FMath::IsZero(s));
		return a * (Scalar1{ 1 } / s);
	}











	// Non-member Function

	// --Implementation-- //


	namespace FMath
	{
		template <typename Scalar1, typename Scalar2>
		FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 4> Mul(const TScalarMatrix<Scalar1, 4, 4>& a, const TScalarArray<Scalar2, 4>& v) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			return TScalarArray<RT, 4>{ Dot(a[0], v),
				Dot(a[1], v),
				Dot(a[2], v),
				Dot(a[3], v) };
		}

		template <typename Scalar1, typename Scalar2>
		FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 4> Mul(const TScalarArray<Scalar1, 4>& v, const TScalarMatrix<Scalar2, 4, 4>& a) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			return TScalarArray<RT, 4>(TDot(a, 0, v),
				TDot(a, 1, v),
				TDot(a, 2, v),
				TDot(a, 3, v));
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, 4, 4> Mul(const TScalarMatrix<Scalar1, 4, 4>& a, const TScalarMatrix<Scalar2, 4, 4>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			return TScalarMatrix<RT, 4, 4>{TDot(b, 0, a[0]), TDot(b, 1, a[0]), TDot(b, 2, a[0]), TDot(b, 3, a[0]),
				TDot(b, 0, a[1]), TDot(b, 1, a[1]), TDot(b, 2, a[1]), TDot(b, 3, a[1]),
				TDot(b, 0, a[2]), TDot(b, 1, a[2]), TDot(b, 2, a[2]), TDot(b, 3, a[2]),
				TDot(b, 0, a[3]), TDot(b, 1, a[3]), TDot(b, 2, a[3]), TDot(b, 3, a[3]) };
		}

		template <typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 4> Row(const TScalarMatrix<Scalar, 4, 4>& a, std::size_t i) noexcept
		{
			return a[i];
		}

		template <typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 4> Column(const TScalarMatrix<Scalar, 4, 4>& a, std::size_t i) noexcept
		{
			return TScalarArray<Scalar, 4>{a[0][i], a[1][i], a[2][i], a[3][i]};
		}

		template <typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> Transpose(const TScalarMatrix<Scalar, 4, 4>& a) noexcept
		{
			return TScalarMatrix<Scalar, 4, 4>{a[0][0], a[1][0], a[2][0], a[3][0],
				a[0][1], a[1][1], a[2][1], a[3][1],
				a[0][2], a[1][2], a[2][2], a[3][2],
				a[0][3], a[1][3], a[2][3], a[3][3] };
		}

		template <typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> Determinate(const TScalarMatrix<Scalar, 4, 4>& a) noexcept
		{
			//Scalar tmp[12];

			//tmp[0] = a[2][2] * a[3][3];
			//tmp[1] = a[3][2] * a[2][3];
			//tmp[2] = a[1][2] * a[3][3];
			//tmp[3] = a[3][2] * a[1][3];
			//tmp[4] = a[1][2] * a[2][3];
			//tmp[5] = a[2][2] * a[1][3];
			//tmp[6] = a[0][2] * a[3][3];
			//tmp[7] = a[3][2] * a[0][3];
			//tmp[8] = a[0][2] * a[2][3];
			//tmp[9] = a[2][2] * a[0][3];
			//tmp[10] = a[0][2] * a[1][3];
			//tmp[11] = a[1][2] * a[0][3];

			//Scalar cof[4];

			//cof[0] = (tmp[0] - tmp[1]) * a[1][1] + (tmp[3] - tmp[2]) * a[2][1] + (tmp[4] - tmp[5]) * a[3][1];
			//cof[1] = (tmp[1] - tmp[0]) * a[0][1] + (tmp[6] - tmp[7]) * a[2][1] + (tmp[9] - tmp[8]) * a[3][1];
			//cof[2] = (tmp[2] - tmp[3]) * a[0][1] + (tmp[7] - tmp[6]) * a[1][1] + (tmp[10] - tmp[11]) * a[3][1];
			//cof[3] = (tmp[5] - tmp[4]) * a[0][1] + (tmp[8] - tmp[9]) * a[1][1] + (tmp[11] - tmp[10]) * a[2][1];

			//return a[0][0] * cof[0] + a[1][0] * cof[1] + a[2][0] * cof[2] + a[3][0] * cof[3];

			Scalar dst[4];
			dst[0] = a[1][1] * a[2][2] * a[3][3] +
				a[1][2] * a[2][3] * a[3][1] +
				a[1][3] * a[2][1] * a[3][2] -
				a[1][1] * a[2][3] * a[3][2] -
				a[1][2] * a[2][1] * a[3][3] -
				a[1][3] * a[2][2] * a[3][1];

			dst[1] = a[1][0] * a[2][3] * a[3][2] +
				a[1][2] * a[2][0] * a[3][3] +
				a[1][3] * a[2][2] * a[3][0] -
				a[1][0] * a[2][2] * a[3][3] -
				a[1][2] * a[2][3] * a[3][0] -
				a[1][3] * a[2][0] * a[3][2];

			dst[2] = a[1][0] * a[2][1] * a[3][3] +
				a[1][1] * a[2][3] * a[3][0] +
				a[1][3] * a[2][0] * a[3][1] -
				a[1][0] * a[2][3] * a[3][1] -
				a[1][1] * a[2][0] * a[3][3] -
				a[1][3] * a[2][1] * a[3][0];

			dst[3] = a[1][0] * a[2][2] * a[3][1] +
				a[1][1] * a[2][0] * a[3][2] +
				a[1][2] * a[2][1] * a[3][0] -
				a[1][0] * a[2][1] * a[3][2] -
				a[1][1] * a[2][2] * a[3][0] -
				a[1][2] * a[2][0] * a[3][1];

			Scalar det = a[0][0] * dst[0] + a[0][1] * dst[1] + a[0][2] * dst[2] + a[0][3] * dst[3];

			return det;
		}

		template <typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> Inverse(const TScalarMatrix<Scalar, 4, 4>& a) noexcept
		{
			Scalar tmp[12];

			TScalarMatrix<Scalar, 4, 4> dst;

			tmp[0] = a[2][2] * a[3][3];
			tmp[1] = a[3][2] * a[2][3];
			tmp[2] = a[1][2] * a[3][3];
			tmp[3] = a[3][2] * a[1][3];
			tmp[4] = a[1][2] * a[2][3];
			tmp[5] = a[2][2] * a[1][3];
			tmp[6] = a[0][2] * a[3][3];
			tmp[7] = a[3][2] * a[0][3];
			tmp[8] = a[0][2] * a[2][3];
			tmp[9] = a[2][2] * a[0][3];
			tmp[10] = a[0][2] * a[1][3];
			tmp[11] = a[1][2] * a[0][3];

			dst[0][0] = (tmp[0] - tmp[1]) * a[1][1] + (tmp[3] - tmp[2]) * a[2][1] + (tmp[4] - tmp[5]) * a[3][1];
			dst[0][1] = (tmp[1] - tmp[0]) * a[0][1] + (tmp[6] - tmp[7]) * a[2][1] + (tmp[9] - tmp[8]) * a[3][1];
			dst[0][2] = (tmp[2] - tmp[3]) * a[0][1] + (tmp[7] - tmp[6]) * a[1][1] + (tmp[10] - tmp[11]) * a[3][1];
			dst[0][3] = (tmp[5] - tmp[4]) * a[0][1] + (tmp[8] - tmp[9]) * a[1][1] + (tmp[11] - tmp[10]) * a[2][1];
			dst[1][0] = (tmp[1] - tmp[0]) * a[1][0] + (tmp[2] - tmp[3]) * a[2][0] + (tmp[5] - tmp[4]) * a[3][0];
			dst[1][1] = (tmp[0] - tmp[1]) * a[0][0] + (tmp[7] - tmp[6]) * a[2][0] + (tmp[8] - tmp[9]) * a[3][0];
			dst[1][2] = (tmp[3] - tmp[2]) * a[0][0] + (tmp[6] - tmp[7]) * a[1][0] + (tmp[11] - tmp[10]) * a[3][0];
			dst[1][3] = (tmp[4] - tmp[5]) * a[0][0] + (tmp[9] - tmp[8]) * a[1][0] + (tmp[10] - tmp[11]) * a[2][0];

			tmp[0] = a[2][0] * a[3][1];
			tmp[1] = a[3][0] * a[2][1];
			tmp[2] = a[1][0] * a[3][1];
			tmp[3] = a[3][0] * a[1][1];
			tmp[4] = a[1][0] * a[2][1];
			tmp[5] = a[2][0] * a[1][1];
			tmp[6] = a[0][0] * a[3][1];
			tmp[7] = a[3][0] * a[0][1];
			tmp[8] = a[0][0] * a[2][1];
			tmp[9] = a[2][0] * a[0][1];
			tmp[10] = a[0][0] * a[1][1];
			tmp[11] = a[1][0] * a[0][1];

			dst[2][0] = (tmp[0] - tmp[1]) * a[1][3] + (tmp[3] - tmp[2]) * a[2][3] + (tmp[4] - tmp[5]) * a[3][3];
			dst[2][1] = (tmp[1] - tmp[0]) * a[0][3] + (tmp[6] - tmp[7]) * a[2][3] + (tmp[9] - tmp[8]) * a[3][3];
			dst[2][2] = (tmp[2] - tmp[3]) * a[0][3] + (tmp[7] - tmp[6]) * a[1][3] + (tmp[10] - tmp[11]) * a[3][3];
			dst[2][3] = (tmp[5] - tmp[4]) * a[0][3] + (tmp[8] - tmp[9]) * a[1][3] + (tmp[11] - tmp[10]) * a[2][3];
			dst[3][0] = (tmp[2] - tmp[3]) * a[2][2] + (tmp[5] - tmp[4]) * a[3][2] + (tmp[1] - tmp[0]) * a[1][2];
			dst[3][1] = (tmp[8] - tmp[9]) * a[3][2] + (tmp[0] - tmp[1]) * a[0][2] + (tmp[7] - tmp[6]) * a[2][2];
			dst[3][2] = (tmp[6] - tmp[7]) * a[1][2] + (tmp[11] - tmp[10]) * a[3][2] + (tmp[3] - tmp[2]) * a[0][2];
			dst[3][3] = (tmp[10] - tmp[11]) * a[2][2] + (tmp[4] - tmp[5]) * a[0][2] + (tmp[9] - tmp[8]) * a[1][2];

			Scalar det = a[0][0] * dst[0][0] + a[1][0] * dst[0][1] + a[2][0] * dst[0][2] + a[3][0] * dst[0][3];

			ASSERT(!IsZero(det));

			Scalar s = Scalar(1) / det;

			return TScalarMatrix<Scalar, 4, 4>(dst[0] * s, dst[1] * s, dst[2] * s, dst[3] * s);
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 3> Origin(const TScalarMatrix<Scalar, 4, 4>& a) noexcept
		{
			return TScalarArray<Scalar, 3>{ a[3][0], a[3][1], a[3][2] };
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 3, 3> Basis(const TScalarMatrix<Scalar, 4, 4>& a) noexcept
		{
			return TScalarMatrix<Scalar, 3, 3>{ a[0][0], a[0][1], a[0][2],
				a[1][0], a[1][1], a[1][2],
				a[2][0], a[2][1], a[2][2]
			};
		}

		template <typename Scalar>
		FORCEINLINE Scalar TDot(const TScalarMatrix<Scalar, 4, 4>& a, int i, const TScalarArray<Scalar, 4>& v) noexcept
		{
			return a[0][i] * v.X + a[1][i] * v.Y + a[2][i] * v.Z + a[3][i] * v.W;
		}

		template <int I, int J, typename Scalar>
		FORCEINLINE Scalar Cofactor(const TScalarMatrix<Scalar, 4, 4>& a) noexcept
		{
			return a[(I + 1) & 3][(J + 1) & 3] * (a[(I + 2) & 3][(J + 2) & 3] * a[(I + 3) & 3][(J + 3) & 3] - a[(I + 3) & 3][(J + 2) & 3] * a[(I + 2) & 3][(J + 3) & 3]) +
				a[(I + 2) & 3][(J + 1) & 3] * (a[(I + 3) & 3][(J + 2) & 3] * a[(I + 1) & 3][(J + 3) & 3] - a[(I + 1) & 3][(J + 2) & 3] * a[(I + 3) & 3][(J + 3) & 3]) +
				a[(I + 3) & 3][(J + 1) & 3] * (a[(I + 1) & 3][(J + 2) & 3] * a[(I + 2) & 3][(J + 3) & 3] - a[(I + 2) & 3][(J + 2) & 3] * a[(I + 1) & 3][(J + 3) & 3]);
		}

		template <typename Scalar>
		FORCEINLINE Scalar Trace(const TScalarMatrix<Scalar, 4, 4>& a) noexcept
		{
			return a[0][0] + a[1][1] + a[2][2] + a[3][3];
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> ScaleMatrix4x4(const TScalarArray<Scalar, 3>& a) noexcept
		{
			return ScaleMatrix4x4(a.X, a.Y, a.Z);
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> ScaleMatrix4x4(Scalar x, Scalar y, Scalar z) noexcept
		{
			return TScalarMatrix<Scalar, 4, 4>{
				x, Scalar{}, Scalar{}, Scalar{},
					Scalar{}, y, Scalar{}, Scalar{},
					Scalar{}, Scalar{}, z, Scalar{},
					Scalar{}, Scalar{}, Scalar{}, Scalar{ 1 }
			};
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> RotateMatrix4x4(const TScalarArray<Scalar, 3>& a) noexcept
		{
			return RotateMatrix4x4(a.X, a.Y, a.Z);
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> RotateMatrix4x4(Scalar yaw, Scalar roll, Scalar pitch) noexcept
		{
			Scalar sinx = Sin(yaw);
			Scalar cosx = Cos(yaw);

			Scalar siny = Sin(roll);
			Scalar cosy = Cos(roll);

			Scalar sinz = Sin(pitch);
			Scalar cosz = Cos(pitch);

			return TScalarMatrix<Scalar, 4, 4>{
				-sinz * sinx * siny + cosz * cosy, -sinz * cosx, sinz* sinx* cosy + cosz * siny, 0,
					-cosz * sinx * siny + sinz * cosy, -cosz * cosx, -cosz * sinx * cosy + sinz * siny, 0,
					-cosx * siny, sinx, cosx* cosy, 0,
					0, 0, 0, 1
			};
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> RotateMatrix4x4(const TScalarQuaternion<Scalar>& q) noexcept
		{
			TScalarMatrix<Scalar, 4, 4> m{ FIdentity{} };
			m[0][0] = 1 - 2 * q.Y * q.Y - 2 * q.Z * q.Z;
			m[0][1] = 2 * q.X * q.Y + 2 * q.Z * q.W;
			m[0][2] = 2 * q.X * q.Z - 2 * q.Y * q.W;

			m[1][0] = 2 * q.X * q.Y - 2 * q.Z * q.W;
			m[1][1] = 1 - 2 * q.X * q.X - 2 * q.Z * q.Z;
			m[1][2] = 2 * q.Y * q.Z + 2 * q.X * q.W;

			m[2][0] = 2 * q.X * q.Z + 2 * q.Y * q.W;
			m[2][1] = 2 * q.Y * q.Z - 2 * q.X * q.W;
			m[2][2] = 1 - 2 * q.X * q.X - 2 * q.Y * q.Y;

			return m;
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> TranslateMatrix4x4(const TScalarArray<Scalar, 3>& a) noexcept
		{
			return TScalarMatrix<Scalar, 4, 4>{
				Scalar{ 1 }, Scalar{}, Scalar{}, Scalar{},
					Scalar{}, Scalar{ 1 }, Scalar{}, Scalar{},
					Scalar{}, Scalar{}, Scalar{ 1 }, Scalar{},
					a.X, a.Y, a.Z, Scalar{ 1 }
			};
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> Frustum(Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar zNear, Scalar zFar) noexcept
		{
			Scalar oneOverWidth = Scalar{ 1 } / (right - left);
			Scalar oneOverHeight = Scalar{ 1 } / (top - bottom);
			Scalar oneOverDepth = Scalar{ 1 } / (zFar - zNear);
			return TScalarMatrix<Scalar, 4, 4>{TScalarArray<Scalar, 4>{zNear* Scalar{ 2 } *oneOverWidth, Scalar{}, -(right + left) * oneOverWidth},
				TScalarArray<Scalar, 4>{Scalar{}, zNear* Scalar{ 2 } *oneOverHeight, -(top + bottom) * oneOverHeight},
				TScalarArray<Scalar, 4>{Scalar{}, Scalar{}, zFar* oneOverDepth, -zNear * zFar * oneOverDepth},
				TScalarArray<Scalar, 4>{Scalar{}, Scalar{}, Scalar{ 1 } }};
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> Frustum(Scalar fov, Scalar aspect, Scalar zNear, Scalar zFar) noexcept
		{
			Scalar oneOverDepth = Scalar{ 1 } / (zFar - zNear);
			Scalar cotHalfFov = Scalar{ 1 } / Tan(Radians(fov) * Scalar { 0.5 });
			return TScalarMatrix<Scalar, 4, 4>{ TScalarArray<Scalar, 4>{ (Scalar{ 1 } / aspect) * cotHalfFov, Scalar{}, Scalar{}, Scalar{} },
				TScalarArray<Scalar, 4>{ Scalar{}, cotHalfFov, Scalar{}, Scalar{} },
				TScalarArray<Scalar, 4>{ Scalar{}, Scalar{}, zFar* oneOverDepth, Scalar{ 1 } },
				TScalarArray<Scalar, 4>{ Scalar{}, Scalar{}, -zFar * zNear * oneOverDepth, Scalar{} }};
		}

		template<typename Scalar>
		FORCEINLINE TScalarMatrix<Scalar, 4, 4> Orthographic(Scalar left, Scalar right, Scalar bottom, Scalar top, Scalar zNear, Scalar zFar) noexcept
		{
			Scalar oneOverWidth = Scalar{ 1 } / (right - left);
			Scalar oneOverHeight = Scalar{ 1 } / (top - bottom);
			Scalar oneOverDepth = Scalar{ 1 } / (zFar - zNear);

			return	TScalarMatrix<Scalar, 4, 4>{	
				TScalarArray<Scalar, 4>{ 2 * oneOverWidth,			Scalar{},						Scalar{}, Scalar{} },
				TScalarArray<Scalar, 4>{ Scalar{},					2 * oneOverHeight,				Scalar{}, Scalar{} },
				TScalarArray<Scalar, 4>{ Scalar{},					 Scalar{},						oneOverDepth, Scalar{} },
				TScalarArray<Scalar, 4>{ -(left + right) * oneOverWidth, -(bottom + top) * oneOverHeight, -zNear * oneOverDepth, Scalar{ 1 } }
			};
		}

		template<typename Scalar>
		TScalarMatrix<Scalar, 4, 4> OriginCenteredOrthographic(Scalar width, Scalar height, Scalar zNear, Scalar zFar) noexcept
		{
			Scalar oneOverDepth = Scalar{ 1 } / (zFar - zNear);

			return TScalarMatrix<Scalar, 4, 4>{
				TScalarArray<Scalar, 4>{ Scalar{2} / width, Scalar{}, Scalar{}, Scalar{} },
				TScalarArray<Scalar, 4>{ Scalar{} ,			Scalar{ 2 } /height, Scalar{}, Scalar{} },
				TScalarArray<Scalar, 4>{ Scalar{},			Scalar{}, oneOverDepth, Scalar{} },
					TScalarArray<Scalar, 4>{ Scalar{}, Scalar{}, -zNear * oneOverDepth, Scalar{1} },
			};
		}
	}
}
