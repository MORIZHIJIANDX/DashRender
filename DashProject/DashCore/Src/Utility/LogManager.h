#pragma once

#include "DesignPatterns/Singleton.h"
#include "Utility/BitwiseEnum.h"
#include "Utility/StringUtility.h"
#include "Utility/Assert.h"
#include <format>

namespace Dash
{
	enum class ELogLevel : uint8
	{
		None = 0,
		Debug =  1 << 0,
		Info = 1 << 1,
		Warning = 1 << 2,
		Error = 1 << 3,
		Fatal = 1 << 4,
		All = Debug | Info | Warning | Error | Fatal,
	};

	ENABLE_BITMASK_OPERATORS(ELogLevel);

	class ILogSink
	{
	public:
		virtual ~ILogSink() {};
		virtual void Log(ELogLevel level, std::string_view category, const std::string& log) = 0;
		virtual void Flush() {};
	};

	struct FLogCategory
	{
	public:
		FLogCategory(std::string_view name, ELogLevel level)
			: mCategoryName (name)
			, mDefaultLogLevel(level)
		{}

		std::string_view GetCategoryName() const { return mCategoryName; }
		ELogLevel GetDefaultLogLevel() const { return mDefaultLogLevel; }

		bool IsPermitted(ELogLevel level) { return (level & ELogLevel::All) <= mDefaultLogLevel; }

	private:
		std::string mCategoryName;
		ELogLevel mDefaultLogLevel;
	};

	class FLogManager : public TSingleton<FLogManager>
	{
	public:
		FLogManager() {}

		virtual ~FLogManager() {};

		void Init();
		void Shutdown();

		template<typename T, typename... TArgs>
		void Log(ELogLevel level, std::string_view category, std::string_view file, uint32 line, const T& Message, TArgs&&... Args)
		{
			DispatchLog(level, category, file, line, Message, std::forward<TArgs>(Args)...);
		}

	private:
		template<typename... Args>
		std::string string_format(const char* fmt, Args&&... args) {
			// 第一次调用：查询需要的长度
			int n = std::snprintf(nullptr, 0, fmt, std::forward<Args>(args)...);
			if (n < 0) ASSERT("swprintf formatting error");
			std::vector<char> buf(n + 1);
			int ret = std::snprintf(buf.data(), buf.size(), fmt, std::forward<Args>(args)...);
			if (ret < 0) ASSERT("swprintf formatting error");
			return std::string(buf.data(), static_cast<size_t>(n));
		}

		template<typename... Args>
		std::wstring wstring_format(const wchar_t* fmt, Args&&... args) {
			int n = std::swprintf(nullptr, 0, fmt, std::forward<Args>(args)...);
			if (n < 0) ASSERT("swprintf formatting error");
			std::vector<wchar_t> buf(static_cast<size_t>(n) + 1);
			int ret = std::swprintf(buf.data(), buf.size(), fmt, std::forward<Args>(args)...);
			if (ret < 0) ASSERT("swprintf formatting error");
			return std::wstring(buf.data(), static_cast<size_t>(n));
		}

		template<typename... TArgs>
		void DispatchLog(ELogLevel level, std::string_view category, std::string_view file, uint32 line, std::string_view Message, TArgs&&... Args)
		{
			//std::string Buffer = string_format(Message.data(), std::forward<TArgs>(Args)...);
			std::string buffer = std::vformat(Message, std::make_format_args(Args...));
			Write(level, category, file, line, buffer);
		}

		template<typename... TArgs>
		void DispatchLog(ELogLevel level, std::string_view category, std::string_view file, uint32 line, std::wstring_view Message, TArgs&&... Args)
		{
			//std::wstring WideBuffer = wstring_format(Message.data(), std::forward<TArgs>(Args)...);
			std::wstring wideBuffer = std::vformat(Message, std::make_wformat_args(Args...));
			std::string	buffer = FStringUtility::WideStringToUTF8(wideBuffer);
			Write(level, category, file, line, buffer);
		}

		void Write(ELogLevel level, std::string_view category, std::string_view file, uint32 line, std::string_view log);

	private:
		std::vector<ILogSink*> mLogSinks;
	};

	#define LOG_CONCATENATE(a, b) a##b

	#define DECLARE_LOG_CATEGORY(Name, Level)					\
		struct FLogCategory##Name : public FLogCategory			\
		{														\
			FLogCategory##Name()								\
			: FLogCategory(#Name, ELogLevel::Level)				\
			{													\
			}													\
		};														\
		extern FLogCategory##Name LOG_CONCATENATE(GLog, Name);	

	#define DEFINE_LOG_CATEGORY(Name)	 FLogCategory##Name LOG_CONCATENATE(GLog, Name)

	#define DASH_LOG(Name, Level, ...)																										\
		if(LOG_CONCATENATE(GLog, Name).IsPermitted(ELogLevel::##Level))																		\
		{																																	\
			FLogManager::Get()->Log(ELogLevel::##Level, LOG_CONCATENATE(GLog, Name).GetCategoryName(), __FILE__, __LINE__, __VA_ARGS__);	\
		}
}






