#pragma once

#define LOGURU_FILENAME_WIDTH  24
#define LOGURU_WITH_STREAMS     1
#define LOGURU_REDEFINE_ASSERT  1
#define LOGURU_USE_FMTLIB       0
#define LOGURU_WITH_FILEABS     0

#include "DesignPatterns/Singleton.h"
#include "ThirdParty/loguru/loguru.h"

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

#define LOG_INFO LOG_S(INFO)
#define LOG_WARNING LOG_S(WARNING)
#define LOG_ERROR LOG_S(ERROR)





