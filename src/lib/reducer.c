#include <mocha/reducer.h>
#include <mocha/list.h>
#include <mocha/object.h>
#include <mocha/values.h>
#include <mocha/context.h>
#include <tyran/tyran_clib.h>

typedef struct reducer_step_info
{
	const mocha_list *source_arguments;
	const mocha_object *last_result;
	const mocha_object *work_fn;
} reducer_step_info;

typedef struct reducer_step_internal_info
{
	mocha_list source_arguments;
	size_t index;
	const mocha_object *last_result;
	mocha_c_fn c_fn;
	mocha_values *values;
	const mocha_context *context;
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

const mocha_object *mocha_reducer_create_step_internal(const mocha_context *context, const mocha_list *arguments, mocha_c_fn init, mocha_c_fn work, const char *debug)
{
	const mocha_object *first = init(context, arguments);
	if (arguments->count == 1)
	{
		return first;
	}

	size_t first_index_to_resolve = 0;
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
