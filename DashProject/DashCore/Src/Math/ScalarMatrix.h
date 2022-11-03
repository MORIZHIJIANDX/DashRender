#pragma once

#include "ScalarArray.h"

namespace Dash
{

	template<typename Scalar, std::size_t M, std::size_t N>
	struct TScalarMatrix
	{
	public:
		using DataType = TScalarArray<Scalar, N>;

		using ScalarType = Scalar;
		using SizeType = std::size_t;

		constexpr TScalarMatrix() noexcept;
		constexpr explicit TScalarMatrix(FZero) noexcept;
		constexpr explicit TScalarMatrix(FIdentity) noexcept;
		constexpr explicit TScalarMatrix(std::initializer_list<Scalar> list) noexcept;
		template <typename Scalar2> constexpr explicit TScalarMatrix(const Scalar2* v) noexcept;

		operator const TScalarArray<Scalar, N>* () const noexcept;
		operator TScalarArray<Scalar, N>* () noexcept;

		TScalarMatrix<Scalar, M, N>& operator=(FZero) noexcept;
		TScalarMatrix<Scalar, M, N>& operator+=(FZero) noexcept;
		TScalarMatrix<Scalar, M, N>& operator-=(FZero) noexcept;
		TScalarMatrix<Scalar, M, N>& operator*=(FZero) noexcept;

		TScalarMatrix<Scalar, M, N>& operator=(FIdentity) noexcept;

		TScalarMatrix<Scalar, M, N>& operator=(Scalar s) noexcept;
		TScalarMatrix<Scalar, M, N>& operator*=(Scalar s) noexcept;
		TScalarMatrix<Scalar, M, N>& operator/=(Scalar s) noexcept;

		TScalarMatrix<Scalar, M, N>& operator+=(const TScalarMatrix<Scalar, M, N>& a) noexcept;
		TScalarMatrix<Scalar, M, N>& operator-=(const TScalarMatrix<Scalar, M, N>& a) noexcept;

	private:
		TScalarArray<Scalar, N> mRows[M];
	};









	// Non-member Operators 

	// --Declaration-- //


#ifdef USE_OSTREAM

	template <typename CharT, typename Traits, typename Scalar, std::size_t M, std::size_t N>
	std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TScalarMatrix<Scalar, M, N>& a);

#endif

#ifdef USE_ISTREAM

	template <typename CharT, typename Traits, typename Scalar, std::size_t M, std::size_t N>
	std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, TScalarMatrix<Scalar, M, N>& a);

#endif

	template <typename Scalar, std::size_t M, std::size_t N> bool operator==(const TScalarMatrix<Scalar, M, N>& a, const TScalarMatrix<Scalar, M, N>& b) noexcept;

	template <typename Scalar, std::size_t M, std::size_t N> TScalarMatrix<Scalar, M, N> operator-(const TScalarMatrix<Scalar, M, N>& a) noexcept;

	template <typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator+(const TScalarMatrix<Scalar1, M, N>& a, const TScalarMatrix<Scalar2, M, N>& b) noexcept;

	template <typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator-(const TScalarMatrix<Scalar1, M, N>& a, const TScalarMatrix<Scalar2, M, N>& b) noexcept;

	template <typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator*(const TScalarMatrix<Scalar1, M, N>& a, const TScalarMatrix<Scalar2, M, N>& b);

	template <typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator*(const TScalarMatrix<Scalar1, M, N>& a, Scalar2 s) noexcept;

	template <typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator*(Scalar1 s, const TScalarMatrix<Scalar2, M, N>& a) noexcept;

	template <typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator/(const TScalarMatrix<Scalar1, M, N>& a, Scalar2 s) noexcept;









	// Non-member Function

	// --Declaration-- //

	namespace FMath
	{
		template <typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
		TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, M> Mul(const TScalarMatrix<Scalar1, M, N>& a, const TScalarArray<Scalar2, N>& v) noexcept;

		template <typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
		TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> Mul(const TScalarArray<Scalar1, M>& v, const TScalarMatrix<Scalar2, M, N>& a) noexcept;

		template<typename Scalar1, typename Scalar2, std::size_t M, std::size_t N, std::size_t P>
		TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, P> Mul(const TScalarMatrix<Scalar1, M, N>& a, const TScalarMatrix<Scalar2, N, P>& b) noexcept;

		template <typename Scalar, std::size_t M, std::size_t N>
		TScalarArray<Scalar, N> Row(const TScalarMatrix<Scalar, M, N>& a, std::size_t i);

		template <typename Scalar, std::size_t M, std::size_t N>
		TScalarArray<Scalar, M> Column(const TScalarMatrix<Scalar, M, N>& a, std::size_t i);

		template <typename Scalar, std::size_t M, std::size_t N>
		TScalarMatrix<Scalar, N, M> Transpose(const TScalarMatrix<Scalar, M, N>& a);
	}






	// Member Function

	// --Implementation-- //

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE constexpr TScalarMatrix<Scalar, M, N>::TScalarMatrix() noexcept
	{
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE constexpr TScalarMatrix<Scalar, M, N>::TScalarMatrix(FZero) noexcept
	{
		for (std::size_t i = 0; i < M; i++)
		{
			mRows[i] = FZero{};
		}
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE constexpr TScalarMatrix<Scalar, M, N>::TScalarMatrix(FIdentity) noexcept
	{
		for (std::size_t i = 0; i < M; i++)
		{
			for (std::size_t j = 0; j < N; j++)
			{
				if (i == j)
				{
					mRows[i][j] = Scalar{ 1 };
				}
				else
				{
					mRows[i][j] = Scalar{ };
				}
			}
		}
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE constexpr TScalarMatrix<Scalar, M, N>::TScalarMatrix(std::initializer_list<Scalar> list) noexcept
	{
		ASSERT((M * N) == list.size());

		auto listIter = list.begin();

		for (std::size_t i = 0; i < M; i++)
		{
			for (std::size_t j = 0; j < N; j++)
			{
				mRows[M][N] = *listIter;
				++listIter;
			}
		}
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	template<typename Scalar2>
	FORCEINLINE constexpr TScalarMatrix<Scalar, M, N>::TScalarMatrix(const Scalar2* v) noexcept
	{
		ASSERT(v != nullptr);

		for (std::size_t i = 0; i < M; i++)
		{
			for (std::size_t j = 0; j < N; j++)
			{
				mRows[M][N] = v[i * M + j];
			}
		}
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>::operator const TScalarArray<Scalar, N>* () const noexcept
	{
		return mRows;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>::operator TScalarArray<Scalar, N>* () noexcept
	{
		return mRows;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>& TScalarMatrix<Scalar, M, N>::operator=(FZero zero) noexcept
	{
		for (std::size_t i = 0; i < M; i++)
		{
			mRows[i] = zero;
		}

		return *this;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>& TScalarMatrix<Scalar, M, N>::operator+=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>& TScalarMatrix<Scalar, M, N>::operator-=(FZero) noexcept
	{
		return *this;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>& TScalarMatrix<Scalar, M, N>::operator*=(FZero zero) noexcept
	{
		for (std::size_t i = 0; i < M; i++)
		{
			mRows[i] = zero;
		}

		return *this;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>& TScalarMatrix<Scalar, M, N>::operator=(FIdentity) noexcept
	{
		for (std::size_t i = 0; i < M; i++)
		{
			for (std::size_t j = 0; j < N; j++)
			{
				if (i == j)
				{
					mRows[i][j] = Scalar{ 1 };
				}
				else
				{
					mRows[i][j] = Scalar{ };
				}
			}
		}

		return *this;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>& TScalarMatrix<Scalar, M, N>::operator=(Scalar s) noexcept
	{
		for (std::size_t i = 0; i < M; i++)
		{
			for (std::size_t j = 0; j < N; j++)
			{
				if (i == j)
				{
					mRows[i][j] = s;
				}
				else
				{
					mRows[i][j] = Scalar{ };
				}
			}
		}

		return *this;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>& TScalarMatrix<Scalar, M, N>::operator*=(Scalar s) noexcept
	{
		for (std::size_t i = 0; i < M; i++)
		{
			mRows[i] *= s;
		}

		return *this;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>& TScalarMatrix<Scalar, M, N>::operator/=(Scalar s) noexcept
	{
		for (std::size_t i = 0; i < M; i++)
		{
			mRows[i] /= s;
		}

		return *this;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>& TScalarMatrix<Scalar, M, N>::operator+=(const TScalarMatrix<Scalar, M, N>& a) noexcept
	{
		for (std::size_t i = 0; i < M; i++)
		{
			mRows[i] += a[i];
		}

		return *this;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N>& TScalarMatrix<Scalar, M, N>::operator-=(const TScalarMatrix<Scalar, M, N>& a) noexcept
	{
		for (std::size_t i = 0; i < M; i++)
		{
			mRows[i] -= a[i];
		}

		return *this;
	}









	// Non-member Operators 

	// --Implementation-- //

#ifdef USE_OSTREAM
	template<typename CharT, typename Traits, typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const TScalarMatrix<Scalar, M, N>& a)
	{
		for (std::size_t i = 0; i < M; i++)
		{
			os << FMath::Row(a, i);
			if (i != M - 1)
			{
				os << ' ';
			}
		}
		return os;
	}
#endif

#ifdef USE_ISTREAM
	template<typename CharT, typename Traits, typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, TScalarMatrix<Scalar, M, N>& a)
	{
		for (std::size_t i = 0; i < M; i++)
		{
			is >> a[i];
		}

		return is;
	}
#endif

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE bool operator==(const TScalarMatrix<Scalar, M, N>& a, const TScalarMatrix<Scalar, M, N>& b) noexcept
	{
		for (std::size_t i = 0; i < M; i++)
		{
			for (std::size_t j = 0; j < N; j++)
			{
				if (a[i][j] != b[i][j])
				{
					return false;
				}
			}
		}

		return true;
	}

	template<typename Scalar, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<Scalar, M, N> operator-(const TScalarMatrix<Scalar, M, N>& a) noexcept
	{
		TScalarMatrix<Scalar, M, N> result;

		for (std::size_t i = 0; i < M; i++)
		{
			result[i] = -a[i];
		}

		return result;
	}

	template<typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator+(const TScalarMatrix<Scalar1, M, N>& a, const TScalarMatrix<Scalar2, M, N>& b) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		TScalarMatrix<RT, M, N> result;

		for (std::size_t i = 0; i < M; i++)
		{
			result[i] = a[i] + b[i];
		}

		return result;
	}

	template<typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator-(const TScalarMatrix<Scalar1, M, N>& a, const TScalarMatrix<Scalar2, M, N>& b) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		TScalarMatrix<RT, M, N> result;

		for (std::size_t i = 0; i < M; i++)
		{
			result[i] = a[i] - b[i];
		}

		return result;
	}

	template<typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator*(const TScalarMatrix<Scalar1, M, N>& a, const TScalarMatrix<Scalar2, M, N>& b)
	{
		//using RT = typename TPromote<Scalar1, Scalar2>::RT;
		//TScalarMatrix<RT, M, N> result;

		//for (std::size_t i = 0; i < M; i++)
		//{
		//	result[i] = a[i] * b[i];
		//}

		//return result;

		return FMath::Mul(a, b);
	}

	template<typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator*(const TScalarMatrix<Scalar1, M, N>& a, Scalar2 s) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		TScalarMatrix<RT, M, N> result;

		for (std::size_t i = 0; i < M; i++)
		{
			result[i] = a[i] * s;
		}

		return result;
	}

	template<typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator*(Scalar1 s, const TScalarMatrix<Scalar2, M, N>& a) noexcept
	{
		return a * s;
	}

	template<typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
	FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, N> operator/(const TScalarMatrix<Scalar1, M, N>& a, Scalar2 s) noexcept
	{
		using RT = typename TPromote<Scalar1, Scalar2>::RT;
		TScalarMatrix<RT, M, N> result;

		for (std::size_t i = 0; i < M; i++)
		{
			result[i] = a[i] / s;
		}

		return result;
	}








	// Non-member Function

	// --Implementation-- //

	namespace FMath
	{

		template<typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
		FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, M> Mul(const TScalarMatrix<Scalar1, M, N>& a, const TScalarArray<Scalar2, N>& v) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			TScalarArray<RT, M> result;

			for (std::size_t i = 0; i < M; i++)
			{
				result[i] = a[i] * v;
			}

			return result;
		}

		template<typename Scalar1, typename Scalar2, std::size_t M, std::size_t N>
		FORCEINLINE TScalarArray<typename TPromote<Scalar1, Scalar2>::RT, N> Mul(const TScalarArray<Scalar1, M>& v, const TScalarMatrix<Scalar2, M, N>& a) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			TScalarArray<RT, N> result;

			//for (std::size_t i = 0; i < M; i++)
			//{
			//	result[i] = v * Column(a, i);
			//}

			for (std::size_t i = 0; i < M; i++)
			{
				result += v[i] * a[i];
			}

			return result;
		}

		template<typename Scalar1, typename Scalar2, std::size_t M, std::size_t N, std::size_t P>
		FORCEINLINE TScalarMatrix<typename TPromote<Scalar1, Scalar2>::RT, M, P> Mul(const TScalarMatrix<Scalar1, M, N>& a, const TScalarMatrix<Scalar2, N, P>& b) noexcept
		{
			using RT = typename TPromote<Scalar1, Scalar2>::RT;
			TScalarMatrix<RT, M, P> result;

			for (std::size_t i = 0; i < M; i++)
			{
				for (std::size_t j = 0; j < P; j++)
				{
					result[i][j] = Dot(a[i], Column(b, j));
				}
			}

			return result;
		}

		template<typename Scalar, std::size_t M, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, N> Row(const TScalarMatrix<Scalar, M, N>& a, std::size_t i)
		{
			return a[i];
		}

		template<typename Scalar, std::size_t M, std::size_t N>
		FORCEINLINE TScalarArray<Scalar, M> Column(const TScalarMatrix<Scalar, M, N>& a, std::size_t i)
		{
			TScalarArray<Scalar, M> result;

			for (std::size_t j = 0; j < M; j++)
			{
				for (std::size_t k = 0; k < N; k++)
				{
					if (k == i)
					{
						result[j] = a[j][k];
					}
				}
			}

			return result;
		}

		template<typename Scalar, std::size_t M, std::size_t N>
		FORCEINLINE TScalarMatrix<Scalar, N, M> Transpose(const TScalarMatrix<Scalar, M, N>& a)
		{
			TScalarMatrix<Scalar, N, M> result;

			for (std::size_t i = 0; i < M; i++)
			{
				for (std::size_t j = 0; j < N; j++)
				{
					result[i][j] = a[j][i];
				}
			}

			return result;
		}
	}
}

#include "Matrix3x3.h"
#include "Matrix4x4.h"