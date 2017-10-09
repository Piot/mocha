#ifndef mocha_log_h
#define mocha_log_h

#include <stdio.h>
#include <stdlib.h>

#include <tyran/tyran_log.h>

#if !defined TORNADO_OS_WEBASSEMBLY
#define MOCHA_BREAK abort()
#else
#define MOCHA_BREAK

#endif

#define MOCHA_ERROR(...)                                                                                                                                                                                                                                       \
	TYRAN_ERROR(__VA_ARGS__);                                                                                                                                                                                                                              \
	MOCHA_BREAK;

#define MOCHA_ASSERT(expression, ...)                                                                                                                                                                                                                          \
	if (!(expression)) {                                                                                                                                                                                                                                       \
		MOCHA_LOG_ERROR(__VA_ARGS__);                                                                                                                                                                                                                          \
		MOCHA_BREAK;                                                                                                                                                                                                                                           \
	}

#define MOCHA_OUTPUT(...)                                                                                                                                                                                                                                      \
	{                                                                                                                                                                                                                                                          \
		char _temp_str[1024];                                                                                                                                                                                                                                  \
		size_t _characters_written = (size_t) sprintf(_temp_str, __VA_ARGS__);                                                                                                                                                                                             \
		fwrite(_temp_str, 1, _characters_written, stdout);                                                                                                                                                                                                     \
		fflush(stdout);                                                                                                                                                                                                                                        \
	}

#define MOCHA_LOG(...)  TYRAN_LOG_INFO(__VA_ARGS__);
#define MOCHA_LOG_INFO(...) TYRAN_LOG_INFO(__VA_ARGS__);
#define MOCHA_LOG_WARN(...) TYRAN_LOG_WARN(__VA_ARGS__);
#define MOCHA_LOG_ERROR(...) TYRAN_LOG_ERROR(__VA_ARGS__);

#endif
