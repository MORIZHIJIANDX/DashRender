#pragma once

#include "Scalar.h"
#include "Algebra.h"

namespace Dash
{
	template<typename Scalar>
	struct BitMask
	{
	public:
		BitMask();
		explicit BitMask(uint32 bits);
		explicit BitMask(FZero);
		template<uint32 I> BitMask(FUnit<I>);

		operator uint32() const;
		bool operator[](int i) const;

	private:
		uint32 mBits;
	};
}