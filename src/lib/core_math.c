#include <mocha/core_math.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/random.h>
#include <mocha/runtime.h>
#include <mocha/values.h>

#include <tyran/tyran_clib.h>
#include <math.h>
/*
static int g_sin_table[360];
static int sin_multiplier = 100;
static const float MOCHA_TWO_PI = 6.2831f;
static const int MOCHA_MAX_DEGREES = 359;

static void init_sin_table(int sine[])
{
	for (int i = 0; i < 360; ++i) {
		double angle = (MOCHA_TWO_PI * (double) i) / (double) MOCHA_MAX_DEGREES;
		float sin_value = sinf(angle);
		sine[i] = (int) ((sin_value * (float) sin_multiplier) + 0.5f);
	}
}

static const mocha_object* sin_helper(const mocha_context* context, const mocha_list* arguments, int offset)
{
	const mocha_object* angle_object = arguments->objects[1];
	int factor = 100;
	if (arguments->count > 2) {
		const mocha_object* factor_object = arguments->objects[2];
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
		return;
	}
	const mocha_object* basis_object = arguments->objects[1];
	const mocha_object* factor_object = arguments->objects[2];

	int basis = mocha_object_integer(basis_object, "basis");
	int modulus = mocha_object_integer(factor_object, "factor") + 1;

	int result_value = (int) mocha_pseudo_random(basis, modulus);
	const mocha_object* result = mocha_values_create_integer(context->values, result_value);
	return  result);
}

static int fast_atan2(signed int x, signed int y) // these hold the XY vector at the start
{
	unsigned char negflag;
	unsigned char tempdegree;
	unsigned char comp;
	unsigned int degree; // this will hold the result
						 // (and they will be destroyed)
	unsigned int ux;
	unsigned int uy;

	// Save the sign flags then remove signs and get XY as unsigned ints
	negflag = 0;
	if (x < 0) {
		negflag += 0x01; // x flag bit
		x = (0 - x);	 // is now +
	}
	ux = x; // copy to unsigned var before multiply
	if (y < 0) {
		negflag += 0x02; // y flag bit
		y = (0 - y);	 // is now +
	}
	uy = y; // copy to unsigned var before multiply

	// 1. Calc the scaled "degrees"
	if (ux > uy) {
		degree = (uy * 45) / ux; // degree result will be 0-45 range
		negflag += 0x10;		 // octant flag bit
	} else {
		degree = (ux * 45) / uy; // degree result will be 0-45 range
	}

	// 2. Compensate for the 4 degree error curve
	comp = 0;
	tempdegree = degree; // use an unsigned char for speed!
	if (tempdegree > 22) // if top half of range
	{
		if (tempdegree <= 44)
			comp++;
		if (tempdegree <= 41)
			comp++;
		if (tempdegree <= 37)
			comp++;
		if (tempdegree <= 32)
			comp++; // max is 4 degrees compensated
	} else			// else is lower half of range
	{
		if (tempdegree >= 2)
			comp++;
		if (tempdegree >= 6)
			comp++;
		if (tempdegree >= 10)
			comp++;
		if (tempdegree >= 15)
			comp++; // max is 4 degrees compensated
	}
	degree += comp; // degree is now accurate to +/- 1 degree!

	// Invert degree if it was X>Y octant, makes 0-45 into 90-45
	if (negflag & 0x10)
		degree = (90 - degree);

	// 3. Degree is now 0-90 range for this quadrant,
	// need to invert it for whichever quadrant it was in
	if (negflag & 0x02) // if -Y
	{
		if (negflag & 0x01) // if -Y -X
			degree = (180 + degree);
		else // else is -Y +X
			degree = (180 - degree);
	} else // else is +Y
	{
		if (negflag & 0x01) // if +Y -X
			degree = (360 - degree);
	}

	return degree;
}

MOCHA_FUNCTION(atan2_func)
{
	if (arguments->count < 3) {
		MOCHA_LOG("atan2() takes two parameters!!");
		return;
	}
	const mocha_object* y_object = arguments->objects[1];
	const mocha_object* x_object = arguments->objects[2];

	int y_int = mocha_object_integer(y_object, "Y-obj");
	int x_int = mocha_object_integer(x_object, "x-obj");

	// int degrees = slow_atan2(y_int, x_int);
	int fast_degrees = fast_atan2(y_int, x_int);

	// MOCHA_LOG("y:%d, x:%d atan2:%d fast_atan:%d diff:%d", y_int, x_int, degrees, fast_degrees, (degrees - fast_degrees));

	const mocha_object* result = mocha_values_create_integer(context->values, fast_degrees);
	return  result);
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
		return;
	}
	const mocha_object* value_object = arguments->objects[1];
	const mocha_object* divider_object = arguments->objects[2];

	int value = mocha_object_integer(value_object, "value");
	int divider = mocha_object_integer(divider_object, "divider");

	if (divider == 0) {
		MOCHA_LOG("Error rem can not handle 0");
		return;
	}

	int remainder = mod(value, divider);
	const mocha_object* result = mocha_values_create_integer(context->values, remainder);
	return  result);
}

MOCHA_FUNCTION(min_func)
{
	if (arguments->count < 2) {
		MOCHA_LOG("min() needs at least one parameter");
		return;
	}

	const mocha_object* minimum_value_object = 0;
	int minimum_value = 0;
	for (size_t i = 1; i < arguments->count; ++i) {
		const mocha_object* value_object = arguments->objects[i];
		int value = mocha_object_integer(value_object, "min_value");
		if (!minimum_value_object || value < minimum_value) {
			minimum_value_object = value_object;
			minimum_value = value;
		}
	}
	return  minimum_value_object);
}

MOCHA_FUNCTION(max_func)
{
	if (arguments->count < 2) {
		MOCHA_LOG("max() needs at least one parameter");
		return;
	}

	const mocha_object* maximum_value_object = 0;
	int maximum_value = 0;
	for (size_t i = 1; i < arguments->count; ++i) {
		const mocha_object* value_object = arguments->objects[i];
		int value = mocha_object_integer(value_object, "max_value");
		if (!maximum_value_object || value > maximum_value) {
			maximum_value_object = value_object;
			maximum_value = value;
		}
	}
	return  maximum_value_object);
}

MOCHA_FUNCTION(mid_func)
{
	if (arguments->count != 3) {
		MOCHA_LOG("mid() needs two parameters");
		return;
	}

	const mocha_object* a_object = arguments->objects[1];
	const mocha_object* b_object = arguments->objects[2];
	int mid = (mocha_object_integer(b_object, "b") + mocha_object_integer(a_object, "a")) / 2;
	const mocha_object* result = mocha_values_create_integer(context->values, mid);
	return  result);
}

static int mocha_clamp(int v, int min, int max)
{
	return (v < min ? min : (v > max ? max : v));
}

MOCHA_FUNCTION(clamp_func)
{
	if (arguments->count != 4) {
		MOCHA_LOG("clamp() needs three parameters");
		return;
	}

	const mocha_object* v_object = arguments->objects[1];
	const mocha_object* min_object = arguments->objects[2];
	const mocha_object* max_object = arguments->objects[3];

	int v = mocha_object_integer(v_object, "clamp_v");
	int min = mocha_object_integer(min_object, "clamp_min");
	int max = mocha_object_integer(max_object, "clamp_max");

	int clamped = mocha_clamp(v, min, max);
	const mocha_object* result = mocha_values_create_integer(context->values, clamped);
	return  result);
}

MOCHA_FUNCTION(lerp_func)
{
	if (arguments->count != 4) {
		MOCHA_LOG("lerp() needs three parameters");
		return;
	}

	const mocha_object* t_object = arguments->objects[1];
	const mocha_object* start_object = arguments->objects[2];
	const mocha_object* end_object = arguments->objects[3];
	int t = mocha_object_integer(t_object, "t");
	int start = mocha_object_integer(start_object, "start");
	int end = mocha_object_integer(end_object, "end");

	int lerped = start + (t * (end - start) / 100);

	const mocha_object* result = mocha_values_create_integer(context->values, lerped);
	return  result);
}

MOCHA_FUNCTION(metronome_func)
{
	if (arguments->count < 4) {
		MOCHA_LOG("metronome() needs at least three parameters");
		return;
	}

	const mocha_object* t_object = arguments->objects[1];
	int t = mocha_object_integer(t_object, "t");
	const mocha_object* cycle_count_object = arguments->objects[2];
	int cycle_count = mocha_object_integer(cycle_count_object, "cycle_count");
	const mocha_object* enabled_count_object = arguments->objects[3];
	int enabled_count = mocha_object_integer(enabled_count_object, "enabled");

	int offset = 0;
	if (arguments->count > 4) {
		const mocha_object* offset_object = arguments->objects[4];
		offset = mocha_object_integer(offset_object, "offset");
	}

	int cycle_frame_number = (offset + t) % cycle_count;
	mocha_boolean enabled = cycle_frame_number < enabled_count;
	const mocha_object* result = mocha_values_create_boolean(context->values, enabled);
	return  result);
}

MOCHA_FUNCTION(drunk_func)
{
	if (arguments->count != 4) {
		MOCHA_LOG("drunk() needs three parameters");
		return;
	}

	const mocha_object* time_object = arguments->objects[1];
	int t = mocha_object_integer(time_object, "t");

	const mocha_object* value_object = arguments->objects[2];
	int value = mocha_object_integer(value_object, "v");

	const mocha_object* random_max_delta_object = arguments->objects[3];
	int random_max_delta = mocha_object_integer(random_max_delta_object, "random_max_delta");
	int span = (random_max_delta * 2) + 1;
	int delta = ((int) mocha_pseudo_random(t, span)) - random_max_delta;
	int result_value = value + delta;
	const mocha_object* result = mocha_values_create_integer(context->values, result_value);
	return  result);
}

void mocha_core_math_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(sin);
	MOCHA_DEF_FUNCTION(cos);
	MOCHA_DEF_FUNCTION(rnd);
	MOCHA_DEF_FUNCTION(atan2);
	MOCHA_DEF_FUNCTION(mod);
	MOCHA_DEF_FUNCTION(min);
	MOCHA_DEF_FUNCTION(max);
	MOCHA_DEF_FUNCTION(mid);
	MOCHA_DEF_FUNCTION(clamp);
	MOCHA_DEF_FUNCTION(lerp);
	MOCHA_DEF_FUNCTION(metronome);
	MOCHA_DEF_FUNCTION(drunk);
	init_sin_table(g_sin_table);
}
*/
