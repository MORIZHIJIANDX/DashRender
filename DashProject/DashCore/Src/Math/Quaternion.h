#pragma once

#ifdef USE_OSTREAM
#include <ostream>
#endif

#ifdef USE_ISTREAM
#include <istream>
#endif

#include "ScalarMatrix.h"
#include "Metric.h"

#include "../Utility/Assert.h"

namespace Dash
{

	template <typename Scalar>
	struct TScalarQuaternion
	{
	public:
		typedef Scalar ScalarType;

		TScalarQuaternion() noexcept;
		TScalarQuaternion(FIdentity) noexcept;
		TScalarQuaternion(Scalar x, Scalar y, Scalar z, Scalar w) noexcept;

		template <typename Scalar2> explicit TScalarQuaternion(const Scalar2* v) noexcept;
		template <typename Scalar2> explicit TScalarQuaternion(const TScalarQuaternion<Scalar2>& a) noexcept;
		template <typename Scalar2> explicit TScalarQuaternion(const TScalarArray<Scalar2, 4>& a) noexcept;
		template <typename Scalar2> TScalarQuaternion(const TScalarArray<Scalar2, 3>& axis, Scalar2 angle) noexcept;
		template <typename Scalar2> TScalarQuaternion(const TScalarArray<Scalar2, 3>& from, const TScalarArray<Scalar2, 3>& to) noexcept;

		operator const Scalar* () const noexcept;
		operator Scalar* () noexcept;

		template <typename Scalar2> TScalarQuaternion<Scalar>& operator*=(const TScalarQuaternion<Scalar2>& other) noexcept;

		template <typename Scalar2> TScalarArray<typename TPromote<Scalar, Scalar2>::RT, 3> operator()(const TScalarArray<Scalar2, 3>& v) const noexcept;

		union
		{
			struct { Scalar X, Y, Z, W; };
			TScalarArray<Scalar, 3> XYZ{};
		};
	};







	// Non-member Operators 

	// --Declaration-- //


#ifdef USE_OSTREAM

	template <typename CharT, typename Traits, typename Scalar>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TScalarQuaternion<Scalar>& a);

#endif

#ifdef USE_ISTREAM

	template <typename CharT, typename Traits, typename Scalar>
	std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, TScalarQuaternion<Scalar>& a);

#endif

	template <typename Scalar1, typename Scalar2> bool operator==(const TScalarQuaternion<Scalar1>& a, const TScalarQuaternion<Scalar2>& b) noexcept;
	template <typename Scalar1, typename Scalar2> bool operator!=(const TScalarQuaternion<Scalar1>& a, const TScalarQuaternion<Scalar2>& b) noexcept;

	template <typename Scalar1, typename Scalar2>
	TScalarQuaternion<typename TPromote<Scalar1, Scalar2>::RT> operator*(const TScalarQuaternion<Scalar1>& a, const TScalarQuaternion<Scalar2>& b) noexcept;







	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		template <typename Scalar1, typename Scalar2>
		typename TPromote<Scalar1, Scalar2>::RT Dot(const TScalarQuaternion<Scalar1>& a, const TScalarQuaternion<Scalar2>& b) noexcept;

		template <typename Scalar1, typename Scalar2>
		TScalarQuaternion<typename TPromote<Scalar1, Scalar2>::RT> Mul(const TScalarQuaternion<Scalar1>& a, const TScalarQuaternion<Scalar2>& b) noexcept;

		template <typename Scalar1, typename Scalar2>
		TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 3> Mul(const TScalarQuaternion<Scalar2>& a, const TScalarArray<Scalar1, 3>& b) noexcept;

		template <typename Scalar> TScalarQuaternion<Scalar> Conjugate(const TScalarQuaternion<Scalar>& a) noexcept;
		template <typename Scalar> TScalarQuaternion<Scalar> Inverse(const TScalarQuaternion<Scalar>& a) noexcept;

		template <typename Scalar> TScalarQuaternion<Scalar> FromAxisAngle(const TScalarArray<Scalar, 3>& axis, Scalar angle) noexcept;
		template <typename Scalar> TScalarQuaternion<Scalar> FromAxisAngle(int axis, Scalar theta) noexcept;
		template <typename Scalar> void ToAxisAngle(TScalarArray<Scalar, 3>& axis, Scalar& theta, const TScalarQuaternion<Scalar>& q) noexcept;

		template <typename Scalar> TScalarQuaternion<Scalar> FromEuler(Scalar pitch, Scalar yaw, Scalar roll) noexcept;
		template <typename Scalar> TScalarQuaternion<Scalar> FromEuler(const TScalarArray<Scalar, 3>& euler) noexcept;

		template <typename Scalar> void ToEuler(Scalar& yaw, Scalar& pitch, Scalar& roll, const TScalarQuaternion<Scalar>& q) noexcept;
		template <typename Scalar> void ToEuler(TScalarArray<Scalar, 3>& euler, const TScalarQuaternion<Scalar>& q) noexcept;

		template <typename Scalar> TScalarQuaternion<Scalar> FromSpherical(Scalar rho, Scalar phi, Scalar theta) noexcept;
		template <typename Scalar> void ToSpherical(Scalar& rho, Scalar& phi, Scalar& theta, const TScalarQuaternion<Scalar>& u) noexcept;

		template <typename Scalar> TScalarArray<Scalar, 2> CartesianToSpherical(const TScalarArray<Scalar, 3>& norm);
		template <typename Scalar> TScalarArray<Scalar, 3> SphericalToCartesian(const TScalarArray<Scalar, 2>& s);

		template <typename Scalar> TScalarQuaternion<Scalar> FromToRotation(const TScalarArray<Scalar, 3>& from, const TScalarArray<Scalar, 3>& to) noexcept;

		template <typename Scalar> TScalarQuaternion<Scalar> FromMatrix(const TScalarMatrix<Scalar, 3, 3>& m) noexcept;
		template <typename Scalar> TScalarQuaternion<Scalar> FromMatrix(const TScalarMatrix<Scalar, 4, 4>& m) noexcept;
		template <typename Scalar> void ToMatrix(TScalarMatrix<Scalar, 4, 4>& m, const TScalarQuaternion<Scalar>& q) noexcept;
		template <typename Scalar> void ToMatrix(TScalarMatrix<Scalar, 3, 3>& m, const TScalarQuaternion<Scalar>& q) noexcept;

		template <typename Scalar> TScalarQuaternion<Scalar> Normalize(const TScalarQuaternion<Scalar>& a) noexcept;

		template <typename Scalar> TScalarQuaternion<Scalar> LerpAndNormalize(const TScalarQuaternion<Scalar>& a, const TScalarQuaternion<Scalar>& b, Scalar t) noexcept;

		template <typename Scalar> TScalarQuaternion<Scalar> Slerp(const TScalarQuaternion<Scalar>& a, const TScalarQuaternion<Scalar>& b, Scalar t) noexcept;

		template <typename Scalar>
		void DecomposeAffineMatrix4x4(TScalarArray<Scalar, 3>& scale, TScalarQuaternion<Scalar>& rotation, TScalarArray<Scalar, 3>& translation, const TScalarMatrix<Scalar, 4, 4>& a) noexcept;
	}







	// Member Function

	// --Implementation-- //

	template<typename Scalar>
	FORCEINLINE TScalarQuaternion<Scalar>::TScalarQuaternion() noexcept
		: X()
		, Y()
		, Z()
		, W()
	{}

	template<typename Scalar>
	FORCEINLINE
		TScalarQuaternion<Scalar>::TScalarQuaternion(FIdentity) noexcept
		: X()
		, Y()
		, Z()
		, W(Scalar{ 1 })
	{}

	template<typename Scalar>
	FORCEINLINE TScalarQuaternion<Scalar>::TScalarQuaternion(Scalar x, Scalar y, Scalar z, Scalar w) noexcept
		: X(x)
		, Y(y)
		, Z(z)
		, W(w)
	{}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarQuaternion<Scalar>::TScalarQuaternion(const Scalar2* v) noexcept
		: X(Scalar{ v[0] })
		, Y(Scalar{ v[1] })
		, Z(Scalar{ v[2] })
		, W(Scalar{ v[3] })
	{
		ASSERT(v != nullptr);
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarQuaternion<Scalar>::TScalarQuaternion(const TScalarQuaternion<Scalar2>& a) noexcept
		: X(Scalar{ a.X })
		, Y(Scalar{ a.Y })
		, Z(Scalar{ a.Z })
		, W(Scalar{ a.W })
	{}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarQuaternion<Scalar>::TScalarQuaternion(const TScalarArray<Scalar2, 4>& a) noexcept
		: X(Scalar{ a.X })
		, Y(Scalar{ a.Y })
		, Z(Scalar{ a.Z })
		, W(Scalar{ a.W })
	{
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarQuaternion<Scalar>::TScalarQuaternion(const TScalarArray<Scalar2, 3>& axis, Scalar2 angle) noexcept
	{
		Scalar halfTheta = angle * Scalar(0.5);
		TScalarArray<Scalar, 3> normalizedAxis = FMath::Normalize(axis) * FMath::Sin(halfTheta);
		X = normalizedAxis.X;
		Y = normalizedAxis.Y;
		Z = normalizedAxis.Z;
		W = FMath::Cos(halfTheta);
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarQuaternion<Scalar>::TScalarQuaternion(const TScalarArray<Scalar2, 3>& from, const TScalarArray<Scalar2, 3>& to) noexcept
	{
		const TScalarArray<Scalar, 3> normStart = FMath::Normalize(from);
		const TScalarArray<Scalar, 3> normEnd = FMath::Normalize(to);
		const Scalar d = FMath::Dot(normStart, normEnd);

		if (d > Scalar{ -1 } + TScalarTraits<Scalar>::Epsilon())
		{
			const TScalarArray<Scalar, 3> c = FMath::Cross(normStart, normEnd);
			const Scalar s = FMath::Sqrt((Scalar{ 1 } +d) * Scalar { 2 });
			const Scalar invS = 1.0f / s;

			X = c.X * invS;
			Y = c.Y * invS;
			Z = c.Z * invS;
			W = Scalar{ 0.5 } *s;
		}
		else
		{
			TScalarArray<Scalar, 3> axis = FMath::Cross(TScalarArray<Scalar, 3>{ FUnit<0>{} }, normStart);

			if (FMath::Length(axis) < TScalarTraits<Scalar>::Epsilon())
			{
				axis = FMath::Cross(TScalarArray<Scalar, 3>{ FUnit<1>{} }, normStart);
			}

			Scalar halfTheta = FMath::Radians(Scalar{ 180 }) * Scalar(0.5);
			TScalarArray<Scalar, 3> normalizedAxis = FMath::Normalize(axis) * FMath::Sin(halfTheta);
			X = normalizedAxis.X;
			Y = normalizedAxis.Y;
			Z = normalizedAxis.Z;
			W = FMath::Cos(halfTheta);
		}
	}

	template<typename Scalar>
	FORCEINLINE TScalarQuaternion<Scalar>::operator const Scalar* () const noexcept
	{
		return &X;
	}

	template<typename Scalar>
	FORCEINLINE TScalarQuaternion<Scalar>::operator Scalar* () noexcept
	{
		return &X;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarQuaternion<Scalar>& TScalarQuaternion<Scalar>::operator*=(const TScalarQuaternion<Scalar2>& other) noexcept
	{
		TScalarQuaternion<Scalar> temp;
		temp.X = W * other.X + X * other.W + Y * other.Z - Z * other.Y;
		temp.Y = W * other.Y - X * other.Z + Y * other.W + Z * other.X;
		temp.Z = W * other.Z + X * other.Y - Y * other.X + Z * other.W;
		temp.W = W * other.W - X * other.X - Y * other.Y - Z * other.Z;
		*this = temp;

		return *this;
	}

	template<typename Scalar>
	template<typename Scalar2>
	FORCEINLINE TScalarArray<typename TPromote<Scalar, Scalar2>::RT, 3> TScalarQuaternion<Scalar>::operator()(const TScalarArray<Scalar2, 3>& v) const noexcept
	{
		using RT = typename TPromote<Scalar, Scalar2>::RT;

		TScalarArray<RT, 3> u{ X, Y, Z };

		TScalarArray<RT, 3> c1 = FMath::Cross(u, v);
		TScalarArray<RT, 3> c2 = FMath::Cross(u, c1);

		return v + RT{ 2 } *(c1 * W + c2);
	}








	// Non-member Operators 

	// --Implementation-- //

#ifdef USE_OSTREAM

	template<typename CharT, typename Traits, typename Scalar>
	FORCEINLINE std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TScalarQuaternion<Scalar>& a)
	{
		return os << a.X << ' ' << a.Y << ' ' << a.Z << ' ' << a.W;
	}

#endif

#ifdef USE_ISTREAM

	template<typename CharT, typename Traits, typename Scalar>
	FORCEINLINE std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, TScalarQuaternion<Scalar>& a)
	{
		return is >> a.X >> a.Y >> a.Z >> a.W;
	}

#endif

	template<typename Scalar1, typename Scalar2>
	FORCEINLINE bool operator==(const TScalarQuaternion<Scalar1>& a, const TScalarQuaternion<Scalar2>& b) noexcept
	{
		return (a.X == b.X) && (a.Y == b.Y) && (a.Z == b.Z) && (a.W == b.W);
	}

	template<typename Scalar1, typename Scalar2>
	FORCEINLINE bool operator!=(const TScalarQuaternion<Scalar1>& a, const TScalarQuaternion<Scalar2>& b) noexcept
	{
		return (a.X != b.X) || (a.Y != b.Y) || (a.Z != b.Z) || (a.W != b.W);
	}

	template<typename Scalar1, typename Scalar2>
	FORCEINLINE TScalarQuaternion<typename TPromote<Scalar1, Scalar2>::RT> operator*(const TScalarQuaternion<Scalar1>& a, const TScalarQuaternion<Scalar2>& b) noexcept
	{
		typedef typename TPromote<Scalar1, Scalar2>::RT RT;
		return TScalarQuaternion<RT>(a.W * b.X + a.X * b.W + a.Y * b.Z - a.Z * b.Y,
			a.W * b.Y + a.Y * b.W + a.Z * b.X - a.X * b.Z,
			a.W * b.Z + a.Z * b.W + a.X * b.Y - a.Y * b.X,
			a.W * b.W - a.X * b.X - a.Y * b.Y - a.Z * b.Z);
	}








	// Non-member Function

	// --Implementation-- //

	namespace FMath
	{

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE typename TPromote<Scalar1, Scalar2>::RT Dot(const TScalarQuaternion<Scalar1>& a, const TScalarQuaternion<Scalar2>& b) noexcept
		{
			return typename TPromote<Scalar1, Scalar2>::RT(a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W);
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE
			TScalarQuaternion<typename TPromote<Scalar1, Scalar2>::RT> Mul(const TScalarQuaternion<Scalar1>& a, const TScalarQuaternion<Scalar2>& b) noexcept
		{
			typedef typename TPromote<Scalar1, Scalar2>::RT RT;
			return TScalarQuaternion<RT>(a.W * b.X + a.X * b.W + a.Y * b.Z - a.Z * b.Y,
				a.W * b.Y + a.Y * b.W + a.Z * b.X - a.X * b.Z,
				a.W * b.Z + a.Z * b.W + a.X * b.Y - a.Y * b.X,
				a.W * b.W - a.X * b.X - a.Y * b.Y - a.Z * b.Z);
		}

		template<typename Scalar1, typename Scalar2>
		FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, 3> Mul(const TScalarQuaternion<Scalar2>& q, const TScalarArray<Scalar1, 3>& v) noexcept
		{
			typedef typename TPromote<Scalar1, Scalar2>::RT RT;

			TScalarArray<RT, 3> u{ q.XYZ };

			TScalarArray<RT, 3> c1 = Cross(u, v);
			TScalarArray<RT, 3> c2 = Cross(u, c1);

			return v + RT{ 2 } *(c1 * q.W + c2);
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> Conjugate(const TScalarQuaternion<Scalar>& q) noexcept
		{
			return TScalarQuaternion<Scalar>{-q.X, -q.Y, -q.Z, q.W};
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> Inverse(const TScalarQuaternion<Scalar>& q) noexcept
		{
			Scalar invLength = Scalar(1) / Length(q);
			ASSERT(IsPositive(invLength));
			return TScalarQuaternion<Scalar>(-q.X * invLength, -q.Y * invLength, -q.Z * invLength, q.W * invLength);
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> FromAxisAngle(const TScalarArray<Scalar, 3>& axis, Scalar angle) noexcept
		{
			return TScalarQuaternion<Scalar>(axis, angle);
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> FromAxisAngle(int axis, Scalar angle) noexcept
		{
			ASSERT(0 <= axis && axis < 3);
			Scalar halfTheta = angle * Scalar{ 0.5 };
			TScalarQuaternion<Scalar> result(Scalar(), Scalar(), Scalar(), Cos(halfTheta));
			result[axis] = Sin(halfTheta);
			return result;
		}

		template<typename Scalar>
		FORCEINLINE void ToAxisAngle(TScalarArray<Scalar, 3>& axis, Scalar& theta, const TScalarQuaternion<Scalar>& q) noexcept
		{
			Scalar s = Sqrt(q.X * q.X + q.Y * q.Y + q.Z * q.Z);
			if (!IsZero(s))
			{
				theta = Atan2(s, q.W) * Scalar { 2 };
				axis = TScalarArray<Scalar, 3>(q.X, q.Y, q.Z) / s;
			}
			else
			{
				theta = Scalar();
				axis = FUnit<0>();
			}
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> FromEuler(Scalar pitch, Scalar yaw, Scalar roll) noexcept
		{
			TScalarArray<Scalar, 4> angles{ pitch, yaw, roll, Scalar(0) };
			angles *= Scalar(0.5);

			TScalarArray<Scalar, 4> sinAngles{ Sin(angles.X), Sin(angles.Y), Sin(angles.Z), Scalar(0) };
			TScalarArray<Scalar, 4> cosAngles{ Cos(angles.X), Cos(angles.Y), Cos(angles.Z), Scalar(0) };

			TScalarArray<Scalar, 4> p0{ sinAngles.X, cosAngles.X, cosAngles.X, cosAngles.X };
			TScalarArray<Scalar, 4> y0{ cosAngles.Y, sinAngles.Y, cosAngles.Y, cosAngles.Y };
			TScalarArray<Scalar, 4> r0{ cosAngles.Z, cosAngles.Z, sinAngles.Z, cosAngles.Z };

			TScalarArray<Scalar, 4> p1{ cosAngles.X, sinAngles.X, sinAngles.X, sinAngles.X };
			TScalarArray<Scalar, 4> y1{ sinAngles.Y, cosAngles.Y, sinAngles.Y, sinAngles.Y };
			TScalarArray<Scalar, 4> r1{ sinAngles.Z, sinAngles.Z, cosAngles.Z, sinAngles.Z };

			TScalarArray<Scalar, 4> sign{ Scalar{1}, -Scalar{1}, -Scalar{1}, Scalar{1} };

			TScalarArray<Scalar, 4> q1 = p1 * sign;
			TScalarArray<Scalar, 4> q0 = p0 * y0;
			q1 = q1 * y1;
			q0 = q0 * r0;

			return TScalarQuaternion<Scalar>(q1 * r1 + q0);
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> FromEuler(const TScalarArray<Scalar, 3>& euler) noexcept
		{
			return FromEuler(euler.X, euler.Y, euler.Z);
		}

		template<typename Scalar>
		FORCEINLINE void ToEuler(Scalar& pitch, Scalar& yaw, Scalar& roll, const TScalarQuaternion<Scalar>& q) noexcept
		{
			yaw = Atan2(Scalar{ 2 } *(q.W * q.Y + q.Z * q.X), Scalar{ 1 } -Scalar{ 2 } *(q.X * q.X + q.Y * q.Y));
			pitch = ASin(Scalar{ 2 } *(q.W * q.X - q.Y * q.Z));
			roll = Atan2(Scalar{ 2 } *(q.W * q.Z + q.X * q.Y), Scalar{ 1 } -Scalar{ 2 } *(q.Z * q.Z + q.X * q.X));
		}

		template<typename Scalar>
		FORCEINLINE void ToEuler(TScalarArray<Scalar, 3>& euler, const TScalarQuaternion<Scalar>& q) noexcept
		{
			ToEuler(euler.X, euler.Y, euler.Z, q);
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> FromSpherical(Scalar rho, Scalar phi, Scalar theta) noexcept
		{
			TScalarArray<Scalar, 2> c1 = TScalarArray<Scalar, 2>{ Cos(theta), Sin(theta) };
			TScalarArray<Scalar, 2> c2 = TScalarArray<Scalar, 2>{ Cos(phi), Sin(phi) };
			TScalarArray<Scalar, 3> axis(c1.X * c2.Y, c1.Y * c2.Y, c2.X);
			return FromAxisAngle(axis, rho);
		}

		template<typename Scalar>
		FORCEINLINE void ToSpherical(Scalar& rho, Scalar& phi, Scalar& theta, const TScalarQuaternion<Scalar>& u) noexcept
		{
			rho = ACos(u.W) * Scalar(2);
			TScalarArray<Scalar, 3> axis = Normalize(u.XYZ);
			phi = ACos(axis.Z);
			theta = Atan2(axis.Y, axis.X);
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 2> CartesianToSpherical(const TScalarArray<Scalar, 3>& norm)
		{
			Scalar theta = Atan2(norm.Z, norm.X);
			Scalar phi = ACos(norm.Y);

			return TScalarArray<Scalar, 2>{theta, phi};
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 3> SphericalToCartesian(const TScalarArray<Scalar, 2>& s)
		{
			Scalar sinTheta, cosTheta;
			SinCos(s.X, sinTheta, cosTheta);

			Scalar sinPhi, cosPhi;
			SinCos(s.Y, sinPhi, cosPhi);

			return TScalarArray<Scalar, 3>{ sinPhi* cosTheta, cosPhi, sinPhi* sinTheta };
		}

		template<typename Scalar>
		FORCEINLINE TScalarArray<Scalar, 3> SphericalToCartesian(Scalar theta, Scalar phi)
		{
			Scalar sinTheta, cosTheta;
			SinCos(theta, sinTheta, cosTheta);

			Scalar sinPhi, cosPhi;
			SinCos(phi, sinPhi, cosPhi);

			return TScalarArray<Scalar, 3>{ sinPhi* cosTheta, cosPhi, sinPhi* sinTheta };
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> FromToRotation(const TScalarArray<Scalar, 3>& from, const TScalarArray<Scalar, 3>& to) noexcept
		{
			const TScalarArray<Scalar, 3> normStart = Normalize(from);
			const TScalarArray<Scalar, 3> normEnd = Normalize(to);
			const Scalar d = Dot(normStart, normEnd);

			if (d > Scalar{ -1 } +TScalarTraits<Scalar>::Epsilon())
			{
				const TScalarArray<Scalar, 3> c = Cross(normStart, normEnd);
				const Scalar s = Sqrt((Scalar{ 1 } +d) * Scalar { 2 });
				const Scalar invS = 1.0f / s;

				return TScalarQuaternion<Scalar>{
					c.X* invS,
						c.Y* invS,
						c.Z* invS,
						Scalar{ 0.5 } *s
				};
			}
			else
			{
				TScalarArray<Scalar, 3> axis = Cross(TScalarArray<Scalar, 3>{ FUnit<0>{} }, normStart);

				if (Length(axis) < TScalarTraits<Scalar>::Epsilon())
				{
					axis = Cross(TScalarArray<Scalar, 3>{ FUnit<1>{} }, normStart);
				}

				Scalar halfTheta = FMath::Radians(Scalar{ 180 }) * Scalar(0.5);
				TScalarArray<Scalar, 3> normalizedAxis = Normalize(axis) * Sin(halfTheta);

				return TScalarQuaternion<Scalar>{
					normalizedAxis.X,
						normalizedAxis.Y,
						normalizedAxis.Z,
						Cos(halfTheta)
				};
			}
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> FromMatrix(const TScalarMatrix<Scalar, 3, 3>& a) noexcept
		{
			Scalar t = Trace(a);

			if (IsPositive(t))
			{
				Scalar d = Sqrt(t + Scalar{ 1 });
				Scalar s = Scalar(0.5) / d;

				return TScalarQuaternion<Scalar>{(a[2][1] - a[1][2])* s,
					(a[0][2] - a[2][0])* s,
					(a[1][0] - a[0][1])* s,
					d* Scalar{ 0.5 }};
			}
			else
			{
				TScalarQuaternion<Scalar> result;

				int i = a[0][0] < a[1][1] ? (a[1][1] < a[2][2] ? 2 : 1) : (a[0][0] < a[2][2] ? 2 : 0);
				int j = (i + 1) % 3;
				int k = (i + 2) % 3;

				Scalar d = Sqrt(a[i][i] - a[j][j] - a[k][k] + Scalar{ 1 });
				Scalar s = Scalar{ 0.5 } / d;

				result[i] = d * Scalar{ 0.5 };
				result[j] = (a[j][i] + a[i][j]) * s;
				result[k] = (a[k][i] + a[i][k]) * s;
				result[3] = (a[k][j] - a[j][k]) * s;

				return result;
			}
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> FromMatrix(const TScalarMatrix<Scalar, 4, 4>& m) noexcept
		{
			return FromMatrix(Basis(m));
		}

		template<typename Scalar>
		FORCEINLINE void ToMatrix(TScalarMatrix<Scalar, 4, 4>& m, const TScalarQuaternion<Scalar>& q) noexcept
		{
			m = TScalarMatrix<Scalar, 4, 4>{ FIdentity{} };
			m[0][0] = 1 - 2 * q.Y * q.Y - 2 * q.Z * q.Z;
			m[0][1] = 2 * q.X * q.Y + 2 * q.Z * q.W;
			m[0][2] = 2 * q.X * q.Z - 2 * q.Y * q.W;

			m[1][0] = 2 * q.X * q.Y - 2 * q.Z * q.W;
			m[1][1] = 1 - 2 * q.X * q.X - 2 * q.Z * q.Z;
			m[1][2] = 2 * q.Y * q.Z + 2 * q.X * q.W;

			m[2][0] = 2 * q.X * q.Z + 2 * q.Y * q.W;
			m[2][1] = 2 * q.Y * q.Z - 2 * q.X * q.W;
			m[2][2] = 1 - 2 * q.X * q.X - 2 * q.Y * q.Y;
		}

		template<typename Scalar>
		FORCEINLINE void ToMatrix(TScalarMatrix<Scalar, 3, 3>& m, const TScalarQuaternion<Scalar>& q) noexcept
		{
			m = TScalarMatrix<Scalar, 3, 3>{ FIdentity{} };
			m[0][0] = 1 - 2 * q.Y * q.Y - 2 * q.Z * q.Z;
			m[0][1] = 2 * q.X * q.Y + 2 * q.Z * q.W;
			m[0][2] = 2 * q.X * q.Z - 2 * q.Y * q.W;

			m[1][0] = 2 * q.X * q.Y - 2 * q.Z * q.W;
			m[1][1] = 1 - 2 * q.X * q.X - 2 * q.Z * q.Z;
			m[1][2] = 2 * q.Y * q.Z + 2 * q.X * q.W;

			m[2][0] = 2 * q.X * q.Z + 2 * q.Y * q.W;
			m[2][1] = 2 * q.Y * q.Z - 2 * q.X * q.W;
			m[2][2] = 1 - 2 * q.X * q.X - 2 * q.Y * q.Y;
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> Normalize(const TScalarQuaternion<Scalar>& a) noexcept
		{
			Scalar invLength = Scalar(1) / Length(a);
			return TScalarQuaternion<Scalar>(a.X * invLength, a.Y * invLength, a.Z * invLength, a.W * invLength);
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> LerpAndNormalize(const TScalarQuaternion<Scalar>& a, const TScalarQuaternion<Scalar>& b, Scalar t) noexcept
		{
			Scalar lerpParam = 1 - t;
			return Normalize(a + TScalarQuaternion<Scalar>{ b.X* lerpParam, b.Y* lerpParam, b.Z* lerpParam, b.W* lerpParam  });
		}

		template<typename Scalar>
		FORCEINLINE TScalarQuaternion<Scalar> Slerp(const TScalarQuaternion<Scalar>& a, const TScalarQuaternion<Scalar>& b, Scalar t) noexcept
		{
			TScalarArray<Scalar, 4> shortQx{ b };
			TScalarArray<Scalar, 4> vx{ b };
			TScalarArray<Scalar, 4> vy{ b };
			if (LengthSquared(vx - vy) > LengthSquared(vx + vy))
			{
				shortQx *= Scalar(-1);
			}

			Scalar cosTheta = Dot(vx, shortQx);

			if (cosTheta > Scalar(0.9999))
			{
				return LerpAndNormalize(a, TScalarQuaternion{ shortQx }, t);
			}

			Scalar theta = ACos(cosTheta);
			Scalar sinTheta = ASin(theta);

			return TScalarQuaternion<Scalar>((vx * Sin(Scalar(1) - t) * theta + shortQx * Sin(t * theta)) / sinTheta);
		}

		template<typename Scalar>
		FORCEINLINE void DecomposeAffineMatrix4x4(TScalarArray<Scalar, 3>& scale, TScalarQuaternion<Scalar>& rotation, TScalarArray<Scalar, 3>& translation, const TScalarMatrix<Scalar, 4, 4>& a) noexcept
		{
			translation = TScalarArray<Scalar, 3>{ a[3][0], a[3][1], a[3][2] };

			scale = TScalarArray<Scalar, 3>{ Length(FMath::Row(a, 0).XYZ), Length(FMath::Row(a, 1).XYZ), Length(FMath::Row(a, 2).XYZ) };

			TScalarArray<Scalar, 3> inverseScale{ 1 / scale.X, 1 / scale.Y, 1 / scale.Z };

			rotation = FromMatrix(TScalarMatrix<Scalar, 3, 3>{
				a[0][0] * inverseScale.X, a[0][1] * inverseScale.X, a[0][2] * inverseScale.X,
					a[1][0] * inverseScale.Y, a[1][1] * inverseScale.Y, a[1][2] * inverseScale.Y,
					a[2][0] * inverseScale.Z, a[2][1] * inverseScale.Z, a[2][2] * inverseScale.Z,
			});
		}
	}
}