#include <mocha/extended_core.h>
#include <mocha/map_utils.h>
#include <tyran/tyran_clib.h>
#include <mocha/core.h>
#include <mocha/hashed_strings.h>
#include <mocha/keyword.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*
#include <burst/burst_file_loader.h>

#define MOCHA_DEF_FUNCTION_HELPER(name, eval_arguments)                                                                                                                                                                                                        \
	static mocha_type name##_def;                                                                                                                                                                                                                              \
	name##_def.invoke = name##_func;                                                                                                                                                                                                                           \
	name##_def.eval_all_arguments = eval_arguments;

#define MOCHA_DEF_FUNCTION(name, eval_arguments)                                                                                                                                                                                                               \
	MOCHA_DEF_FUNCTION_HELPER(name, eval_arguments)                                                                                                                                                                                                            \
	mocha_context_add_function(context, values, #name, &name##_def);

typedef struct resource_state {
	mocha_values* values;
	resolve_callback callback_info;
	const char* debug_name;
	burst_file_loader loader;
	const mocha_object* symbol_object;
} resource_state;

static const mocha_object* get_source_map(const mocha_context* context, mocha_values* values)
{
	const mocha_object* temp_symbol_object_key = mocha_values_create_symbol(values, "_sources");
	const mocha_object* source_map_object = mocha_context_lookup(context, temp_symbol_object_key);
	return source_map_object;
}

static void add_to_source_map(mocha_context* context, const mocha_object* symbol_object, const mocha_object* blob_object)
{
	mocha_values* values = context->values;
	const mocha_object* source_map_object = get_source_map(context, values);
	if (!source_map_object) {
		source_map_object = mocha_values_create_map(values, 0, 0);
	}
	const mocha_map* map = mocha_object_map(source_map_object);
	const mocha_object* adds[] = {symbol_object, blob_object};
	const mocha_object* new_map = mocha_map_assoc(map, values, adds, 2);
	const mocha_object* temp_key = mocha_values_create_symbol(values, "_sources");
	mocha_context_add(context, temp_key, new_map);
	
}

static void on_load_source(void* user_data, const uint8_t* octets, int32_t result)
{
	resource_state* self = (resource_state*) user_data;

	MOCHA_LOG("Loading source done for '%s' %d", self->debug_name, result);
	const mocha_object* source_string_object = mocha_values_create_string_from_cstr_ex(self->values, (const char*) octets, result);
	mocha_context* root_context = (mocha_context*)self->values->runtime->root_context;
	add_to_source_map(root_context, self->symbol_object, source_string_object);
	MOCHA_RESULT_VALUE(self->callback_info, source_string_object);
	tyran_free(self);
}

const mocha_object* get_existing_source_string(mocha_values* values, const mocha_object* symbol_object)
{
	const mocha_context* root_context = values->runtime->root_context;
	const mocha_object* source_map_object = get_source_map(root_context, values);

	if (!source_map_object) {
		return 0;
	}

	return mocha_map_lookup(mocha_object_map(source_map_object), symbol_object);
}

// const struct mocha_object* mocha_map_assoc(const mocha_map* map, mocha_values* values, const mocha_object** adds, size_t add_count)

MOCHA_FUNCTION(source_func)
{
	const mocha_object* argument = arguments->objects[1];
	const mocha_keyword* keyword = mocha_object_keyword(argument, "source");
	const mocha_object* symbol_object = mocha_values_create_symbol_from_hash(context->values, keyword->hash);
	const mocha_object* source_string_object = get_existing_source_string(context->values, symbol_object);
	if (source_string_object) {
		mocha_hashed_strings* strings = context->values->hashed_strings;
		const char* debug_source_name = mocha_hashed_strings_lookup(strings, keyword->hash);
		MOCHA_LOG("Returning cached value for '%s'", debug_source_name);
		return  source_string_object);
	} else {
		char filename[128];

		mocha_hashed_strings* strings = context->values->hashed_strings;
		const char* resource = mocha_hashed_strings_lookup(strings, keyword->hash);

		tyran_strncpy(filename, 128, resource, tyran_strlen(resource));
		tyran_strncat(filename, ".nim", 128);
		resource_state* state = tyran_malloc(sizeof(resource_state));
		state->debug_name = filename;
		state->values = context->values;
		state->callback_info = result_callback;
		state->symbol_object = symbol_object;

		burst_file_loader_init(&state->loader);
		burst_file_loader_load(&state->loader, filename, (void*) state, on_load_source);
	}
}

static void on_load(void* user_data, const uint8_t* octets, int32_t result)
{
	resource_state* self = (resource_state*) user_data;

	MOCHA_LOG("Loading done for '%s' %d", self->debug_name, result);
	// MOCHA_LOG("Data %s", (const char*) octets);
	MOCHA_RESULT_VALUE(self->callback_info, mocha_values_create_blob(self->values, octets, result));
	tyran_free(self);
}

MOCHA_FUNCTION(resource_func)
{
	const mocha_object* argument = arguments->objects[1];
	const mocha_keyword* keyword = mocha_object_keyword(argument, "resource");
	mocha_hashed_strings* strings = context->values->hashed_strings;
	const char* resource = mocha_hashed_strings_lookup(strings, keyword->hash);
	resource_state* state = tyran_malloc(sizeof(resource_state));

	state->debug_name = resource;
	state->values = context->values;
	state->callback_info = result_callback;
	burst_file_loader_init(&state->loader);
	burst_file_loader_load(&state->loader, resource, (void*) state, on_load);
}

void mocha_extended_core_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(resource, mocha_true);
	MOCHA_DEF_FUNCTION(source, mocha_true);
}
*/
