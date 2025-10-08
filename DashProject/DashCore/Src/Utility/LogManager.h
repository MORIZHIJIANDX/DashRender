#pragma once

#define LOGURU_FILENAME_WIDTH  24
#define LOGURU_WITH_STREAMS     1
#define LOGURU_REDEFINE_ASSERT  1
#define LOGURU_USE_FMTLIB       0
#define LOGURU_WITH_FILEABS     0

#include "DesignPatterns/Singleton.h"

namespace Dash
{
	class FLogManager : public TSingleton<FLogManager>
	{
	public:
		FLogManager() {}

		virtual ~FLogManager() {};

		void Init();
		void Shutdown();
	};
}

#define LOG_INFO std::cout
#define LOG_WARNING std::cout
#define LOG_ERROR std::cout





