#pragma once

//#include "../Consolid/Consolid.h"
#include "../Utility/Assert.h"
#include "ScalarTraits.h"

#include <cmath>
#include <immintrin.h>

namespace Dash {

	#undef min
	#undef max

	namespace FMath {

		#define UPPER_ALIGNMENT(Base,Alignment) ((UINT)(((Base)+((Alignment)-1))&~(Alignment - 1)))

		template <typename Scalar> constexpr Scalar ACos(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar ASin(Scalar x) noexcept;

		template <typename Scalar> constexpr Scalar Atan(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Atan2(Scalar y, Scalar x) noexcept;

		template <typename Scalar> constexpr Scalar Tan(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Tanh(Scalar x) noexcept;

		template <typename Scalar> constexpr Scalar Sin(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Sinh(Scalar x) noexcept;

		template <typename Scalar> constexpr Scalar Cos(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Cosh(Scalar x) noexcept;

		template <typename Scalar> constexpr void SinCos(Scalar x, Scalar& sin, Scalar& cos) noexcept;

		template <typename Scalar> constexpr bool IsNan(Scalar a) noexcept;
		template <typename Scalar> constexpr bool IsInf(Scalar a) noexcept;
		template <typename Scalar> constexpr bool IsFinite(Scalar a) noexcept;
		template <typename Scalar> constexpr bool IsPositive(Scalar a) noexcept;
		template <typename Scalar> constexpr bool IsNegative(Scalar a) noexcept;
		template <typename Scalar> constexpr bool IsZero(Scalar a) noexcept;
		template <typename Scalar> constexpr bool IsLessnn(Scalar a, Scalar b) noexcept;
		template <typename Scalar> constexpr bool IsEqualnn(Scalar a, Scalar b) noexcept;

		template <typename Scalar> constexpr Scalar Sign(Scalar x) noexcept;
		template <typename Scalar> constexpr bool Signbit(Scalar x) noexcept;

		template <typename Scalar> constexpr Scalar Min(Scalar a, Scalar b) noexcept;
		template <typename Scalar> constexpr Scalar Max(Scalar a, Scalar b) noexcept;
		template <typename Scalar> constexpr Scalar Min(Scalar a, Scalar b, Scalar c) noexcept;
		template <typename Scalar> constexpr Scalar Max(Scalar a, Scalar b, Scalar c) noexcept;

		template <typename Scalar> constexpr Scalar& Minimize(Scalar& a, Scalar b) noexcept;
		template <typename Scalar> constexpr Scalar& Maximize(Scalar& a, Scalar b) noexcept;

		template <typename Element, typename Scalar>
		constexpr Element Lerp(const Element& a, const Element& b, Scalar t) noexcept;

		template <typename Scalar> constexpr Scalar Square(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Cube(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Abs(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Saturate(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Clamp(Scalar x, Scalar a, Scalar b) noexcept;
		template <typename Scalar> constexpr Scalar SmoothStep(Scalar a, Scalar b, Scalar x) noexcept;

		template <typename Scalar> constexpr Scalar Ceil(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Floor(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Trunc(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Round(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Frac(Scalar x) noexcept;

		template <typename Scalar> constexpr int TruncToInt(Scalar x);
		template <typename Scalar> constexpr float TruncToFloat(Scalar x);
		template <typename Scalar> constexpr int RoundToInt(Scalar x);
		template <typename Scalar> constexpr float RoundToFloat(Scalar x);
		template <typename Scalar> constexpr int FloorToInt(Scalar x);
		template <typename Scalar> constexpr float FloorToFloat(Scalar x);
		template <typename Scalar> constexpr int CeilToInt(Scalar x);
		template <typename Scalar> constexpr float CeilToFloat(Scalar x);

		template <typename Scalar> constexpr Scalar Fmod(Scalar x, Scalar y) noexcept;
		template <typename Scalar> constexpr Scalar Modf(Scalar x, Scalar* iptr) noexcept;

		template <typename Scalar> constexpr Scalar Frexp(Scalar x, Scalar* exp) noexcept;
		template <typename Scalar> constexpr Scalar Ldexp(Scalar x, int exp) noexcept;

		template <typename Scalar> constexpr Scalar Exp(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Log(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Log2(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Log10(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar Sqrt(Scalar x) noexcept;
		template <typename Scalar> constexpr Scalar RSqrt(Scalar a) noexcept;
		template <typename Scalar> constexpr Scalar Cbrt(Scalar a) noexcept;
		template <typename Scalar> constexpr Scalar Pow(Scalar base, Scalar exp) noexcept;

		template <typename To, typename From> To Bitcast(From from) noexcept;

		template <typename Scalar> Scalar Radians(Scalar degs) noexcept;
		template <typename Scalar> Scalar Degrees(Scalar rads) noexcept;

		template <typename Scalar> constexpr void Swap(Scalar& x, Scalar& y) noexcept;

		template <typename Scalar> constexpr Scalar ModAngle(Scalar rad) noexcept;

		template <typename T> T AlignUpWithMask(T value, size_t mask) noexcept;

		template <typename T> T AlignDownWithMask(T value, size_t mask) noexcept;

		template <typename T> T AlignUp(T value, size_t alignment) noexcept;

		template <typename T> T AlignDown(T value, size_t alignment) noexcept;

		template <typename T> bool IsAligned(T value, size_t alignment) noexcept;

		template <typename T> T DivideByMultiple(T value, size_t alignment) noexcept;

		template <typename T> bool IsPowerOfTwo(T value) noexcept;

		template <typename T> bool IsDivisible(T value, T divisor) noexcept;

		static FORCEINLINE int Rand() { return rand(); }

		/** Seeds global random number functions Rand() and FRand() */
		static FORCEINLINE void RandInit(int Seed) { srand(Seed); }

		/** Returns a random float between 0 and 1, inclusive. */
		static FORCEINLINE float FRand() { return Rand() / (float)RAND_MAX; }

		// Modulo the range of the given angle such that -XM_PI <= Angle < XM_PI
		

		template<typename Scalar>
		FORCEINLINE constexpr Scalar ACos(Scalar x) noexcept
		{
			return std::acos(Clamp(x, Scalar{ -1 }, Scalar{ 1 }));
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar ASin(Scalar x) noexcept
		{
			return std::asin(Clamp(x, Scalar{ -1 }, Scalar{ 1 }));
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Atan(Scalar x) noexcept
		{
			return std::atan(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Atan2(Scalar y, Scalar x) noexcept
		{
			return std::atan2(y, x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Tan(Scalar x) noexcept
		{
			return std::tan(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Tanh(Scalar x) noexcept
		{
			return std::tanh(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Sin(Scalar x) noexcept
		{
			return std::sin(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Sinh(Scalar x) noexcept
		{
			return std::sinh(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Cos(Scalar x) noexcept
		{
			return std::cos(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Cosh(Scalar x) noexcept
		{
			return std::cosh(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr void SinCos(Scalar x, Scalar& sin, Scalar& cos) noexcept
		{
			sin = Sin(x);
			cos = Cos(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr bool IsNan(Scalar a) noexcept
		{
			return std::isnan(a);
		}

		template<typename Scalar>
		FORCEINLINE constexpr bool IsInf(Scalar a) noexcept
		{
			return std::isinf(a);
		}

		template<typename Scalar>
		FORCEINLINE constexpr bool IsFinite(Scalar a) noexcept
		{
			return std::isfinite(a);
		}

		template<typename Scalar>
		FORCEINLINE constexpr bool IsPositive(Scalar a) noexcept
		{
			return a > Scalar{};
		}

		template<typename Scalar>
		FORCEINLINE constexpr bool IsNegative(Scalar a) noexcept
		{
			return a < Scalar{};
		}

		template<typename Scalar>
		FORCEINLINE constexpr bool IsZero(Scalar a) noexcept
		{
			return a == Scalar{};
		}

		template<typename Scalar>
		FORCEINLINE constexpr bool IsLessnn(Scalar a, Scalar b) noexcept
		{
			ASSERT(!Signbit(a) && !Signbit(b));
			return a < b;
		}

		template<typename Scalar>
		FORCEINLINE constexpr bool IsEqualnn(Scalar a, Scalar b) noexcept
		{
			ASSERT(!Signbit(a) && !Signbit(b));
			return a == b;
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Sign(Scalar x) noexcept
		{
			return IsPositive(x) ? Scalar{ 1 } : (IsZero(x) ? Scalar{} : Scalar{ -1 });
		}

		template<typename Scalar>
		FORCEINLINE constexpr bool Signbit(Scalar x) noexcept
		{
			return (Scalar{ 1 } / x) < Scalar{};
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Min(Scalar a, Scalar b) noexcept
		{
			return a < b ? a : b;
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Max(Scalar a, Scalar b) noexcept
		{
			return a > b ? a : b;
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Min(Scalar a, Scalar b, Scalar c) noexcept
		{
			return Min(Min(a, b), c);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Max(Scalar a, Scalar b, Scalar c) noexcept
		{
			return Max(Max(a, b), c);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar& Minimize(Scalar& a, Scalar b) noexcept
		{
			if (b < a)
			{
				a = b;
			}
			return a;
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar& Maximize(Scalar& a, Scalar b) noexcept
		{
			if (a < b)
			{
				a = b;
			}
			return a;
		}

		template<typename Element, typename Scalar>
		FORCEINLINE constexpr Element Lerp(const Element& a, const Element& b, Scalar t) noexcept
		{
			return a * (Scalar{ 1 } -t) + b * t;
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Square(Scalar x) noexcept
		{
			return x * x;
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Cube(Scalar x) noexcept
		{
			return x * x * x;
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Abs(Scalar x) noexcept
		{
			return std::abs(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Saturate(Scalar x) noexcept
		{
			return Clamp(x, Scalar{}, Scalar{ 1 });
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Clamp(Scalar x, Scalar a, Scalar b) noexcept
		{
			ASSERT(a <= b);
			return x < a ? a : (x > b ? b : x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar SmoothStep(Scalar a, Scalar b, Scalar x) noexcept
		{
			ASSERT(a <= b);
			x = Saturate((x - a) / (b - a));
			return (Scalar{ 3 } -Scalar{ 2 } *x) * x * x;
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Ceil(Scalar x) noexcept
		{
			return std::ceil(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Floor(Scalar x) noexcept
		{
			return std::floor(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Trunc(Scalar x) noexcept
		{
			return static_cast<Scalar>(std::trunc(x));
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Round(Scalar x) noexcept
		{
			return std::round(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Frac(Scalar x) noexcept
		{
			return x - Trunc(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr int TruncToInt(Scalar x)
		{
			return static_cast<int>(Trunc(x));
		}

		template<typename Scalar>
		FORCEINLINE constexpr float TruncToFloat(Scalar x)
		{
			return static_cast<float>(Trunc(x));
		}

		template<typename Scalar>
		FORCEINLINE constexpr int RoundToInt(Scalar x)
		{
			return static_cast<int>(Round(x));
		}

		template<typename Scalar>
		FORCEINLINE constexpr float RoundToFloat(Scalar x)
		{
			return static_cast<float>(Round(x));
		}

		template<typename Scalar>
		FORCEINLINE constexpr int FloorToInt(Scalar x)
		{
			return static_cast<int>(Floor(x));
		}

		template<typename Scalar>
		FORCEINLINE constexpr float FloorToFloat(Scalar x)
		{
			return static_cast<float>(Floor(x));
		}

		template<typename Scalar>
		FORCEINLINE constexpr int CeilToInt(Scalar x)
		{
			return static_cast<int>(Ceil(x));
		}

		template<typename Scalar>
		FORCEINLINE constexpr float CeilToFloat(Scalar x)
		{
			return static_cast<float>(Ceil(x));
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Fmod(Scalar x, Scalar y) noexcept
		{
			return std::fmod(x, y);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Modf(Scalar x, Scalar* iptr) noexcept
		{
			return std::modf(x, iptr);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Frexp(Scalar x, Scalar* exp) noexcept
		{
			return std::frexp(x, exp);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Ldexp(Scalar x, int exp) noexcept
		{
			return std::ldexp(x, exp);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Exp(Scalar x) noexcept
		{
			return std::exp(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Log(Scalar x) noexcept
		{
			return std::log(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Log2(Scalar x) noexcept
		{
			return std::log2(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Log10(Scalar x) noexcept
		{
			return std::log10(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Sqrt(Scalar x) noexcept
		{
			ASSERT(IsPositive(x));
			return std::sqrt(x);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar RSqrt(Scalar a) noexcept
		{
			ASSERT(IsPositive(a));
			return Scalar{ 1 } / Sqrt(a);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Cbrt(Scalar a) noexcept
		{
			ASSERT(IsPositive(a));
			return std::cbrt(a);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar Pow(Scalar base, Scalar exp) noexcept
		{
			return std::pow(base, exp);
		}

		template<typename To, typename From>
		FORCEINLINE To Bitcast(From from) noexcept
		{
			STATIC_ASSERT(sizeof(To) == sizeof(From));

			union { To to; From from; } pun{};
			pun.from = from;
			return pun.to;
		}

		template<typename Scalar>
		FORCEINLINE Scalar Radians(Scalar degs) noexcept
		{
			static const Scalar RADS_PER_DEG = TScalarTraits<Scalar>::Pi() / Scalar(180);
			return RADS_PER_DEG * degs;
		}

		template<typename Scalar>
		FORCEINLINE Scalar Degrees(Scalar rads) noexcept
		{
			static const Scalar DEGS_PER_RAD = Scalar(180) / TScalarTraits<Scalar>::Pi();
			return DEGS_PER_RAD * rads;
		}

		template<typename Scalar>
		FORCEINLINE constexpr void Swap(Scalar& x, Scalar& y) noexcept
		{
			return std::swap(x, y);
		}

		template<typename Scalar>
		FORCEINLINE constexpr Scalar ModAngle(Scalar rad) noexcept
		{
			// Note: The modulo is performed with unsigned math only to work
			// around a precision error on numbers that are close to PI

			// Normalize the range from 0.0f to XM_2PI
			rad = rad + TScalarTraits<Scalar>::Pi();
			// Perform the modulo, unsigned
			float fTemp = Abs(rad);
			fTemp = fTemp - (TScalarTraits<Scalar>::TwoPi() * static_cast<float>(static_cast<int32>(fTemp / TScalarTraits<Scalar>::TwoPi())));
			// Restore the number to the range of -XM_PI to XM_PI-epsilon
			fTemp = fTemp - TScalarTraits<Scalar>::Pi();
			// If the modulo'd value was negative, restore negation
			if (rad < 0.0f) {
				fTemp = -fTemp;
			}
			return fTemp;
		}

		template <typename T> 
		FORCEINLINE T AlignUpWithMask(T value, size_t mask) noexcept
		{
			return (T)(((size_t)value + mask) & ~mask);
		}

		template <typename T> 
		FORCEINLINE T AlignDownWithMask(T value, size_t mask) noexcept
		{
			return (T)((size_t)value & ~mask);
		}

		template <typename T> 
		FORCEINLINE T AlignUp(T value, size_t alignment) noexcept
		{
			return AlignUpWithMask(value, alignment - 1);
		}

		template <typename T> 
		FORCEINLINE T AlignDown(T value, size_t alignment) noexcept
		{
			return AlignDownWithMask(value, alignment - 1);
		}

		template <typename T> 
		FORCEINLINE bool IsAligned(T value, size_t alignment) noexcept
		{
			return 0 == ((size_t)value & (alignment - 1));
		}

		template <typename T> 
		FORCEINLINE T DivideByMultiple(T value, size_t alignment) noexcept
		{
			return (T)((value + alignment - 1) / alignment);
		}

		template <typename T> 
		FORCEINLINE bool IsPowerOfTwo(T value) noexcept
		{
			return 0 == (value & (value - 1));
		}

		template <typename T> 
		FORCEINLINE bool IsDivisible(T value, T divisor) noexcept
		{
			return (value / divisor) * divisor == value;
		}
	}
}
