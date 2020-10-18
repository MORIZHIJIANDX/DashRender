#pragma once

#include <sstream>
#include "LogEnums.h"
#include "../DesignPatterns/Singleton.h"

#define LOG_SIMPLE 1;

namespace Dash
{
	class FLogManager;
	class FLogStream;

	class FLogString : public std::wostringstream
	{
	public:
		FLogString(FLogManager& logger, ELogLevel level)
			: mLogger(logger), mLogLevel(level)
		{}

		FLogString(const FLogString& ls)
			: mLogger(ls.mLogger), mLogLevel(ls.mLogLevel)
		{}

		~FLogString();

	private:
		FLogManager& mLogger;
		ELogLevel mLogLevel;
	};

	class FLogManager : public TSingleton<FLogManager>
	{
		friend class FLogString;
	public:
		FLogManager() {}

		virtual ~FLogManager() {};

		FLogString operator()(ELogLevel level);

		void Init();
		void Shutdown();

		void RegisterLogStream(std::shared_ptr<FLogStream> logStream);
		void UnregisterLogStream(std::shared_ptr<FLogStream> logStream);
		void UnregisterAllStreams();

	private:
		void Log(ELogLevel level, std::wstring logInfo);
	};
}

#ifdef LOG_SIMPLE
	#define LOG_INFO (Dash::FLogManager::Get()->operator()(Dash::ELogLevel::Info)) << "  [Info]: "
	#define LOG_WARNING (Dash::FLogManager::Get()->operator()(Dash::ELogLevel::Warning)) << "  [Warning]: "
	#define LOG_ERROR (Dash::FLogManager::Get()->operator()(Dash::ELogLevel::Error)) << "  [Error]: "
#else
	#define LOG_INFO (Dash::FLogManager::Get()->operator()(Dash::ELogLevel::Info)) << "[File]: " << __FILE__ << "  [Line]: " << __LINE__ << "  [Function]: " << __FUNCTION__ << "  [Info]: "
	#define LOG_WARNING (Dash::FLogManager::Get()->operator()(Dash::ELogLevel::Warning)) << "[File]: " << __FILE__ << "  [Line]: " << __LINE__ << "  [Function]: " << __FUNCTION__ << "  [Warning]: "
	#define LOG_ERROR (Dash::FLogManager::Get()->operator()(Dash::ELogLevel::Error)) << "[File]: " << __FILE__ << "  [Line]: " << __LINE__ << "  [Function]: " << __FUNCTION__ << "  [Error]: "
#endif // LOG_SIMPLE




