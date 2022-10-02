#pragma once

#include "PixelBuffer.h"
#include "../Math/Color.h"

namespace Dash
{
	class FColorBuffer : public FPixelBuffer
	{
	public:
		FColorBuffer(const FLinearColor& clearColor = FLinearColor{});

		void Create();

	private:
		FLinearColor mClearColor;
	};
}