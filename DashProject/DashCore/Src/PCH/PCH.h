#pragma once

#include <cstdint>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <filesystem>

#include <thread>
#include <mutex>
#include <atomic>

#include <algorithm>

#include <memory>

#include <vector>
#include <queue>
#include <map>

#include "../Consolid/Consolid.h"
#include "../Utility/LogManager.h"
#include "../Utility/Assert.h"
#include "../Utility/Events.h"

#include "../Math/MathType.h"




static void SetThreadName(std::thread& thread, const char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = GetThreadId(reinterpret_cast<HANDLE>(thread.native_handle()));
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

