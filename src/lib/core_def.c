#include <mocha/core_def.h>
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
#include <tyran/tyran_clib.h>

static const mocha_object* fn(const mocha_context* context, const mocha_object* name, const mocha_object* arguments, const mocha_object* body)
{
	const mocha_object* r = mocha_values_create_function(context->values, context, name, arguments, body);

	return r;
}

MOCHA_FUNCTION(fn_func)
{
	const mocha_object* default_name = mocha_values_create_symbol(context->values, "_fn");
	const mocha_object* r = fn(context, default_name, arguments->objects[1], arguments->objects[2]);

	return r;
}

MOCHA_FUNCTION(defn_func)
{
	if (arguments->count < 4) {
		MOCHA_LOG("defn error %s", "");
		return 0;
	}
	const mocha_object* name = arguments->objects[1];
	size_t index = 2;

	if (mocha_object_is_string(arguments->objects[index])) {
		// Skip documentation
		index++;
	}
	const mocha_object* func = fn(context, name, arguments->objects[index], arguments->objects[index + 1]);

	mocha_context_add_or_replace((mocha_context*) context, name, func);
	return func;
}

typedef struct def_func_info {
	const mocha_context* context;
	const mocha_list* arguments;
} def_func_info;

static void def(struct mocha_context* context, const mocha_object* name, const mocha_object* value)
{
	mocha_context_add_or_replace(context, name, value);
}

static const mocha_object* def_next(void* _self, const struct mocha_context* context, const struct mocha_object* value_object)
{
	const def_func_info* self = (const def_func_info*) _self;

	def((mocha_context*) self->context, self->arguments->objects[1], value_object);

	return mocha_values_create_nil(context->values);
}

MOCHA_FUNCTION(def_func)
{
	def_func_info* state = tyran_malloc_type(def_func_info);

	state->context = context;
	state->arguments = arguments;
	const mocha_object* r = mocha_values_create_execute_step_data(context->values, def_next, state, arguments->objects[2], "def_next");
	return r;
}

MOCHA_FUNCTION(let_func)
{
	const mocha_object* assignments = arguments->objects[1];

	if (!assignments || assignments->type != mocha_object_type_vector) {
		MOCHA_LOG("must have vector in let!");
		return 0;
	}

	const mocha_vector* assignment_vector = &assignments->data.vector;

	if ((assignment_vector->count % 2) != 0) {
		MOCHA_LOG("Wrong number of assignments");
		return 0;
	}
	const mocha_object* body = arguments->objects[2];
	mocha_context* new_context = mocha_context_create(context, "core_def");

	for (size_t i = 0; i < assignment_vector->count; i += 2) {
		const mocha_object* symbol = assignment_vector->objects[i];
		const mocha_object* value = assignment_vector->objects[i + 1];
		mocha_context_add(new_context, symbol, value);
	}

	const mocha_object* body_closure = mocha_values_create_closure(context->values, new_context, body);

	return body_closure;
}

void mocha_core_def_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(defn);
	MOCHA_DEF_FUNCTION(fn);
	MOCHA_DEF_FUNCTION(def);
	MOCHA_DEF_FUNCTION(let);
}
