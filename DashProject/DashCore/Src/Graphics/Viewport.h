#pragma once

#include "../Math/MathType.h"
#include "d3dx12.h"

namespace Dash
{
	struct FViewport : public D3D12_VIEWPORT
	{
		explicit FViewport(Scalar x = 0, Scalar y = 0, Scalar width = 512, Scalar height = 512,
			Scalar minDepth = 0, Scalar maxDepth = 1)
		{
			TopLeftX = x;
			TopLeftY = y;
			Width = width;
			Height = height;
			MinDepth = minDepth;
			MaxDepth = maxDepth;
		}
	};
}