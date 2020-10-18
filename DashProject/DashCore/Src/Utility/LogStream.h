#pragma once

#include <fstream>

namespace Dash
{
	class FLogStream
	{
	public:
		virtual void Write(ELogLevel level, std::wstring logInfo) = 0;
	};


	class FLogStreamFile : public FLogStream
	{
	public:
		FLogStreamFile(std::wstring fileName);
		virtual ~FLogStreamFile();

		virtual void Write(ELogLevel level, std::wstring logInfo) override;

	private:
		std::wofstream mFileStream;
		std::mutex mFileMutex;
	};


	class FLogStreamConsole : public FLogStream
	{
	public:
		FLogStreamConsole();
		~FLogStreamConsole() = default;

		virtual void Write(ELogLevel level, std::wstring logInfo) override;
	};


	class FLogStreamVS : public FLogStream
	{
	public:
		FLogStreamVS() = default;
		~FLogStreamVS() = default;

		virtual void Write(ELogLevel level, std::wstring logInfo) override;
	};


	class FLogStreamMessageBox : public FLogStream
	{
	public:
		FLogStreamMessageBox(ELogLevel filter);
		~FLogStreamMessageBox() = default;

		virtual void Write(ELogLevel level, std::wstring logInfo) override;

	private:
		ELogLevel mLevelFilter;
	};

}