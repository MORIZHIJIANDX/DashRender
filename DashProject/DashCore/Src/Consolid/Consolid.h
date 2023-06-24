#pragma once

#include <cstddef>
#include <assert.h>
#include <cinttypes>
#include <cstring>

#include "DashWinAPI.h"

//#include "../Utility/Assert.h"
//#include "../Utility/LogManager.h"


//#if defined(__SSE__) || (_M_IX86_FP >= 1) || defined(_M_X64)
//#   define USE_SSE 1
//#endif

#define USE_IEEE_754 1
#define USE_ROUNDING_CONTROL 0
#define INDEX_NONE -1

#ifndef FORCEINLINE
#   if (_MSC_VER >= 1200)
#      define FORCEINLINE __forceinline
#   elif defined(__GNUC__) 
#      define FORCEINLINE __attribute__((__always_inline__)) inline
#   else
#      define FORCEINLINE inline
#   endif
#endif

#ifndef UNUSED
#   define UNUSED(arg) 
#endif

template<typename T>
void WriteData(const T& src, void* dest, std::size_t offset = 0)
{
	std::memcpy(static_cast<uint8_t*>(dest) + offset, &src, sizeof(T));
}

template<typename T>
void GetData(T& dest, void* src, std::size_t offset = 0)
{
	std::memcpy(&dest, static_cast<uint8_t*>(src) + offset, sizeof(T));
}

const DWORD MS_VC_EXCEPTION = 0x406D1388;

// Set the name of a running thread (for debugging)
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

// Helper smart-pointers
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10) || (defined(_XBOX_ONE) && defined(_TITLE)) || !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
struct virtual_deleter { void operator()(void* p) noexcept { if (p) VirtualFree(p, 0, MEM_RELEASE); } };
#endif

//struct aligned_deleter { void operator()(void* p) noexcept { _aligned_free(p); } };

//struct handle_closer { void operator()(HANDLE h) noexcept { if (h) CloseHandle(h); } };

//using ScopedHandle = std::unique_ptr<void, handle_closer>;

//inline HANDLE safe_handle(HANDLE h) noexcept { return (h == INVALID_HANDLE_VALUE) ? nullptr : h; }