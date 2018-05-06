#ifndef mocha_log_h
#define mocha_log_h

#include <stdio.h>
#include <stdlib.h>

#include <clog/clog.h>

#if !defined TORNADO_OS_WEBASSEMBLY
#define MOCHA_BREAK abort()
#else
#define MOCHA_BREAK

#endif

#define MOCHA_ERROR(...) CLOG_ERROR(__VA_ARGS__)

#define MOCHA_SOFT_ERROR(...) CLOG_SOFT_ERROR(__VA_ARGS__);

#define MOCHA_ASSERT(expression, ...)                                                                                                                                                                                                                          \
	if (!(expression)) {                                                                                                                                                                                                                                       \
		MOCHA_ERROR(__VA_ARGS__);                                                                                                                                                                                                                              \
		MOCHA_BREAK;                                                                                                                                                                                                                                           \
	}

#define MOCHA_OUTPUT(...) CLOG_OUTPUT(__VA_ARGS__);

#define MOCHA_LOG(...) CLOG_INFO(__VA_ARGS__);
#define MOCHA_LOG_INFO(...) CLOG_INFO(__VA_ARGS__);
#define MOCHA_LOG_WARN(...) CLOG_WARN(__VA_ARGS__);

#endif
