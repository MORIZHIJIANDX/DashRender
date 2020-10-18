#pragma once

#include "../Math/MathType.h"

namespace Dash
{
	struct FViewport
	{
		explicit FViewport(Scalar x = 0, Scalar y = 0, Scalar width = 512, Scalar height = 512,
			Scalar minDepth = 0, Scalar maxDepth = 1)
			: TopLeftX(x)
			, TopLeftY(y)
			, Width(width)
			, Height(height)
			, MinDepth(minDepth)
			, MaxDepth(maxDepth)
		{}

		Scalar TopLeftX;
		Scalar TopLeftY;
		Scalar Width;
		Scalar Height;
		Scalar MinDepth;
		Scalar MaxDepth;
	};
}