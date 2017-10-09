#include <mocha/core_arithmetic.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/print.h>
#include <mocha/reducer_internal.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

MOCHA_FUNCTION(do_int_init_func)
{
	(void) arguments;
	return mocha_values_create_integer(context->values, 0);
}

MOCHA_FUNCTION(do_mul_init_func)
{
	(void) arguments;
	return mocha_values_create_integer(context->values, 1);
}

MOCHA_FUNCTION(do_add_func)
{
	int a = mocha_object_integer(arguments->objects[1], "add1");
	int b = mocha_object_integer(arguments->objects[2], "add2");

	return mocha_values_create_integer(context->values, a + b);
}

MOCHA_FUNCTION(do_sub_func)
{
	int a = mocha_object_integer(arguments->objects[1], "sub1");
	int b = mocha_object_integer(arguments->objects[2], "sub2");

	return mocha_values_create_integer(context->values, a - b);
}

MOCHA_FUNCTION(do_mul_func)
{
	int a = mocha_object_integer(arguments->objects[1], "mul1");
	int b = mocha_object_integer(arguments->objects[2], "mul2");

	return mocha_values_create_integer(context->values, a * b);
}

MOCHA_FUNCTION(do_div_func)
{
	int a = mocha_object_integer(arguments->objects[1], "div1");
	int b = mocha_object_integer(arguments->objects[2], "div2");

	return mocha_values_create_integer(context->values, a / b);
}

MOCHA_FUNCTION(dec_func)
{
	const mocha_object* argument = arguments->objects[1];
	int v = mocha_object_integer(argument, "dec") - 1;
	const mocha_object* r = mocha_values_create_integer(context->values, v);
	return r;
}

MOCHA_FUNCTION(inc_func)
{
	const mocha_object* argument = arguments->objects[1];
	int v = mocha_object_integer(argument, "inc") + 1;
	const mocha_object* r = mocha_values_create_integer(context->values, v);
	return r;
}

MOCHA_FUNCTION(mul_func)
{
	return mocha_reducer_create_step_internal(context, arguments, do_mul_init_func, do_mul_func, "*");
}

MOCHA_FUNCTION(sub_func)
{
	return mocha_reducer_create_step_internal(context, arguments, do_int_init_func, do_sub_func, "-");
}

MOCHA_FUNCTION(div_func)
{
	return mocha_reducer_create_step_internal(context, arguments, do_mul_init_func, do_div_func, "/");
}

MOCHA_FUNCTION(add_func)
{
	return mocha_reducer_create_step_internal(context, arguments, do_int_init_func, do_add_func, "+");
}

void mocha_core_arithmetic_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION_EX(add, "+");
	MOCHA_DEF_FUNCTION_EX(sub, "-");
	MOCHA_DEF_FUNCTION_EX(mul, "*");
	MOCHA_DEF_FUNCTION_EX(div, "/");
	MOCHA_DEF_FUNCTION_EX(dec, "dec");
	MOCHA_DEF_FUNCTION_EX(inc, "inc");
}
