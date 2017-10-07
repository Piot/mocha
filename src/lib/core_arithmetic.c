#include <mocha/core_arithmetic.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <mocha/object.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

MOCHA_FUNCTION(do_int_init_func)
{
	return mocha_values_create_integer(context->values, 0);
}

MOCHA_FUNCTION(do_mul_init_func)
{
	return mocha_values_create_integer(context->values, 1);
}

typedef struct reducer_step_info
{
	const mocha_list *source_arguments;
	const mocha_object *last_result;
	const mocha_object *work_fn;
} reducer_step_info;

typedef struct reducer_step_internal_info
{
	const mocha_list source_arguments;
	size_t index;
	const mocha_object *last_result;
	mocha_c_fn c_fn;
	mocha_values *values;
	mocha_context *context;
} reducer_step_internal_info;

static const mocha_object *call_reduce_internal(const struct mocha_context *context, mocha_c_fn c_fn, const mocha_object *last_result, const mocha_object *item)
{
	const mocha_object *args[3];
	args[0] = 0;
	args[1] = last_result;
	args[2] = item;

	mocha_list arguments;
	arguments.objects = args;
	arguments.count = 3;

	const mocha_object *next_result = c_fn(context, &arguments);

	return next_result;
}

static const struct mocha_object *reducer_internal_fn(void *_self, const struct mocha_context *context, const struct mocha_object *item)
{
	reducer_step_internal_info *self = (reducer_step_internal_info *)_self;
	const mocha_object *new_result = call_reduce_internal(context, self->c_fn, self->last_result, item);
	self->last_result = new_result;

	if (self->index >= self->source_arguments.count)
	{
		return self->last_result;
	}
	else
	{
		const mocha_object *next_object_to_resolve = self->source_arguments.objects[self->index];
		const mocha_object *reducer_step_data_object = mocha_values_create_execute_step_data(self->values, reducer_internal_fn, self, next_object_to_resolve, "reducer_internal_func");
		self->index++;
		return reducer_step_data_object;
	}
}

static const mocha_object *create_reducer_step_internal(mocha_context *context, const mocha_list *arguments, mocha_c_fn init, mocha_c_fn work, const char *debug)
{
	const mocha_object *first = init(context, arguments);
	if (arguments->count == 1)
	{
		return first;
	}

	int first_index_to_resolve = 0;
	const mocha_object *initial_object;
	if (arguments->count == 2)
	{
		first_index_to_resolve = 0;
		initial_object = first;
	}
	else
	{
		initial_object = arguments->objects[1];
		first_index_to_resolve = 1;
	}
	reducer_step_internal_info *state = tyran_malloc_type(reducer_step_internal_info);
	state->index = first_index_to_resolve + 1;
	state->c_fn = work;
	state->values = context->values;
	state->context = context;
	state->last_result = initial_object;
	mocha_list_init(&state->source_arguments, &context->values->object_references, &arguments->objects[1], arguments->count - 1);
	const mocha_object *first_object_to_resolve = state->source_arguments.objects[first_index_to_resolve];
	const mocha_object *step_data = mocha_values_create_execute_step_data(context->values, reducer_internal_fn, state, first_object_to_resolve, debug);
	return step_data;
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

MOCHA_FUNCTION(mul_func)
{
	return create_reducer_step_internal(context, arguments, do_mul_init_func, do_mul_func, "*");
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

MOCHA_FUNCTION(do_sub_func)
{
	int a = mocha_object_integer(arguments->objects[1], "sub1");
	int b = mocha_object_integer(arguments->objects[2], "sub2");

	return mocha_values_create_integer(context->values, a - b);
}

MOCHA_FUNCTION(sub_func)
{
	return create_reducer_step_internal(context, arguments, do_int_init_func, do_sub_func, "-");
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
	return create_reducer_step_internal(context, arguments, do_mul_init_func, do_div_func, "/");
}

MOCHA_FUNCTION(do_add_func)
{
	int a = mocha_object_integer(arguments->objects[1], "add1");
	int b = mocha_object_integer(arguments->objects[2], "add2");

	return mocha_values_create_integer(context->values, a + b);
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

MOCHA_FUNCTION(add_func)
{
	return create_reducer_step_internal(context, arguments, do_int_init_func, do_add_func, "+");
}

void mocha_core_arithmetic_define_context(mocha_context *context, mocha_values *values)
{
	MOCHA_DEF_FUNCTION_EX(add, "+", mocha_true);
	MOCHA_DEF_FUNCTION_EX(sub, "-", mocha_true);
	MOCHA_DEF_FUNCTION_EX(mul, "*", mocha_true);
	MOCHA_DEF_FUNCTION_EX(div, "/", mocha_true);
	MOCHA_DEF_FUNCTION_EX(dec, "dec", mocha_true);
	MOCHA_DEF_FUNCTION_EX(inc, "inc", mocha_true);
}
