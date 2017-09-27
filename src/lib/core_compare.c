#include <mocha/core_compare.h>
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

MOCHA_FUNCTION(less_or_equal_func)
{
	mocha_boolean result = mocha_true;

	const mocha_object* source = arguments->objects[1];

	for (size_t i = 1; i < arguments->count; ++i) {
		const mocha_object* v = arguments->objects[i];

		if (!(mocha_object_equal(source, v) || mocha_object_less(source, v))) {
			result = mocha_false;
			break;
		}
	}

	const mocha_object* r = mocha_values_create_boolean(context->values, result);
	MOCHA_RESULT_VALUE(result_callback, r);
}

MOCHA_FUNCTION(less_func)
{
	mocha_boolean result = mocha_true;

	const mocha_object* source = arguments->objects[1];

	for (size_t i = 2; i < arguments->count; ++i) {
		const mocha_object* v = arguments->objects[i];

		if (!mocha_object_less(source, v)) {
			result = mocha_false;
			break;
		}
	}

	const mocha_object* r = mocha_values_create_boolean(context->values, result);
	MOCHA_RESULT_VALUE(result_callback, r);
}

MOCHA_FUNCTION(greater_func)
{
	mocha_boolean result = mocha_true;

	const mocha_object* source = arguments->objects[1];

	for (size_t i = 2; i < arguments->count; ++i) {
		const mocha_object* v = arguments->objects[i];

		if (mocha_object_equal(source, v) || mocha_object_less(source, v)) {
			result = mocha_false;
			break;
		}
	}

	const mocha_object* r = mocha_values_create_boolean(context->values, result);
	MOCHA_RESULT_VALUE(result_callback, r);
}

MOCHA_FUNCTION(greater_or_equal_func)
{
	mocha_boolean result = mocha_true;

	const mocha_object* source = arguments->objects[1];

	for (size_t i = 1; i < arguments->count; ++i) {
		const mocha_object* v = arguments->objects[i];

		if (!(mocha_object_equal(source, v) || !mocha_object_less(source, v))) {
			result = mocha_false;
			break;
		}
	}

	const mocha_object* r = mocha_values_create_boolean(context->values, result);
	MOCHA_RESULT_VALUE(result_callback, r);
}

MOCHA_FUNCTION(equal_func)
{
	mocha_boolean result = mocha_true;

	const mocha_object* source = arguments->objects[1];

	for (size_t i = 1; i < arguments->count; ++i) {
		const mocha_object* v = arguments->objects[i];

		if (!mocha_object_equal(source, v)) {
			result = mocha_false;
			break;
		}
	}

	const mocha_object* r = mocha_values_create_boolean(context->values, result);
	MOCHA_RESULT_VALUE(result_callback, r);
}

MOCHA_FUNCTION(zero_func)
{
	const mocha_object* argument = arguments->objects[1];
	mocha_boolean b = mocha_object_integer(argument, "zero") == 0;

	const mocha_object* o = mocha_values_create_boolean(context->values, b);
	MOCHA_RESULT_VALUE(result_callback, o);
}

MOCHA_FUNCTION(nil_func)
{
	const mocha_object* o = arguments->objects[1];

	const mocha_object* result = mocha_values_create_boolean(context->values, o->type == mocha_object_type_nil);

	MOCHA_RESULT_VALUE(result_callback, result);
}

void mocha_core_compare_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION_EX(less_or_equal, "<=", mocha_true);
	MOCHA_DEF_FUNCTION_EX(greater_or_equal, ">=", mocha_true);
	MOCHA_DEF_FUNCTION_EX(greater, ">", mocha_true);
	MOCHA_DEF_FUNCTION_EX(less, "<", mocha_true);
	MOCHA_DEF_FUNCTION_EX(equal, "=", mocha_true);
	MOCHA_DEF_FUNCTION_EX(nil, "nil?", mocha_true);
	MOCHA_DEF_FUNCTION_EX(zero, "zero?", mocha_true);
}
