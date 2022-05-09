#include "PCH.h"

#include "ThreadSafeQueue.h"
#include "LogStream.h"


namespace Dash
{
	struct LogMessage
	{
		ELogLevel mLevel = ELogLevel::Info;
		std::wstring mMessage;
	};

	static std::vector<std::shared_ptr<FLogStream>> gsLogStreams;
	static std::mutex gsLogStreamMutex;
	static std::thread gsLogThread;
	static std::atomic_bool gsProcessMessage = true;
	static TThreadSafeQueue<LogMessage> gsMessageQueue;

	// ******************
	// FLogString
	//

	FLogString::~FLogString()
	{
		mLogger.Log(mLogLevel, str());
	}


	FLogString FLogManager::operator()(ELogLevel level)
	{
		return FLogString(*this, level);
	}


	// ******************
	// FLogManager
	//

	void ProcessMessagesFunc()
	{
		while (gsProcessMessage)
		{
			LogMessage msg;
			while (gsMessageQueue.TryPop(msg))
			{
				std::scoped_lock lock(gsLogStreamMutex);
				for (auto log : gsLogStreams)
				{
					log->Write(msg.mLevel, msg.mMessage);
				}
			}

			std::this_thread::yield();
		}
	}

	void FLogManager::Init()
	{
		gsLogThread = std::thread(&ProcessMessagesFunc);
	}

	void FLogManager::Shutdown()
	{
		while (!gsMessageQueue.Empty())
		{
		}

		gsProcessMessage = false;

		if (gsLogThread.joinable())
		{
			gsLogThread.join();
		}

		UnregisterAllStreams();
	}

	void FLogManager::RegisterLogStream(std::shared_ptr<FLogStream> logStream)
	{
		std::scoped_lock lock(gsLogStreamMutex);
		gsLogStreams.push_back(logStream);
	}

	void FLogManager::UnregisterLogStream(std::shared_ptr<FLogStream> logStream)
	{
		std::scoped_lock lock(gsLogStreamMutex);

		auto iter = std::find(gsLogStreams.begin(), gsLogStreams.end(), logStream);

		if (iter != gsLogStreams.end())
		{
			gsLogStreams.erase(iter);
		}
	}

	void FLogManager::UnregisterAllStreams()
	{
		std::scoped_lock lock(gsLogStreamMutex);
		gsLogStreams.clear();
	}

	void FLogManager::Log(ELogLevel level, std::wstring logInfo)
	{
		std::wstring msg = logInfo + L"\n";
		gsMessageQueue.Push({ level, msg });
	}

}