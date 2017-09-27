#ifndef mocha_function_h
#define mocha_function_h

#include <mocha/types.h>

struct mocha_object;
struct mocha_context;

typedef struct mocha_function
{
	const struct mocha_object* arguments;
	const struct mocha_object* code;
	const struct mocha_context* context;
	mocha_boolean is_special;
} mocha_function;

#endif
