#include <mocha/core_arithmetic.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <mocha/core_bit.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

MOCHA_FUNCTION(bit_shift_right_func)
{
    if (arguments->count <= 2) {
        MOCHA_LOG("bit-shift needs two parameters");
        return 0;
    }
	int v = mocha_object_integer(arguments->objects[1], "bit-shift-right-value");
	int count = mocha_object_integer(arguments->objects[2], "bit-shift-right-count");
    if (count < 0 || count > 31) {
        MOCHA_LOG("Illegal shift count:%d", count);
        return 0;
    }
    v >>= count;
	const mocha_object* r = mocha_values_create_integer(context->values, v);
	return r;
}

MOCHA_FUNCTION(bit_shift_left_func)
{
    if (arguments->count <= 2) {
        MOCHA_LOG("bit-shift needs two parameters");
        return 0;
    }
	int v = mocha_object_integer(arguments->objects[1], "bit-shift-left-value");
	int count = mocha_object_integer(arguments->objects[2], "bit-shift-left-count");
    if (count < 0 || count > 31) {
        MOCHA_LOG("Illegal shift count:%d", count);
        return 0;
    }
    v <<= count;
	const mocha_object* r = mocha_values_create_integer(context->values, v);
	return r;
}


MOCHA_FUNCTION(bit_or_func)
{
    if (arguments->count <= 2) {
        MOCHA_LOG("bit-or needs at least two parameters");
        return 0;
    }

	int v = mocha_object_integer(arguments->objects[1], "bit-or1");

	for (size_t c = 2; c < arguments->count; ++c) {
		v |= mocha_object_integer(arguments->objects[c], "bit-or");
	}

	const mocha_object* r = mocha_values_create_integer(context->values, v);
	return r;
}

MOCHA_FUNCTION(bit_and_func)
{
    if (arguments->count <= 2) {
        MOCHA_LOG("bit-and needs at least two parameters");
        return 0;
    }

	int v = mocha_object_integer(arguments->objects[1], "bit-and1");

	for (size_t c = 2; c < arguments->count; ++c) {
		v &= mocha_object_integer(arguments->objects[c], "bit-and");
	}

	const mocha_object* r = mocha_values_create_integer(context->values, v);
	return r;
}

void mocha_core_bit_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION_EX(bit_and, "bit-and", mocha_true);
	MOCHA_DEF_FUNCTION_EX(bit_or, "bit-or", mocha_true);
	MOCHA_DEF_FUNCTION_EX(bit_shift_left, "bit-shift-left", mocha_true);
	MOCHA_DEF_FUNCTION_EX(bit_shift_right, "bit-shift-right", mocha_true);
}
