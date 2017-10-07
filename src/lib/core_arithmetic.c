#include <mocha/core_arithmetic.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <mocha/object.h>
#include <mocha/reducer.h>
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
	const mocha_object *argument = arguments->objects[1];
	int v = mocha_object_integer(argument, "dec") - 1;
	const mocha_object *r = mocha_values_create_integer(context->values, v);
	return r;
}

MOCHA_FUNCTION(inc_func)
{
	const mocha_object *argument = arguments->objects[1];
	int v = mocha_object_integer(argument, "inc") + 1;
	const mocha_object *r = mocha_values_create_integer(context->values, v);
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


// void *user_data, const mocha_object *object_to_resolve, const char *debug_name)

/*
typedef struct transducer_step_info
{
	const mocha_list *source_arguments;
	const mocha_object *last_result;
	const mocha_object *step_data;
	const mocha_object *work_fn;
	void *work_user_data;
	mocha_transducer_adder adder;
} transducer_step_info;

typedef struct transducer_adder_info
{
	mocha_values *values;
	mocha_list *target;
	size_t index;
} transducer_adder_info;

void transducer_adder_sequence(void *user_data, const struct mocha_object *result_item)
{
	transducer_adder_info *self = (transducer_adder_info *)user_data;
	self->target.objects[self->index++] = result_item;
}

const struct mocha_object *transducer_adder_sequence_done(void *user_data)
{
	transducer_adder_info *self = (transducer_adder_info *)user_data;
	const mocha_object *list_object = mocha_values_create_list(self->values, self->target.objects, self->target.count);

	return list_object;
}

typedef const struct mocha_object *transducer_fn(void *_self, const struct mocha_context *context, const struct mocha_object *item)
{
	transducer_step_info *self = (transducer_step_info *)_self;

	//a_context *context, const mocha_object *fn, void* user_data, mocha_transducer_adder adder, const mocha_object *state, const mocha_object *item)
	self->last_result = mocha_transduce(context, self->work_fn, self->work_user_data, self->adder, self->last_result, item);
	return self->step_data;
}

static const mocha_object *create_transducer_step(mocha_values *values, const mocha_list *arguments, mocha_c_fn init, mocha_c_fn work, const char *debug)
{
	if (arguments->count == 1)
	{
		return init(0, arguments);
	}
	transducer_step_info *state = tyran_malloc_type(transducer_step_info);
	const mocha_object *step_data = mocha_values_create_execute_step_data(context->values, transducer_fn, state, init, work, debug);
	state->step_data = step_data;
	return step_data;
}
*/

void mocha_core_arithmetic_define_context(mocha_context *context, mocha_values *values)
{
	MOCHA_DEF_FUNCTION_EX(add, "+", mocha_true);
	MOCHA_DEF_FUNCTION_EX(sub, "-", mocha_true);
	MOCHA_DEF_FUNCTION_EX(mul, "*", mocha_true);
	MOCHA_DEF_FUNCTION_EX(div, "/", mocha_true);
	MOCHA_DEF_FUNCTION_EX(dec, "dec", mocha_true);
	MOCHA_DEF_FUNCTION_EX(inc, "inc", mocha_true);
}
