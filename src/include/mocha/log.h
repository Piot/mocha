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

#define MOCHA_ERROR(...) TYRAN_ERROR(__VA_ARGS__)

#define MOCHA_SOFT_ERROR(...) TYRAN_SOFT_ERROR(__VA_ARGS__);

#define MOCHA_ASSERT(expression, ...)                                                                                                                                                                                                                          \
	if (!(expression)) {                                                                                                                                                                                                                                       \
		MOCHA_ERROR(__VA_ARGS__);                                                                                                                                                                                                                              \
		MOCHA_BREAK;                                                                                                                                                                                                                                           \
	}

#define MOCHA_OUTPUT(...) TYRAN_OUTPUT(__VA_ARGS__);

#define MOCHA_LOG(...) TYRAN_LOG_INFO(__VA_ARGS__);
#define MOCHA_LOG_INFO(...) TYRAN_LOG_INFO(__VA_ARGS__);
#define MOCHA_LOG_WARN(...) TYRAN_LOG_WARN(__VA_ARGS__);

#endif
