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
#include <mocha/core_arithmetic.h>
#include <mocha/core_bit.h>
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

const mocha_object* do_bit_or(mocha_values* values, const mocha_object* a_object, const mocha_object* b_object)
{
	int v = mocha_object_integer(a_object, "bit-and1") | mocha_object_integer(b_object, "bit-and2");
	const mocha_object* r = mocha_values_create_integer(values, v);
	return r;
}

MOCHA_FUNCTION(bit_or_func)
{
	return mocha_reducer_reduce_internal_no_init(context, arguments, do_bit_or, "bit-or");
}

const mocha_object* do_bit_and(mocha_values* values, const mocha_object* a_object, const mocha_object* b_object)
{
	int v = mocha_object_integer(a_object, "bit-and1");
	v &= mocha_object_integer(b_object, "bit-and2");
	const mocha_object* r = mocha_values_create_integer(values, v);
	return r;
}

MOCHA_FUNCTION(bit_and_func)
{
	return mocha_reducer_reduce_internal_no_init(context, arguments, do_bit_and, "bit-and");
}

void mocha_core_bit_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION_EX(bit_and, "bit-and");
	MOCHA_DEF_FUNCTION_EX(bit_or, "bit-or");
	MOCHA_DEF_FUNCTION_EX(bit_shift_left, "bit-shift-left");
	MOCHA_DEF_FUNCTION_EX(bit_shift_right, "bit-shift-right");
}
