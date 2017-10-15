#include <mocha/core_compare.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/reducer_internal.h>

#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const mocha_object* do_less_or_equal(mocha_values* values, const mocha_object* a, const mocha_object* b, mocha_boolean* should_continue)
{
	mocha_boolean is_less_or_equal = mocha_object_less(a, b) || mocha_object_equal(a, b);
	*should_continue = is_less_or_equal;
	return mocha_values_create_boolean(values, is_less_or_equal);
}

MOCHA_FUNCTION(less_or_equal_func)
{
	return mocha_reducer_reduce_internal_check(context, arguments, do_less_or_equal, "less-or-equal");
}

const mocha_object* do_less(mocha_values* values, const mocha_object* a, const mocha_object* b, mocha_boolean* should_continue)
{
	mocha_boolean is_less = mocha_object_less(a, b);
	*should_continue = is_less;
	return mocha_values_create_boolean(values, is_less);
}

MOCHA_FUNCTION(less_func)
{
	return mocha_reducer_reduce_internal_check(context, arguments, do_less, "less");
}

const mocha_object* do_greater(mocha_values* values, const mocha_object* a, const mocha_object* b, mocha_boolean* should_continue)
{
	mocha_boolean is_greater = !mocha_object_less(a, b) && !mocha_object_equal(a, b);
	*should_continue = is_greater;
	return mocha_values_create_boolean(values, is_greater);
}

MOCHA_FUNCTION(greater_func)
{
	return mocha_reducer_reduce_internal_check(context, arguments, do_greater, "greater");
}

const mocha_object* do_greater_or_equal(mocha_values* values, const mocha_object* a, const mocha_object* b, mocha_boolean* should_continue)
{
	mocha_boolean is_greater_or_equal = !mocha_object_less(a, b);
	*should_continue = is_greater_or_equal;
	return mocha_values_create_boolean(values, is_greater_or_equal);
}

MOCHA_FUNCTION(greater_or_equal_func)
{
	return mocha_reducer_reduce_internal_check(context, arguments, do_greater_or_equal, "greater-or-equal");
}

const mocha_object* do_equal(mocha_values* values, const mocha_object* a, const mocha_object* b, mocha_boolean* should_continue)
{
	mocha_boolean is_equal = mocha_object_equal(a, b);
	*should_continue = is_equal;
	return mocha_values_create_boolean(values, is_equal);
}

MOCHA_FUNCTION(equal_func)
{
	return mocha_reducer_reduce_internal_check(context, arguments, do_equal, "equal");
}

MOCHA_FUNCTION(zero_func)
{
	const mocha_object* argument = mocha_runner_eval(context, arguments->objects[1]);

	mocha_boolean b = mocha_object_integer(argument, "zero") == 0;

	const mocha_object* o = mocha_values_create_boolean(context->values, b);
	return o;
}

MOCHA_FUNCTION(nil_func)
{
	const mocha_object* o = mocha_runner_eval(context, arguments->objects[1]);

	const mocha_object* result = mocha_values_create_boolean(context->values, o->type == mocha_object_type_nil);

	return result;
}

void mocha_core_compare_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION_EX(less_or_equal, "<=");
	MOCHA_DEF_FUNCTION_EX(greater_or_equal, ">=");
	MOCHA_DEF_FUNCTION_EX(greater, ">");
	MOCHA_DEF_FUNCTION_EX(less, "<");
	MOCHA_DEF_FUNCTION_EX(equal, "=");
	MOCHA_DEF_FUNCTION_EX(nil, "nil?");
	MOCHA_DEF_FUNCTION_EX(zero, "zero?");
}
