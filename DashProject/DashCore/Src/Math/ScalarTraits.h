#pragma once

#include <limits>

namespace Dash
{
    #undef min
    #undef max

    template<typename Scalar>
    struct TScalarTraits {};

    template<>
    struct TScalarTraits<uint16>
    {
        static uint16 Pi()
        {
            return 3;
        }

        static uint16 TwoPi()
        {
            return 6;
        }

        static uint16 InvPi()
        {
            return 0;
        }

        static float InvTwoPi()
        {
            return 0;
        }

        static uint16 Infinity()
        {
            return std::numeric_limits<uint16>::infinity();
        }

        static uint16 Epsilon()
        {
            return std::numeric_limits<uint16>::epsilon();
        }

        static uint16 Max()
        {
            return std::numeric_limits<uint16>::max();
        }

        static uint16 Lowest()
        {
            return std::numeric_limits<uint16>::lowest();
        }
    };

    template<>
    struct TScalarTraits<std::size_t>
    {
        static std::size_t Pi()
        {
            return 3;
        }

        static std::size_t TwoPi()
        {
            return 6;
        }

        static std::size_t InvPi()
        {
            return 0;
        }

        static float InvTwoPi()
        {
            return 0;
        }

        static std::size_t Infinity()
        {
            return std::numeric_limits<std::size_t>::infinity();
        }

        static std::size_t Epsilon()
        {
            return std::numeric_limits<std::size_t>::epsilon();
        }

        static std::size_t Max()
        {
            return std::numeric_limits<std::size_t>::max();
        }

        static std::size_t Lowest()
        {
            return std::numeric_limits<std::size_t>::lowest();
        }
    };

    template<>
    struct TScalarTraits<float>
    {
        static float Pi()
        {
            return 3.14159265358979323846f;
        }

        static float TwoPi()
        {
            return 6.283185307179586476925f;
        }

        static float InvPi()
        {
            return 0.318309886183790671537f;
        }

        static float InvTwoPi()
        {
            return 0.1591549430918953357688f;
        }

        static float HalfPi()
        {
            return 1.5707963267948966192313f;
        }

        static float Infinity()
        {
            return std::numeric_limits<float>::infinity();
        }

        static float Epsilon()
        {
            return std::numeric_limits<float>::epsilon();
        }

        static float Max()
        {
            return std::numeric_limits<float>::max();
        }

        static float Lowest()
        {
            return std::numeric_limits<float>::lowest();
        }
    };


    template<>
    struct TScalarTraits<double>
    {
        static double Pi()
        {
            return 3.14159265358979323846;
        }

        static double TwoPi()
        {
            return 6.283185307179586476925;
        }

        static double InvPi()
        {
            return 0.318309886183790671537;
        }

        static double InvTwoPi()
        {
            return 0.1591549430918953357688;
        }

        static double HalfPi()
        {
            return 1.5707963267948966192313216916398;
        }

        static double Infinity()
        {
            return std::numeric_limits<double>::infinity();
        }

        static double Epsilon()
        {
            return std::numeric_limits<double>::epsilon();
        }

        static double Max()
        {
            return std::numeric_limits<double>::max();
        }

        static double Lowest()
        {
            return std::numeric_limits<double>::lowest();
        }
    };


    template<>
    struct TScalarTraits<long double>
    {
        static long double Pi()
        {
            return 3.1415926535897932384626433832795029L;
        }

        static long double TwoPi()
        {
            return 6.283185307179586476925286766559L;
        }

        static long double InvPi()
        {
            return 0.31830988618379067153776752674503L;
        }

        static long double InvTwoPi()
        {
            return 0.15915494309189533576888376337251L;
        }

        static long double HalfPi()
        {
            return 1.5707963267948966192313216916398L;
        }

        static long double Infinity()
        {
            return std::numeric_limits<long double>::infinity();
        }

        static long double Epsilon()
        {
            return std::numeric_limits<long double>::epsilon();
        }

        static long double Max()
        {
            return std::numeric_limits<long double>::max();
        }

        static long double Lowest()
        {
            return std::numeric_limits<long double>::lowest();
        }
    };
}
