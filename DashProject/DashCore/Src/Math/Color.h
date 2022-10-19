#pragma once

#include "Algebra.h"
#include "Scalar.h"
#include "ScalarTraits.h"
#include <cstdint>

namespace Dash
{
	template<typename Scalar, std::size_t N>
	class TScalarArray;

	class FFloat16Color;

	struct FColor;

	/**
	 * Enum for the different kinds of gamma spaces we expect to need to convert from/to.
	 */
	enum class EGammaSpace : uint8_t
	{
		/** No gamma correction is applied to this space, the incoming colors are assumed to already be in linear space. */
		Linear,
		/** a simplified sRGB gamma correction is applied, pow(1/2.2). */
		Pow22,
		/** Use the standard sRGB conversion. */
		sRGB,
	};


	/**
	 * a linear, 32-bit/component floating point RGBA color.
	 */
	struct FLinearColor
	{
		union
		{
			struct { float r, g, b, a; };
			float Data[4];
		};
		

		/** Static lookup table used for FColor -> FLinearColor conversion. Pow(2.2) */
		static float Pow22OneOver255Table[256];

		/** Static lookup table used for FColor -> FLinearColor conversion. sRGB */
		static float sRGBToLinearTable[256];

		FORCEINLINE FLinearColor()
			: r(0), g(0), b(0), a(0)
		{}
		FORCEINLINE explicit FLinearColor(FZero)
			: r(0), g(0), b(0), a(0)
		{}
		constexpr FORCEINLINE FLinearColor(float InR, float InG, float InB, float InA = 1.0f) : r(InR), g(InG), b(InB), a(InA) {}

		/**
		 * Converts an FColor which is assumed to be in sRGB space, into linear color space.
		 * @param Color The sRGB color that needs to be converted into linear space.
		 */
		FLinearColor(const FColor& Color);

		FLinearColor(const TScalarArray<float, 3>& Vector);

		explicit FLinearColor(const TScalarArray<float, 4>& Vector);

		// Conversions.
		FColor ToRGBE() const;

		/**
		 * Converts an FColor coming from an observed sRGB output, into a linear color.
		 * @param Color The sRGB color that needs to be converted into linear space.
		 */
		static FLinearColor FromSRGBColor(const FColor& Color);

		/**
		 * Converts an FColor coming from an observed Pow(1/2.2) output, into a linear color.
		 * @param Color The Pow(1/2.2) color that needs to be converted into linear space.
		 */
		static FLinearColor FromPow22Color(const FColor& Color);

		// Operators.

		FORCEINLINE float& operator[](int32_t Index)
		{
			return Data[Index];
		}

		FORCEINLINE const float& operator[](int32_t Index) const
		{
			return Data[Index];
		}

		FORCEINLINE FLinearColor operator+(const FLinearColor& ColorB) const
		{
			return FLinearColor(
				this->r + ColorB.r,
				this->g + ColorB.g,
				this->b + ColorB.b,
				this->a + ColorB.a
			);
		}
		FORCEINLINE FLinearColor& operator+=(const FLinearColor& ColorB)
		{
			r += ColorB.r;
			g += ColorB.g;
			b += ColorB.b;
			a += ColorB.a;
			return *this;
		}

		FORCEINLINE FLinearColor operator-(const FLinearColor& ColorB) const
		{
			return FLinearColor(
				this->r - ColorB.r,
				this->g - ColorB.g,
				this->b - ColorB.b,
				this->a - ColorB.a
			);
		}
		FORCEINLINE FLinearColor& operator-=(const FLinearColor& ColorB)
		{
			r -= ColorB.r;
			g -= ColorB.g;
			b -= ColorB.b;
			a -= ColorB.a;
			return *this;
		}

		FORCEINLINE FLinearColor operator*(const FLinearColor& ColorB) const
		{
			return FLinearColor(
				this->r * ColorB.r,
				this->g * ColorB.g,
				this->b * ColorB.b,
				this->a * ColorB.a
			);
		}
		FORCEINLINE FLinearColor& operator*=(const FLinearColor& ColorB)
		{
			r *= ColorB.r;
			g *= ColorB.g;
			b *= ColorB.b;
			a *= ColorB.a;
			return *this;
		}

		FORCEINLINE FLinearColor operator*(float Scalar) const
		{
			return FLinearColor(
				this->r * Scalar,
				this->g * Scalar,
				this->b * Scalar,
				this->a * Scalar
			);
		}

		FORCEINLINE FLinearColor& operator*=(float Scalar)
		{
			r *= Scalar;
			g *= Scalar;
			b *= Scalar;
			a *= Scalar;
			return *this;
		}

		FORCEINLINE FLinearColor operator/(const FLinearColor& ColorB) const
		{
			return FLinearColor(
				this->r / ColorB.r,
				this->g / ColorB.g,
				this->b / ColorB.b,
				this->a / ColorB.a
			);
		}
		FORCEINLINE FLinearColor& operator/=(const FLinearColor& ColorB)
		{
			r /= ColorB.r;
			g /= ColorB.g;
			b /= ColorB.b;
			a /= ColorB.a;
			return *this;
		}

		FORCEINLINE FLinearColor operator/(float Scalar) const
		{
			const float	InvScalar = 1.0f / Scalar;
			return FLinearColor(
				this->r * InvScalar,
				this->g * InvScalar,
				this->b * InvScalar,
				this->a * InvScalar
			);
		}
		FORCEINLINE FLinearColor& operator/=(float Scalar)
		{
			const float	InvScalar = 1.0f / Scalar;
			r *= InvScalar;
			g *= InvScalar;
			b *= InvScalar;
			a *= InvScalar;
			return *this;
		}

		// clamped in 0..1 range
		FORCEINLINE FLinearColor GetClamped(float InMin = 0.0f, float InMax = 1.0f) const
		{
			FLinearColor Ret;

			Ret.r = FMath::Clamp(r, InMin, InMax);
			Ret.g = FMath::Clamp(g, InMin, InMax);
			Ret.b = FMath::Clamp(b, InMin, InMax);
			Ret.a = FMath::Clamp(a, InMin, InMax);

			return Ret;
		}

		/** Comparison operators */
		FORCEINLINE bool operator==(const FLinearColor& ColorB) const
		{
			return this->r == ColorB.r && this->g == ColorB.g && this->b == ColorB.b && this->a == ColorB.a;
		}
		FORCEINLINE bool operator!=(const FLinearColor& Other) const
		{
			return this->r != Other.r || this->g != Other.g || this->b != Other.b || this->a != Other.a;
		}

		// Error-tolerant comparison.
		FORCEINLINE bool Equals(const FLinearColor& ColorB, float Tolerance = TScalarTraits<float>::Epsilon()) const
		{
			return FMath::Abs(this->r - ColorB.r) < Tolerance && FMath::Abs(this->g - ColorB.g) < Tolerance && FMath::Abs(this->b - ColorB.b) < Tolerance && FMath::Abs(this->a - ColorB.a) < Tolerance;
		}

		FLinearColor CopyWithNewOpacity(float NewOpacicty) const
		{
			FLinearColor NewCopy = *this;
			NewCopy.a = NewOpacicty;
			return NewCopy;
		}

		//UE_DEPRECATED(4.22, "FGetHSV doesn't perform a valid HSV conversion, use MakeFromHSV8 instead")
		//	static FLinearColor FGetHSV(uint8_t H, uint8_t S, uint8_t V);

		/**
		 * Converts byte hue-saturation-brightness to floating point red-green-blue.
		 */
		static FLinearColor MakeFromHSV8(uint8_t H, uint8_t S, uint8_t V);

		/**
		* Makes a random but quite nice color.
		*/
		static FLinearColor MakeRandomColor();

		/**
		* Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
		*/
		static FLinearColor MakeFromColorTemperature(float Temp);

		/**
		 * Euclidean distance between two points.
		 */
		static inline float Dist(const FLinearColor& V1, const FLinearColor& V2)
		{
			return FMath::Sqrt(FMath::Square(V2.r - V1.r) + FMath::Square(V2.g - V1.g) + FMath::Square(V2.b - V1.b) + FMath::Square(V2.a - V1.a));
		}

		/** Converts a linear space RGB color to an HSV color */
		FLinearColor LinearRGBToHSV() const;

		/** Converts an HSV color to a linear space RGB color */
		FLinearColor HSVToLinearRGB() const;

		/**
		 * Linearly interpolates between two colors by the specified progress amount.  The interpolation is performed in HSV color space
		 * taking the shortest path to the new color's hue.  This can give better results than FMath::Lerp(), but is much more expensive.
		 * The incoming colors are in RGB space, and the output color will be RGB.  The alpha value will also be interpolated.
		 *
		 * @param	From		The color and alpha to interpolate from as linear RGBA
		 * @param	To			The color and alpha to interpolate to as linear RGBA
		 * @param	Progress	Scalar interpolation amount (usually between 0.0 and 1.0 inclusive)
		 * @return	The interpolated color in linear RGB space along with the interpolated alpha value
		 */
		static FLinearColor LerpUsingHSV(const FLinearColor& From, const FLinearColor& To, const float Progress);

		/** Quantizes the linear color and returns the result as a FColor.  This bypasses the SRGB conversion. */
		FColor Quantize() const;

		/** Quantizes the linear color with rounding and returns the result as a FColor.  This bypasses the SRGB conversion. */
		FColor QuantizeRound() const;

		/** Quantizes the linear color and returns the result as a FColor with optional sRGB conversion and quality as goal. */
		FColor ToFColor(const bool bSRGB) const;

		/**
		 * Returns a desaturated color, with 0 meaning no desaturation and 1 == full desaturation
		 *
		 * @param	Desaturation	Desaturation factor in range [0..1]
		 * @return	Desaturated color
		 */
		FLinearColor Desaturate(float Desaturation) const;

		/** Computes the perceptually weighted luminance value of a color. */
		inline float ComputeLuminance() const
		{
			return r * 0.3f + g * 0.59f + b * 0.11f;
		}

		/**
		 * Returns the maximum value in this color structure
		 *
		 * @return The maximum color channel value
		 */
		FORCEINLINE float GetMax() const
		{
			return FMath::Max(FMath::Max(FMath::Max(r, g), b), a);
		}

		/** useful to detect if a light contribution needs to be rendered */
		bool IsAlmostBlack() const
		{
			return FMath::Square(r) < TScalarTraits<float>::Epsilon() && FMath::Square(g) < TScalarTraits<float>::Epsilon() && FMath::Square(b) < TScalarTraits<float>::Epsilon();
		}

		/**
		 * Returns the minimum value in this color structure
		 *
		 * @return The minimum color channel value
		 */
		FORCEINLINE float GetMin() const
		{
			return FMath::Min(FMath::Min(FMath::Min(r, g), b), a);
		}

		FORCEINLINE float GetLuminance() const
		{
			return r * 0.3f + g * 0.59f + b * 0.11f;
		}

		 // Common colors.	
		static const FLinearColor White;
		static const FLinearColor Gray;
		static const FLinearColor Black;
		static const FLinearColor Transparent;
		static const FLinearColor Red;
		static const FLinearColor Green;
		static const FLinearColor Blue;
		static const FLinearColor Yellow;
	};

	FORCEINLINE FLinearColor operator*(float Scalar, const FLinearColor& Color)
	{
		return Color.operator*(Scalar);
	}

#ifdef USE_OSTREAM

	template<typename CharT, typename Traits>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const FLinearColor& v)
	{
		os << "r=" << v.r << ",g=" << v.g << ",b=" << v.b << ",a=" << v.a;
		return os;
	}

#endif // USE_OSTREAM

#ifdef USE_ISTREAM

	template<typename CharT, typename Traits>
	std::basic_istream<CharT, Traits>& operator>>(std::basic_ostream<CharT, Traits>& is, const FLinearColor& v)
	{
		is >> v.r >> v.g >> v.b >> v.a;
		return is;
	}

#endif // USE_ISTREAM


	struct FColor
	{
	public:
		// Variables.
#if PLATFORM_LITTLE_ENDIAN
#ifdef _MSC_VER
	// Win32 x86
		union { struct { uint8_t b, g, r, a; }; uint32_t AlignmentDummy; };
#else
	// Linux x86, etc
		uint8_t b GCC_ALIGN(4);
		uint8_t g, r, a;
#endif
#else // PLATFORM_LITTLE_ENDIAN
		union { struct { uint8_t a, r, g, b; }; uint32_t AlignmentDummy; };
#endif

		uint32_t& DWColor(void) { return *((uint32_t*)this); }
		const uint32_t& DWColor(void) const { return *((uint32_t*)this); }

		// Constructors.
		FORCEINLINE FColor() 
			: r(0), g(0), b(0), a(0)
		{}
		FORCEINLINE explicit FColor(FZero)
		{
			// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
			r = g = b = a = 0;
		}
		constexpr FORCEINLINE FColor(uint8_t InR, uint8_t InG, uint8_t InB, uint8_t InA = 255)
			// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
			: a(InA), r(InR), g(InG), b(InB)
		{}

		FORCEINLINE explicit FColor(const TScalarArray<uint8_t, 4>& InColor);

		FORCEINLINE explicit FColor(uint32_t InColor)
		{
			DWColor() = InColor;
		}

		// Operators.
		FORCEINLINE bool operator==(const FColor& C) const
		{
			return DWColor() == C.DWColor();
		}

		FORCEINLINE bool operator!=(const FColor& C) const
		{
			return DWColor() != C.DWColor();
		}

		FORCEINLINE void operator+=(const FColor& C)
		{
			r = (uint8_t)FMath::Min((int32_t)r + (int32_t)C.r, 255);
			g = (uint8_t)FMath::Min((int32_t)g + (int32_t)C.g, 255);
			b = (uint8_t)FMath::Min((int32_t)b + (int32_t)C.b, 255);
			a = (uint8_t)FMath::Min((int32_t)a + (int32_t)C.a, 255);
		}

		FLinearColor FromRGBE() const;

		/**
		 * Creates a color value from the given hexadecimal string.
		 *
		 * Supported formats are: RGB, RRGGBB, RRGGBBAA, #RGB, #RRGGBB, #RRGGBBAA
		 *
		 * @param HexString - The hexadecimal string.
		 * @return The corresponding color value.
		 * @see ToHex
		 */
		static FColor FromHex(const std::string& HexString);

		/**
		 * Makes a random but quite nice color.
		 */
		static FColor MakeRandomColor();

		/**
		 * Makes a color red->green with the passed in scalar (e.g. 0 is red, 1 is green)
		 */
		static FColor MakeRedToGreenColorFromScalar(float Scalar);

		/**
		* Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
		*/
		static FColor MakeFromColorTemperature(float Temp);

		/**
		 *	@return a new FColor based of this color with the new alpha value.
		 *	Usage: const FColor& MyColor = FColorList::Green.WithAlpha(128);
		 */
		FColor WithAlpha(uint8_t Alpha) const
		{
			return FColor(r, g, b, Alpha);
		}

		/**
		 * Reinterprets the color as a linear color.
		 *
		 * @return The linear color representation.
		 */
		FORCEINLINE FLinearColor ReinterpretAsLinear() const
		{
			return FLinearColor(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
		}

		/**
		 * Converts this color value to a hexadecimal string.
		 *
		 * The format of the string is RRGGBBAA.
		 *
		 * @return Hexadecimal string.
		 * @see FromHex, ToString
		 */
		 //FORCEINLINE std::string ToHex() const
		 //{
		 //	return std::string::Printf(TEXT("%02X%02X%02X%02X"), r, g, b, a);
		 //}

		 ///**
		 // * Converts this color value to a string.
		 // *
		 // * @return The string representation.
		 // * @see ToHex
		 // */
		 //FORCEINLINE std::string ToString() const
		 //{
		 //	return std::string::Printf(TEXT("(r=%i,g=%i,b=%i,a=%i)"), r, g, b, a);
		 //}

		 /**
		  * Initialize this Color based on an std::string. The String is expected to contain r=, g=, b=, a=.
		  * The FColor will be bogus when InitFromString returns false.
		  *
		  * @param	InSourceString	std::string containing the color values.
		  * @return true if the r,g,b values were read successfully; false otherwise.
		  */
		  //bool InitFromString(const std::string& InSourceString)
		  //{
		  //	r = g = b = 0;
		  //	a = 255;

		  //	// The initialization is only successful if the r, g, and b values can all be parsed from the string
		  //	const bool bSuccessful = FParse::Value(*InSourceString, TEXT("r="), r) && FParse::Value(*InSourceString, TEXT("g="), g) && FParse::Value(*InSourceString, TEXT("b="), b);

		  //	// Alpha is optional, so don't factor in its presence (or lack thereof) in determining initialization success
		  //	FParse::Value(*InSourceString, TEXT("a="), a);

		  //	return bSuccessful;
		  //}

		  /**
		   * Gets the color in a packed uint32_t format packed in the order ARGB.
		   */
		FORCEINLINE uint32_t ToPackedARGB() const
		{
			return (a << 24) | (r << 16) | (g << 8) | (b << 0);
		}

		/**
		 * Gets the color in a packed uint32_t format packed in the order ABGR.
		 */
		FORCEINLINE uint32_t ToPackedABGR() const
		{
			return (a << 24) | (b << 16) | (g << 8) | (r << 0);
		}

		/**
		 * Gets the color in a packed uint32_t format packed in the order RGBA.
		 */
		FORCEINLINE uint32_t ToPackedRGBA() const
		{
			return (r << 24) | (g << 16) | (b << 8) | (a << 0);
		}

		/**
		 * Gets the color in a packed uint32_t format packed in the order BGRA.
		 */
		FORCEINLINE uint32_t ToPackedBGRA() const
		{
			return (b << 24) | (g << 16) | (r << 8) | (a << 0);
		}

		/** Some pre-inited colors, useful for debug code */
		static const FColor White;
		static const FColor Black;
		static const FColor Transparent;
		static const FColor Red;
		static const FColor Green;
		static const FColor Blue;
		static const FColor Yellow;
		static const FColor Cyan;
		static const FColor Magenta;
		static const FColor Orange;
		static const FColor Purple;
		static const FColor Turquoise;
		static const FColor Silver;
		static const FColor Emerald;

	private:
		/**
		 * Please use .ToFColor(true) on FLinearColor if you wish to convert from FLinearColor to FColor,
		 * with proper sRGB conversion applied.
		 *
		 * Note: Do not implement or make public.  We don't want people needlessly and implicitly converting between
		 * FLinearColor and FColor.  It's not a free conversion.
		 */
		explicit FColor(const FLinearColor& LinearColor);
	};


	FORCEINLINE uint32_t GetTypeHash(const FColor& Color)
	{
		return Color.DWColor();
	}


	/** Computes a brightness and a fixed point color from a floating point color. */
	extern void ComputeAndFixedColorAndIntensity(const FLinearColor& InLinearColor, FColor& OutColor, float& OutIntensity);

}

