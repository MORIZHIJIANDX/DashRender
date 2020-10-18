#pragma once

namespace Dash
{
	enum class ELogLevel : unsigned int
	{
		Info = (1 << 0),
		Error = (1 << 1),
		Warning = (1 << 2),
	};
}