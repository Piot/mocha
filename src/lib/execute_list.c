#include <mocha/execute_list.h>

#include <mocha/context.h>
#include <mocha/list.h>
#include <mocha/object.h>

#include <mocha/log.h>
#include <mocha/type.h>
#include <mocha/resolve_arguments.h>

#include <mocha/execute.h>
#include <tyran/tyran_clib.h>

typedef struct resolve_list_info {
	const mocha_list* arguments;
	const mocha_context* context;
	resolve_callback callback_info;
} resolve_list_info;

typedef struct execute_list_arguments_info {
	resolve_list_info* info;
	const mocha_object* fn;
} execute_list_arguments_info;

static void execute_list_done(void* user_data, const mocha_object* result)
{
   // MOCHA_LOG("execute_list executed");
	resolve_list_info* self = (resolve_list_info*) user_data;
	resolve_callback callback_info = self->callback_info;
	tyran_free(self);
	callback_info.callback(callback_info.user_data, result);
}

static void execute_list_arguments_resolved(void* user_data, const struct mocha_list* resolved_arguments)
{
	//MOCHA_LOG("execute_list arguments resolved");

	execute_list_arguments_info* self = (execute_list_arguments_info*) user_data;
	resolve_callback callback_info;
	callback_info.user_data = self->info;
	callback_info.callback = execute_list_done;
	execute((mocha_context*) self->info->context, self->fn, resolved_arguments, callback_info);
	tyran_free(self);
}

static mocha_boolean function_wants_to_evaluate_arguments(const mocha_object* fn)
{
	mocha_boolean should_evaluate_arguments = mocha_true;

	if (fn->object_type) {
		should_evaluate_arguments = fn->object_type->eval_all_arguments;
	}
	return should_evaluate_arguments;
}

static void execute_list_function_ready(void* user_data, const mocha_object* fn_object)
{
	// MOCHA_LOG("execute_list function ready");

	resolve_list_info* self = (resolve_list_info*) user_data;

	if (!mocha_object_is_invokable(fn_object)) {
		MOCHA_LOG("ERROR: Object resolved is not an invokable!");
		return;
	}

	if (function_wants_to_evaluate_arguments(fn_object)) {
		execute_list_arguments_info* info = tyran_malloc(sizeof(execute_list_arguments_info));
		info->info = self;
		info->fn = fn_object;
		resolve_arguments_callback(self->context, self->arguments, info, execute_list_arguments_resolved);
	} else {
		resolve_callback callback_info;
		callback_info.user_data = self;
		callback_info.callback = execute_list_done;
		execute((mocha_context*) self->context, fn_object, self->arguments, callback_info);
	}
}

void execute_list(const mocha_context* context, const mocha_list* list, resolve_callback callback)
{
	if (context == 0) {
		MOCHA_LOG("execute_list is null!");
	}
	//MOCHA_LOG("execute_list");
	resolve_list_info* info = tyran_malloc(sizeof(resolve_list_info));

	info->arguments = list;
	info->context = context;
	info->callback_info = callback;

	const mocha_object* invokable_object = list->objects[0];
	resolve_closure_ex(context, invokable_object, info, execute_list_function_ready);
}
