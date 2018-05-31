/*

MIT License

Copyright (c) 2013 Peter Bjorklund

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
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
