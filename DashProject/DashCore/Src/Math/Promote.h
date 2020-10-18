#pragma once

namespace Dash 
{
	template<typename Scalar1, typename Scalar2>
	struct TPromote
	{
		using RT = Scalar1;
	};

	template<>
	struct TPromote<int, float>
	{
		using RT = float;
	};

	template<>
	struct TPromote<int, double>
	{
		using RT = double;
	};

	template<>
	struct TPromote<int, long double>
	{
		using RT = long double;
	};

	template<>
	struct TPromote<int, long long>
	{
		using RT = long long;
	};

	template<>
	struct TPromote<float, double>
	{
		using RT = double;
	};

	template<>
	struct TPromote<float, long double>
	{
		using RT = long double;
	};

	template<>
	struct TPromote<double, long double>
	{
		using RT = long double;
	};
}
