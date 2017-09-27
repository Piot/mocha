#ifndef mocha_log_h
#define mocha_log_h

#include <stdio.h>
#include <stdlib.h>

typedef struct mocha_log
{
	void (*log)(const char* string);
	void (*log_info)(const char* string);
} mocha_log;

extern mocha_log g_mocha_log;

#if !defined TORNADO_OS_WEBASSEMBLY
#define MOCHA_BREAK abort()
#else
#define MOCHA_BREAK

#endif

#define MOCHA_ERROR(...)                                                                                                                                                                                                                                       \
	MOCHA_LOG(__VA_ARGS__);                                                                                                                                                                                                                                    \
	MOCHA_BREAK;
#define MOCHA_ASSERT(expression, ...)                                                                                                                                                                                                                          \
	if (!(expression)) {                                                                                                                                                                                                                                       \
		MOCHA_LOG(__VA_ARGS__);                                                                                                                                                                                                                                \
		MOCHA_BREAK;                                                                                                                                                                                                                                           \
	}

#define MOCHA_OUTPUT(...)                                                                                                                                                                                                                                      \
	{                                                                                                                                                                                                                                                          \
		char _temp_str[1024];                                                                                                                                                                                                                                  \
		int _characters_written = sprintf(_temp_str, __VA_ARGS__);                                                                                                                                                                                             \
		if (g_mocha_log.log) {                                                                                                                                                                                                                                 \
			g_mocha_log.log(_temp_str);                                                                                                                                                                                                                        \
		}                                                                                                                                                                                                                                                      \
		fwrite(_temp_str, 1, _characters_written, stdout);                                                                                                                                                                                                     \
		fflush(stdout);                                                                                                                                                                                                                                        \
	}

#define MOCHA_LOG(...)                                                                                                                                                                                                                                         \
	{                                                                                                                                                                                                                                                          \
		char _temp_str[10*1024];                                                                                                                                                                                                                                  \
		sprintf(_temp_str, __VA_ARGS__);                                                                                                                                                                                                                       \
		if (g_mocha_log.log) {                                                                                                                                                                                                                                 \
			g_mocha_log.log(_temp_str);                                                                                                                                                                                                                        \
		}                                                                                                                                                                                                                                                      \
		puts(_temp_str);                                                                                                                                                                                                                                       \
		fflush(stdout);                                                                                                                                                                                                                                        \
	}

#define MOCHA_LOG_INFO(...)                                                                                                                                                                                                                                    \
	{                                                                                                                                                                                                                                                          \
		char _temp_str[1024];                                                                                                                                                                                                                                  \
		sprintf(_temp_str, __VA_ARGS__);                                                                                                                                                                                                                       \
		if (g_mocha_log.log) {                                                                                                                                                                                                                                 \
			g_mocha_log.log_info(_temp_str);                                                                                                                                                                                                                   \
		}                                                                                                                                                                                                                                                      \
		puts(_temp_str);                                                                                                                                                                                                                                       \
		fflush(stdout);                                                                                                                                                                                                                                        \
	}
#endif
