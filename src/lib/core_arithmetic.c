#include <mocha/core_arithmetic.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

MOCHA_FUNCTION(mul_func)
{
	int result = 1;

	for (size_t c = 1; c < arguments->count; ++c) {
		result *= mocha_object_integer(arguments->objects[c], "mul");
	}

	const mocha_object* r = mocha_values_create_integer(context->values, result);
	MOCHA_RESULT_VALUE(result_callback, r);
}

MOCHA_FUNCTION(dec_func)
{
	const mocha_object* argument = arguments->objects[1];
	int v = mocha_object_integer(argument, "dec") - 1;
	const mocha_object* r = mocha_values_create_integer(context->values, v);
	MOCHA_RESULT_VALUE(result_callback, r);
}

MOCHA_FUNCTION(inc_func)
{
	const mocha_object* argument = arguments->objects[1];
	int v = mocha_object_integer(argument, "inc") + 1;
	const mocha_object* r = mocha_values_create_integer(context->values, v);
	MOCHA_RESULT_VALUE(result_callback, r);
}

MOCHA_FUNCTION(sub_func)
{
	int v = 0;
	int start_index = 1;

	if (arguments->count > 2) {
		start_index = 2;
		v = mocha_object_integer(arguments->objects[1], "sub1");
	}

	for (size_t c = start_index; c < arguments->count; ++c) {
		v -= mocha_object_integer(arguments->objects[c], "sub2");
	}

	const mocha_object* r = mocha_values_create_integer(context->values, v);

	MOCHA_RESULT_VALUE(result_callback, r);
}

static inline int floor_div(int x, int y)
{
	int q = x / y;
	int r = x % y;
	if ((r != 0) && ((r < 0) != (y < 0)))
		--q;
	return q;
}

MOCHA_FUNCTION(div_func)
{
	int r = 1;
	int start_index = 1;

	if (arguments->count > 1) {
		start_index = 2;
		r = mocha_object_integer(arguments->objects[1], "div1");
	}
	for (size_t c = start_index; c < arguments->count; ++c) {
		r = floor_div(r, mocha_object_integer(arguments->objects[c], "div"));
	}

	const mocha_object* o = mocha_values_create_integer(context->values, r);
	MOCHA_RESULT_VALUE(result_callback, o);
}

MOCHA_FUNCTION(add_func)
{
	int v = 0;

	for (size_t c = 1; c < arguments->count; ++c) {
		v += mocha_object_integer(arguments->objects[c], "add");
	}

	const mocha_object* r = mocha_values_create_integer(context->values, v);
	MOCHA_RESULT_VALUE(result_callback, r);
}

void mocha_core_arithmetic_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION_EX(add, "+", mocha_true);
	MOCHA_DEF_FUNCTION_EX(sub, "-", mocha_true);
	MOCHA_DEF_FUNCTION_EX(mul, "*", mocha_true);
	MOCHA_DEF_FUNCTION_EX(div, "/", mocha_true);
	MOCHA_DEF_FUNCTION_EX(dec, "dec", mocha_true);
	MOCHA_DEF_FUNCTION_EX(inc, "inc", mocha_true);
}
