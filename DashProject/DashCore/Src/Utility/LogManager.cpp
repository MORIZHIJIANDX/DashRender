#include "PCH.h"
#include "LogManager.h"

namespace Dash
{
	static constexpr std::string_view Eol = "\r\n";

	struct FLogEntry
	{
		ELogLevel Level;
		std::string_view Category;
		std::string Log;
		std::string_view File;
		uint32 Line;
	};

	LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT* pNumArgs)
	{
		int retval;
		retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, NULL, 0);
		if (!SUCCEEDED(retval))
			return NULL;

		LPWSTR lpWideCharStr = (LPWSTR)malloc(retval * sizeof(WCHAR));
		if (lpWideCharStr == NULL)
			return NULL;

		retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, lpWideCharStr, retval);
		if (!SUCCEEDED(retval))
		{
			free(lpWideCharStr);
			return NULL;
		}

		int numArgs;
		LPWSTR* args;
		args = CommandLineToArgvW(lpWideCharStr, &numArgs);
		free(lpWideCharStr);
		if (args == NULL)
			return NULL;

		int storage = numArgs * sizeof(LPSTR);
		for (int i = 0; i < numArgs; ++i)
		{
			BOOL lpUsedDefaultChar = FALSE;
			retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, NULL, 0, NULL, &lpUsedDefaultChar);
			if (!SUCCEEDED(retval))
			{
				LocalFree(args);
				return NULL;
			}

			storage += retval;
		}

		LPSTR* result = (LPSTR*)LocalAlloc(LMEM_FIXED, storage);
		if (result == NULL)
		{
			LocalFree(args);
			return NULL;
		}

		int bufLen = storage - numArgs * sizeof(LPSTR);
		LPSTR buffer = ((LPSTR)result) + numArgs * sizeof(LPSTR);
		for (int i = 0; i < numArgs; ++i)
		{
			assert(bufLen > 0);
			BOOL lpUsedDefaultChar = FALSE;
			retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, buffer, bufLen, NULL, &lpUsedDefaultChar);
			if (!SUCCEEDED(retval))
			{
				LocalFree(result);
				LocalFree(args);
				return NULL;
			}

			result[i] = buffer;
			buffer += retval;
			bufLen -= retval;
		}

		LocalFree(args);

		*pNumArgs = numArgs;
		return result;
	}

	inline const char* ToString(ELogLevel level) {
		switch (level) {
		case ELogLevel::None: return "None";
		case ELogLevel::Debug: return "Debug";
		case ELogLevel::Info:  return "Info";
		case ELogLevel::Warning:  return "Warning";
		case ELogLevel::Error: return "Error";
		case ELogLevel::Fatal: return "Fatal";
		default: return "Unknown";
		}
	}

	inline std::string make_prefix(ELogLevel lvl, std::string_view category, std::string_view file, uint32 line) {
		using namespace std::chrono;
		auto now = system_clock::now();
		auto t = system_clock::to_time_t(now);
		auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

		std::tm tm;
		localtime_s(&tm, &t);

		std::ostringstream oss;
		oss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
		oss << '.' << std::setw(3) << std::setfill('0') << ms.count() << "]";
		oss << " [" << category << "]";
		oss << " [" << ToString(lvl) << "]";
		oss << " {TID=" << static_cast<unsigned long>(::GetCurrentThreadId()) << "}";
		oss << " " << file << ":" << line;
		oss << " - ";
		return oss.str();
	}

	class FVSLogSink : public ILogSink
	{
	public:
		FVSLogSink() {};
		virtual ~FVSLogSink() {};

		virtual void Log(ELogLevel level, std::string_view category, const std::string& log) override
		{
			OutputDebugStringA(log.c_str());
		}
	};

	void FLogManager::Init()
	{
		//int argc = 0;
		//LPSTR* argv = CommandLineToArgvA(GetCommandLineA(), &argc);

		mLogSinks.emplace_back(new FVSLogSink());
	}

	void FLogManager::Shutdown()
	{
		for (int32 i = 0; i < mLogSinks.size(); i++)
		{
			delete mLogSinks[i];
		}

		mLogSinks.clear();
	}

	void FLogManager::Write(ELogLevel level, std::string_view category, std::string_view file, uint32 line, std::string_view log)
	{
		std::string	formattedMessage = make_prefix(level, category, file, line);
		formattedMessage.append(log);
		formattedMessage.append(Eol);

		for (ILogSink* sink : mLogSinks)
		{
			sink->Log(level, category, formattedMessage);
		}
	}
}