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
#include <mocha/core_math.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/random.h>
#include <mocha/reducer_internal.h>
#include <mocha/runtime.h>
#include <mocha/values.h>

#include "fast_atan2.h"
#include <math.h>
#include <tiny-libc/tiny_libc.h>

static int g_sin_table[360];
static int sin_multiplier = 100;
static const float MOCHA_TWO_PI = 6.2831f;
static const int MOCHA_MAX_DEGREES = 360;

static void init_sin_table(int sine[])
{
	for (int i = 0; i < 360; ++i) {
		double angle = (MOCHA_TWO_PI * (double) i) / (double) MOCHA_MAX_DEGREES;
		float sin_value = sinf(angle);
		sine[i] = (int) ((sin_value * (float) sin_multiplier) + 0.1f);
	}
}

static const mocha_object* sin_helper(const mocha_context* context, const mocha_list* arguments, int offset)
{
	const mocha_object* angle_object = mocha_runner_eval(context, arguments->objects[1]);
	int factor = 100;
	if (arguments->count > 2) {
		const mocha_object* factor_object = mocha_runner_eval(context, arguments->objects[2]);
		factor = mocha_object_integer(factor_object, "factor");
	}
	int int_angle = mocha_object_integer(angle_object, "angle");
	int_angle += offset;
	int_angle = int_angle % 360;

	int sin_value = g_sin_table[int_angle];
	int result = sin_value * factor / sin_multiplier;

	const mocha_object* r = mocha_values_create_integer(context->values, result);
	return r;
}

MOCHA_FUNCTION(sin_func)
{
	const mocha_object* r = sin_helper(context, arguments, 0);
	return r;
}

MOCHA_FUNCTION(cos_func)
{
	const mocha_object* r = sin_helper(context, arguments, 90);
	return r;
}

MOCHA_FUNCTION(rnd_func)
{
	if (arguments->count < 3) {
		MOCHA_LOG("rnd() takes two parameters!!");
		return 0;
	}
	const mocha_object* basis_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* factor_object = mocha_runner_eval(context, arguments->objects[2]);

	int basis = mocha_object_integer(basis_object, "basis");
	int modulus = mocha_object_integer(factor_object, "factor") + 1;

	int result_value = (int) mocha_pseudo_random(basis, modulus);
	const mocha_object* result = mocha_values_create_integer(context->values, result_value);
	return result;
}

MOCHA_FUNCTION(atan2_func)
{
	if (arguments->count < 3) {
		MOCHA_LOG("atan2() takes two parameters!!");
		return 0;
	}
	const mocha_object* y_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* x_object = mocha_runner_eval(context, arguments->objects[2]);

	int y_int = mocha_object_integer(y_object, "Y-obj");
	int x_int = mocha_object_integer(x_object, "x-obj");

	// int degrees = slow_atan2(y_int, x_int);
	int fast_degrees = fast_atan2(y_int, x_int);

	const mocha_object* result = mocha_values_create_integer(context->values, fast_degrees);
	return result;
}

MOCHA_FUNCTION(mid_func)
{
	if (arguments->count != 3) {
		MOCHA_LOG("mid() needs two parameters");
		return 0;
	}

	const mocha_object* a_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* b_object = mocha_runner_eval(context, arguments->objects[2]);
	int mid = (mocha_object_integer(b_object, "b") + mocha_object_integer(a_object, "a")) / 2;
	const mocha_object* result = mocha_values_create_integer(context->values, mid);
	return result;
}

static int mocha_clamp(int v, int min, int max)
{
	return (v < min ? min : (v > max ? max : v));
}

MOCHA_FUNCTION(clamp_func)
{
	if (arguments->count != 4) {
		MOCHA_LOG("clamp() needs three parameters");
		return 0;
	}

	const mocha_object* v_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* min_object = mocha_runner_eval(context, arguments->objects[2]);
	const mocha_object* max_object = mocha_runner_eval(context, arguments->objects[3]);

	int v = mocha_object_integer(v_object, "clamp_v");
	int min = mocha_object_integer(min_object, "clamp_min");
	int max = mocha_object_integer(max_object, "clamp_max");

	int clamped = mocha_clamp(v, min, max);
	const mocha_object* result = mocha_values_create_integer(context->values, clamped);
	return result;
}

MOCHA_FUNCTION(lerp_func)
{
	if (arguments->count != 4) {
		MOCHA_LOG("lerp() needs three parameters");
		return 0;
	}

	const mocha_object* t_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* start_object = mocha_runner_eval(context, arguments->objects[2]);
	const mocha_object* end_object = mocha_runner_eval(context, arguments->objects[3]);
	int t = mocha_object_integer(t_object, "t");
	int start = mocha_object_integer(start_object, "start");
	int end = mocha_object_integer(end_object, "end");

	int lerped = start + (t * (end - start) / 100);

	const mocha_object* result = mocha_values_create_integer(context->values, lerped);
	return result;
}

MOCHA_FUNCTION(metronome_func)
{
	if (arguments->count < 4) {
		MOCHA_LOG("metronome() needs at least three parameters");
		return 0;
	}

	const mocha_object* t_object = mocha_runner_eval(context, arguments->objects[1]);
	int t = mocha_object_integer(t_object, "t");
	const mocha_object* cycle_count_object = mocha_runner_eval(context, arguments->objects[2]);
	int cycle_count = mocha_object_integer(cycle_count_object, "cycle_count");
	const mocha_object* enabled_count_object = mocha_runner_eval(context, arguments->objects[3]);
	int enabled_count = mocha_object_integer(enabled_count_object, "enabled");

	int offset = 0;
	if (arguments->count > 4) {
		const mocha_object* offset_object = mocha_runner_eval(context, arguments->objects[4]);
		offset = mocha_object_integer(offset_object, "offset");
	}

	int cycle_frame_number = (offset + t) % cycle_count;
	mocha_boolean enabled = cycle_frame_number < enabled_count;
	const mocha_object* result = mocha_values_create_boolean(context->values, enabled);
	return result;
}

MOCHA_FUNCTION(drunk_func)
{
	if (arguments->count != 4) {
		MOCHA_LOG("drunk() needs three parameters");
		return 0;
	}

	const mocha_object* time_object = mocha_runner_eval(context, arguments->objects[1]);
	int t = mocha_object_integer(time_object, "t");

	const mocha_object* value_object = mocha_runner_eval(context, arguments->objects[2]);
	int value = mocha_object_integer(value_object, "v");

	const mocha_object* random_max_delta_object = mocha_runner_eval(context, arguments->objects[3]);
	int random_max_delta = mocha_object_integer(random_max_delta_object, "random_max_delta");
	int span = (random_max_delta * 2) + 1;
	int delta = ((int) mocha_pseudo_random(t, span)) - random_max_delta;
	int result_value = value + delta;
	const mocha_object* result = mocha_values_create_integer(context->values, result_value);
	return result;
}

static int mod(int v, int div)
{
	if (div < 0) {
		div = -div;
	}
	int ret = v % div;
	if (ret < 0) {
		ret += div;
	}
	return ret;
}

MOCHA_FUNCTION(mod_func)
{
	if (arguments->count < 3) {
		MOCHA_LOG("mod() takes two parameters!!");
		return 0;
	}
	const mocha_object* value_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* divider_object = mocha_runner_eval(context, arguments->objects[2]);

	int value = mocha_object_integer(value_object, "value");
	int divider = mocha_object_integer(divider_object, "divider");

	if (divider == 0) {
		MOCHA_SOFT_ERROR("Error rem can not handle 0");
		return 0;
	}

	int remainder = mod(value, divider);
	const mocha_object* result = mocha_values_create_integer(context->values, remainder);
	return result;
}

const mocha_object* just_return(struct mocha_values* values, const struct mocha_object* a)
{
	return a;
}

const mocha_object* do_max(struct mocha_values* values, const struct mocha_object* a, const struct mocha_object* b)
{
	if (mocha_object_less(a, b)) {
		return b;
	} else {
		return a;
	}
}

MOCHA_FUNCTION(max_func)
{
	return mocha_reducer_reduce_internal_single(context, arguments, just_return, do_max, "max");
}

const mocha_object* do_min(struct mocha_values* values, const struct mocha_object* a, const struct mocha_object* b)
{
	if (mocha_object_less(a, b)) {
		return a;
	} else {
		return b;
	}
}

MOCHA_FUNCTION(min_func)
{
	return mocha_reducer_reduce_internal_single(context, arguments, just_return, do_min, "min");
}

void mocha_core_math_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(max);
	MOCHA_DEF_FUNCTION(min);
	MOCHA_DEF_FUNCTION(mod);
	MOCHA_DEF_FUNCTION(sin);
	MOCHA_DEF_FUNCTION(cos);
	MOCHA_DEF_FUNCTION(rnd);
	MOCHA_DEF_FUNCTION(atan2);
	MOCHA_DEF_FUNCTION(mid);
	MOCHA_DEF_FUNCTION(clamp);
	MOCHA_DEF_FUNCTION(lerp);
	MOCHA_DEF_FUNCTION(metronome);
	MOCHA_DEF_FUNCTION(drunk);
	init_sin_table(g_sin_table);
}
