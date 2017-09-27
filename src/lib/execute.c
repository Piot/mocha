#include <mocha/context.h>
#include <mocha/execute.h>
#include <mocha/function.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/type.h>
#include <mocha/values.h>
#include <tyran/tyran_clib.h>

static void c_execute(const mocha_context* context, const mocha_c_fn c_fn, const mocha_list* arguments_list, resolve_callback resolve)
{

	c_fn(resolve, context, arguments_list);
}

static const mocha_context* create_invoke_context(const mocha_context* context, const mocha_function* fn, const mocha_list* arguments_list)
{
	const mocha_list* args = &fn->arguments->data.list;
	mocha_context* new_context = (mocha_context*) mocha_object_context(mocha_values_create_context(context->values, fn->context, "fn->context")); //, "fn->context");
	// mocha_context_print_debug("function context:", new_context);
	size_t minimum_number_of_arguments = args->count;
	// mocha_boolean var_args = mocha_false;

	for (size_t arg_count = 0; arg_count < args->count; ++arg_count) {
		const mocha_object* arg = args->objects[arg_count];

		if (arg->type != mocha_object_type_symbol) {
			MOCHA_LOG("It is not a symbol what is up with that!?");
			MOCHA_LOG("Illegal argument:%s", mocha_print_object_debug_str(arg));
			return 0;
		}

		/*if (mocha_string_equal_str(arg->data.symbol.string, "&")) {
			const mocha_object* rest_arg = args->objects[arg_count + 1];
			const mocha_object* list = mocha_values_create_list(context->values, &arguments_list->objects[1 + arg_count], arguments_list->count - arg_count - 1);
			minimum_number_of_arguments = arg_count;
			var_args = mocha_true;
			mocha_context_add(new_context, rest_arg, list);
		} else {*/
			const mocha_object* value = arguments_list->objects[1 + arg_count];
			mocha_context_add(new_context, arg, value);
		//}
	}

	if (arguments_list->count - 1 < minimum_number_of_arguments) {
		MOCHA_LOG("Illegal number of arguments: %lu (expected %d '%s'", arguments_list->count, (int) minimum_number_of_arguments,  mocha_print_object_debug_str(fn->arguments));
		return 0;
	}
	// mocha_runtime_push_context(self, new_context);
	return new_context;
}

typedef struct script_execute_info {
	resolve_callback callback_info;
} script_execute_info;

static void script_execute_done(void* user_data, const struct mocha_object* result)
{
	script_execute_info* self = (script_execute_info*) user_data;

	// mocha_runtime_destroy_context(self->closure->context->runtime, (mocha_context*)self->closure->context);
	resolve_callback next = self->callback_info;
	tyran_free(self);
	next.callback(next.user_data, result);
}

void script_execute(const mocha_context* context, const mocha_function* script_fn, const mocha_list* arguments_list, resolve_callback resolve)
{
	if (context == 0) {
		MOCHA_LOG("Script_Execute is null");
	}
	const mocha_context* fn_context = create_invoke_context(context, script_fn, arguments_list);

	if (fn_context == 0) {
		MOCHA_LOG("Script-execute fn-context is null");
	}

	if (fn_context->parent == 0) {
		MOCHA_LOG("Script-execute fn-parent is null");
	}
	script_execute_info* info = tyran_malloc(sizeof(script_execute_info));
	info->callback_info = resolve;
	// node->is_special = script_fn->is_special;

	resolve_closure_ex(fn_context, script_fn->code, info, script_execute_done);
}

void execute(const mocha_context* context, const mocha_object* object_fn, const mocha_list* arguments, resolve_callback resolve)
{
	// o = find_cache(self, l);
	// const mocha_object* o = 0;
	// if (o) { push_result(o); }
	// if (!o) {
	// MOCHA_LOG("execute '%s'", mocha_print_object_debug_str(object_fn));
	const mocha_c_fn c_fn = object_fn->object_type->invoke;

	if (c_fn != 0) {
		c_execute(context, c_fn, arguments, resolve);
	} else if (mocha_object_is_function(object_fn)) {
		const mocha_function* fn = mocha_object_function(object_fn);
		// MOCHA_LOG("script execute fn %p", (void*) object_fn);

		script_execute(context, fn, arguments, resolve);
	} else {
		MOCHA_LOG("Invoke failed");
	}
}
