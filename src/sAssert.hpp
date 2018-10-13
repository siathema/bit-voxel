#pragma once
// stolen from https://github.com/gpakosz/Assert
#if defined(_WIN32)
#  define s_debug_break() __debugbreak()
#elif defined(__ORBIS__)
#  define s_debug_break() __builtin_trap()
#elif defined(__clang__)
#  define s_debug_break() __builtin_debugtrap()
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE__)
#  include <signal.h>
#  define s_debug_break() raise(SIGTRAP)
#elif defined(__GNUC__)
#  define s_debug_break() __builtin_trap()
#else
#  define s_debug_break() ((void)0)
#endif

#define s_debug(fmt, ...) fprintf(stderr, "%s:%d: " fmt "\n", __func__, __LINE__, __VA_ARGS__)

inline void s_assert(bool x, const char* reason)
{
	if (!x)
	{
		//fprintf(stderr,"%s", reason);
		s_debug_break();
	}
}
