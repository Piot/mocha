#include <mocha/caller.h>
#include <mocha/context.h>
#include <mocha/list.h>
#include <mocha/string.h>
#include <mocha/log.h>
#include <mocha/values.h>
#include <mocha/execute.h>

static void call_script_function_ex(const mocha_context* context, const char* update_name, const mocha_list* args, set_result_callback result_callback, void* user_data)
{
	// MOCHA_LOG("call script: context %p values: %p objects:%p", (void*) context, (void*) context->values, (void*) context->values->objects);
	const struct mocha_object* key = mocha_values_create_symbol(context->values, update_name);
	const mocha_context* root_context = context;

	if (root_context == 0) {
		MOCHA_LOG("Root context is null!");
	}
	const struct mocha_object* update_func = mocha_context_lookup(root_context, key);

	if (update_func == 0) {
		MOCHA_LOG("Update not ready yet");
		return;
	}
	const struct mocha_function* fn = mocha_object_function(update_func);
	resolve_callback result_callback_info;
	result_callback_info.callback = result_callback;
	result_callback_info.user_data = user_data;

	script_execute(context, fn, args, result_callback_info);
}

void mocha_call_script_function_2(const struct mocha_context* context, const char* update_name, const struct mocha_object* argument, const struct mocha_object* argument2, set_result_callback result_callback, void* user_data)
{
	static mocha_list args;
	static const struct mocha_object* temp_map_arguments[3];

	temp_map_arguments[0] = 0;
	temp_map_arguments[1] = argument;
	temp_map_arguments[2] = argument2;
	mocha_list_init(&args, &context->values->object_references, temp_map_arguments, 3);
	call_script_function_ex(context, update_name, &args, result_callback, user_data);
}

void mocha_call_script_function(const struct mocha_context* context, const char* update_name, const struct mocha_object* argument, set_result_callback result_callback, void* user_data)
{
	static mocha_list args;
	static const struct mocha_object* temp_map_arguments[2];

	temp_map_arguments[0] = 0;
	temp_map_arguments[1] = argument;
	mocha_list_init(&args, &context->values->object_references, temp_map_arguments, 2);
	call_script_function_ex(context, update_name, &args, result_callback, user_data);
}
