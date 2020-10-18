#pragma once


#ifdef _DEBUG
#define USE_ASSERTS 1
#else
#define USE_ASSERTS 0
#endif

namespace Dash {
	namespace Assert
	{
		enum class FailBehavior
		{
			Break,
			Continue,
		};

		typedef FailBehavior(*Handler)(const char* condition,
			const char* msg,
			const char* file,
			int line);

		Handler GetHandler();
		void SetHandler(Handler newHandler);

		FailBehavior ReportFailure(const char* condition,
			const char* file,
			int line,
			const char* msg, ...);
	}
}

#define ASSERT_BREAK() __debugbreak()
#define ASSERT_UNUSED(x) do { (void)sizeof(x); } while(0)

#ifdef USE_ASSERTS
#define DASH_DEBUG_ASSERT(cond) \
		do \
		{ \
			if (!(cond)) \
			{ \
				if (Dash::Assert::ReportFailure(#cond, __FILE__, __LINE__, 0) == \
					Dash::Assert::FailBehavior::Break) \
					ASSERT_BREAK(); \
			} \
		} while(0)

#define DASH_DEBUG_ASSERT_MSG(cond, msg, ...) \
		do \
		{ \
			if (!(cond)) \
			{ \
				if (Dash::Assert::ReportFailure(#cond, __FILE__, __LINE__, (msg), __VA_ARGS__) == \
					Dash::Assert::FailBehavior::Break) \
					ASSERT_BREAK(); \
			} \
		} while(0)

#define DASH_DEBUG_ASSERT_FAIL(msg, ...) \
		do \
		{ \
			if (Dash::Assert::ReportFailure(0, __FILE__, __LINE__, (msg), __VA_ARGS__) == \
				Dash::Assert::FailBehavior::Break) \
				ASSERT_BREAK(); \
		} while(0)

#else
#define DASH_DEBUG_ASSERT(condition) \
		do { ASSERT_UNUSED(condition); } while(0)
#define DASH_DEBUG_ASSERT_MSG(condition, msg, ...) \
		do { ASSERT_UNUSED(condition); ASSERT_UNUSED(msg); } while(0)
#define DASH_DEBUG_ASSERT_FAIL(msg, ...) \
		do { ASSERT_UNUSED(msg); } while(0)
#endif


#if USE_ASSERTS
#define ASSERT(x) DASH_DEBUG_ASSERT(x)
#define ASSERT_MSG(x, msg, ...) DASH_DEBUG_ASSERT_MSG(x, msg, __VA_ARGS__)
#define ASSERT_FAIL(msg, ...) DASH_DEBUG_ASSERT_FAIL(msg, __VA_ARGS__)
#else
#define ASSERT(x)
#define ASSERT_MSG(x, msg, ...)
#define ASSERT_FAIL(msg, ...)
#endif

#if USE_ASSERTS
#define STATIC_ASSERT(x) static_assert(x, "Compile-time assertion failed: " #x);
#define STATIC_ASSERT_MSG(x, msg) static_assert(x, "Compile-time assertion failed: " #x ", " msg);
#else
#define STATIC_ASSERT(x)
#define STATIC_ASSERT_MSG(x, msg)
#endif
