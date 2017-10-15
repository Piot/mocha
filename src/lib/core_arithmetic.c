#include <mocha/core_arithmetic.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/print.h>
#include <mocha/reducer_internal.h>
#include <mocha/runner.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const mocha_object* do_add_init(mocha_values* values)
{
	return mocha_values_create_integer(values, 0);
}

const mocha_object* do_mul_init(mocha_values* values)
{
	return mocha_values_create_integer(values, 1);
}

const mocha_object* do_add(mocha_values* values, const mocha_object* a_object, const mocha_object* b_object)
{
	int a = mocha_object_integer(a_object, "add1");
	int b = mocha_object_integer(b_object, "add2");

	return mocha_values_create_integer(values, a + b);
}

const mocha_object* do_sub(mocha_values* values, const mocha_object* a_object, const mocha_object* b_object)
{
	int a = mocha_object_integer(a_object, "sub1");
	int b = mocha_object_integer(b_object, "sub2");

	return mocha_values_create_integer(values, a - b);
}

const mocha_object* do_mul(mocha_values* values, const mocha_object* a_object, const mocha_object* b_object)
{
	int a = mocha_object_integer(a_object, "mul1");
	int b = mocha_object_integer(b_object, "mul2");

	return mocha_values_create_integer(values, a * b);
}

const mocha_object* do_div(mocha_values* values, const mocha_object* a_object, const mocha_object* b_object)
{
	int a = mocha_object_integer(a_object, "div1");
	int b = mocha_object_integer(b_object, "div2");

	return mocha_values_create_integer(values, a / b);
}

MOCHA_FUNCTION(dec_func)
{
	const mocha_object* argument = mocha_runner_eval(context, arguments->objects[1]);
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
	return mocha_reducer_reduce_internal(context, arguments, do_mul_init, do_mul, "*");
}

MOCHA_FUNCTION(div_func)
{
	return mocha_reducer_reduce_internal(context, arguments, do_mul_init, do_div, "/");
}

MOCHA_FUNCTION(add_func)
{
	return mocha_reducer_reduce_internal(context, arguments, do_add_init, do_add, "+");
}

MOCHA_FUNCTION(sub_func)
{
	return mocha_reducer_reduce_internal(context, arguments, do_add_init, do_sub, "-");
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
