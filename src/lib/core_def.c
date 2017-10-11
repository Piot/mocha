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

	MOCHA_LOG("DEF '%s'", mocha_print_object_debug_str(value_object));
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

static void setup_context(mocha_context* parent_context, const mocha_vector* let_vector, const mocha_vector* value_vector)
{
	mocha_context* new_context = mocha_context_create(parent_context, "let_def");

	for (size_t i = 0; i < value_vector->count; ++i) {
		const mocha_object* symbol = let_vector->objects[i * 2];
		const mocha_object* value = value_vector->objects[i];
		mocha_context_add(new_context, symbol, value);
	}
}

typedef struct let_func_info {
	const mocha_context* context;
	const mocha_vector* let_vector;
	const mocha_object* body;
} let_func_info;

static const mocha_object* let_next(void* _self, const struct mocha_context* context, const struct mocha_object* list_of_values_object)
{
	const let_func_info* self = (const let_func_info*) _self;

	const mocha_vector* value_vector = mocha_object_vector(list_of_values_object);
	if (value_vector->count != self->let_vector->count / 2) {
		MOCHA_ERROR("We have a problem with mismatching vectors");
	}
	const mocha_vector* values_vector = mocha_object_vector(list_of_values_object);
	setup_context((mocha_context*) self->context, self->let_vector, values_vector);
	return mocha_values_create_nil(context->values);
}

MOCHA_FUNCTION(let_func)
{
	const mocha_vector* let_vector = mocha_object_vector(arguments->objects[1]);
	if ((let_vector->count % 2) != 0) {
		MOCHA_LOG("Wrong number of assignments");
		return 0;
	}
	let_func_info* state = tyran_malloc_type(let_func_info);
	state->context = context;
	state->let_vector = let_vector;
	state->body = arguments->objects[2];

	size_t value_count = let_vector->count / 2;
	const mocha_object** temp = tyran_malloc_type_count(const mocha_object*, value_count);
	for (size_t i = 0; i < value_count; ++i) {
		temp[i] = let_vector->objects[i * 2 + 1];
	}

	const mocha_object* values_to_resolve = mocha_values_create_vector(context->values, temp, value_count);
	tyran_free(temp);

	const mocha_object* r = mocha_values_create_execute_step_data(context->values, let_next, state, values_to_resolve, "let_next");
	return r;
}

void mocha_core_def_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(defn);
	MOCHA_DEF_FUNCTION(fn);
	MOCHA_DEF_FUNCTION(def);
	MOCHA_DEF_FUNCTION(let);
}
