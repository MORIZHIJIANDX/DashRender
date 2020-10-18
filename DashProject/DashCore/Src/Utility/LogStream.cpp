#include "PCH.h"

#include "LogStream.h"
#include <corecrt_io.h>
#include <fcntl.h>

namespace Dash
{
	// ******************
// FLogStreamFile
//

	FLogStreamFile::FLogStreamFile(std::wstring fileName)
		: mFileStream(fileName, std::fstream::out)
	{
	}

	FLogStreamFile::~FLogStreamFile()
	{
		if (mFileStream.is_open())
		{
			mFileStream.close();
		}
	}

	void FLogStreamFile::Write(ELogLevel level, std::wstring logInfo)
	{
		std::scoped_lock lock(mFileMutex);
		if (mFileStream.is_open())
		{
			mFileStream << logInfo;
			mFileStream.flush();
		}
	}


	// ******************
	// FLogStreamConsole
	//

	static const int MAX_CONSOLE_LINES = 500;

	FLogStreamConsole::FLogStreamConsole()
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

	void FLogStreamConsole::Write(ELogLevel level, std::wstring logInfo)
	{
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		HANDLE hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
		// Get the console info so we can restore it after.
		GetConsoleScreenBufferInfo(hConOut, &consoleInfo);

		switch (level)
		{
		case ELogLevel::Info:
			SetConsoleTextAttribute(hConOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
			std::wcout << logInfo;
			break;
		case ELogLevel::Warning:
			SetConsoleTextAttribute(hConOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			std::wcout << logInfo;
			break;
		case ELogLevel::Error:
			SetConsoleTextAttribute(hConOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
			std::wcout << logInfo;
			break;
		default:
			std::wcout << logInfo;
			break;
		}

		// Restore console attributes.
		SetConsoleTextAttribute(hConOut, consoleInfo.wAttributes);
	}


	// ******************
	// FLogStreamVS
	//

	void FLogStreamVS::Write(ELogLevel level, std::wstring logInfo)
	{
		OutputDebugStringW(logInfo.c_str());
	}


	// ******************
	// FLogStreamMessageBox
	//

	FLogStreamMessageBox::FLogStreamMessageBox(ELogLevel filter)
		: mLevelFilter(filter)
	{
	}

	void FLogStreamMessageBox::Write(ELogLevel level, std::wstring logInfo)
	{
		std::wstring caption;
		UINT uType = 0;

		switch (level)
		{
		case ELogLevel::Info:
			caption = L"Information";
			uType = MB_ICONINFORMATION;
			break;
		case ELogLevel::Warning:
			caption = L"Warning";
			uType = MB_ICONWARNING;
			break;
		case ELogLevel::Error:
			caption = L"Error";
			uType = MB_ICONERROR;
			break;
		}

		if ((static_cast<uint32_t>(mLevelFilter) & static_cast<uint32_t>(level)) != 0)
		{
			MessageBoxW(NULL, logInfo.c_str(), caption.c_str(), uType);
		}
	}

}