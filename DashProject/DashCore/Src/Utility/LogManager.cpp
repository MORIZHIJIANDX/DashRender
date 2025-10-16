#include "PCH.h"
#include "LogManager.h"
#include <corecrt_io.h>
#include <fcntl.h>
#include "FileUtility.h"

namespace Dash
{
	static constexpr std::string_view Eol = "\n";

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

	inline std::string MakePrefix(ELogLevel lvl, std::string_view category, std::string_view file, uint32 line, bool printFileAndLine = false) {
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
		oss << " [TID=" << static_cast<unsigned long>(::GetCurrentThreadId()) << "]";
		if (printFileAndLine)
		{
			oss << " " << file << ":" << line;
		}
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

	static const int MAX_CONSOLE_LINES = 500;

	class FConsoleLogSink : public ILogSink
	{
	public:
		FConsoleLogSink() 
		{
			// Allocate a console. 
			if (AllocConsole())
			{
				HANDLE lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

				// Increase screen buffer to allow more lines of text than the default.
				CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
				GetConsoleScreenBufferInfo(lStdHandle, &consoleInfo);
				consoleInfo.dwSize.Y = MAX_CONSOLE_LINES;
				SetConsoleScreenBufferSize(lStdHandle, consoleInfo.dwSize);
				SetConsoleCursorPosition(lStdHandle, { 0, 0 });

				// Redirect unbuffered STDOUT to the console.
				int hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
				FILE* fp = _fdopen(hConHandle, "w");
				freopen_s(&fp, "CONOUT$", "w", stdout);
				setvbuf(stdout, nullptr, _IONBF, 0);

				// Redirect unbuffered STDIN to the console.
				lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
				hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
				fp = _fdopen(hConHandle, "r");
				freopen_s(&fp, "CONIN$", "r", stdin);
				setvbuf(stdin, nullptr, _IONBF, 0);

				// Redirect unbuffered STDERR to the console.
				lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
				hConHandle = _open_osfhandle((intptr_t)lStdHandle, _O_TEXT);
				fp = _fdopen(hConHandle, "w");
				freopen_s(&fp, "CONOUT$", "w", stderr);
				setvbuf(stderr, nullptr, _IONBF, 0);

				//Clear the error state for each of the C++ standard stream objects. We need to do this, as
				//attempts to access the standard streams before they refer to a valid target will cause the
				//iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
				//to always occur during startup regardless of whether anything has been read from or written to
				//the console or not.
				std::wcout.clear();
				std::cout.clear();
				std::wcerr.clear();
				std::cerr.clear();
				std::wcin.clear();
				std::cin.clear();
			}
		}
		virtual ~FConsoleLogSink() {};

		virtual void Log(ELogLevel level, std::string_view category, const std::string& log) override
		{
			CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
			HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
			// Get the console info so we can restore it after.
			GetConsoleScreenBufferInfo(hConOut, &consoleInfo);

			switch (level)
			{
			case ELogLevel::Info:
				SetConsoleTextAttribute(hConOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
				std::cout << log;
				break;
			case ELogLevel::Warning:
				SetConsoleTextAttribute(hConOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				std::cout << log;
				break;
			case ELogLevel::Error:
				SetConsoleTextAttribute(hConOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
				std::cout << log;
				break;
			default:
				std::cout << log;
				break;
			}

			// Restore console attributes.
			SetConsoleTextAttribute(hConOut, consoleInfo.wAttributes);
		}
	};

	class FFileLogSink : public ILogSink
	{
	public:
		FFileLogSink(std::string fileName)
			: mFileStream(fileName, std::fstream::out)
		{
		};

		virtual ~FFileLogSink() 
		{
			if (mFileStream.is_open())
			{
				mFileStream.flush();
				mFileStream.close();
			}
		};

		virtual void Log(ELogLevel level, std::string_view category, const std::string& log) override
		{
			std::scoped_lock lock(mFileMutex);
			if (mFileStream.is_open())
			{
				mFileStream << log;
			}
		}

		virtual void Flush() override
		{
			mFileStream.flush();
		}

	private:
		std::ofstream mFileStream;
		std::mutex mFileMutex;
	};

	void FLogManager::Init()
	{
		//int argc = 0;
		//LPSTR* argv = CommandLineToArgvA(GetCommandLineA(), &argc);

		mLogSinks.emplace_back(new FVSLogSink());
		mLogSinks.emplace_back(new FConsoleLogSink());

		std::string logFileName = FFileUtility::CombinePath(FFileUtility::GetProjectDir(), "Saved\\log.txt");
		FFileUtility::EnsureFileExist(logFileName);
		mLogSinks.emplace_back(new FFileLogSink(logFileName));
	}

	void FLogManager::Shutdown()
	{
		for (int32 i = 0; i < mLogSinks.size(); i++)
		{
			mLogSinks[i]->Flush();
			delete mLogSinks[i];
		}

		mLogSinks.clear();
	}

	void FLogManager::Write(ELogLevel level, std::string_view category, std::string_view file, uint32 line, std::string_view log)
	{
		std::string	formattedMessage = MakePrefix(level, category, file, line);
		formattedMessage.append(log);
		formattedMessage.append(Eol);

		for (ILogSink* sink : mLogSinks)
		{
			sink->Log(level, category, formattedMessage);
		}

		if (EnumMaskContains(level, ELogLevel::Fatal))
		{
			ASSERT(false);
		}
	}
}