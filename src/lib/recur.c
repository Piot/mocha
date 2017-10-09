#include <mocha/context.h>
#include <mocha/execute.h>
#include <mocha/list.h>
#include <mocha/object.h>
#include <mocha/recur.h>
#include <mocha/values.h>
#include <tyran/tyran_clib.h>

typedef struct recur_step_info {
	mocha_list source_arguments;
	mocha_list target_arguments;
	size_t index;
	const mocha_function* script_fn;
	mocha_values* values;
} recur_step_info;

static const struct mocha_object* recur_fn(void* _self, const struct mocha_context* context, const struct mocha_object* item)
{
	recur_step_info* self = (recur_step_info*) _self;
	if (self->index >= self->source_arguments.count) {
		const mocha_object* recur_func_object = script_execute(context, self->script_fn, &self->target_arguments);
		return recur_func_object;
	} else {

		self->target_arguments.objects[self->index] = item;
		self->index++;
		const mocha_object* next_object_to_resolve = self->source_arguments.objects[self->index];
		const mocha_object* reducer_step_data_object = mocha_values_create_execute_step_data(self->values, recur_fn, self, next_object_to_resolve, "recur_internal_func");
		return reducer_step_data_object;
	}
}

const mocha_object* mocha_recur_create_step(const mocha_context* context, const mocha_list* arguments, const mocha_function* fn, const char* debug)
{
	recur_step_info* state = tyran_malloc_type(recur_step_info);
	state->index = 0;
	state->script_fn = fn;
	state->values = context->values;
	mocha_list_init(&state->source_arguments, &context->values->object_references, &arguments->objects[1], arguments->count - 1);
	const mocha_object* first_object_to_resolve = state->source_arguments.objects[0];
	const mocha_object* step_data = mocha_values_create_execute_step_data(context->values, recur_fn, state, first_object_to_resolve, debug);
	return step_data;
}
