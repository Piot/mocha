#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/resolve_arguments.h>
#include <mocha/runtime.h>
#include <mocha/values.h>
#include <stdlib.h>
#include <tyran/tyran_clib.h>

typedef struct resolve_arguments_info
{
	mocha_list target_arguments;
	void* root_user_data;
	set_arguments_callback callback;
} resolve_arguments_info;

typedef struct resolve_arguments_info_arg
{
	resolve_arguments_info* parent;
	int index;
} resolve_arguments_info_arg;

static mocha_boolean all_arguments_done(const resolve_arguments_info* info)
{
	for (size_t i=0; i<info->target_arguments.count; ++i) {
		if (info->target_arguments.objects[i] == 0) {
			return mocha_false;
		}
	}

	return mocha_true;
}

static void argument_ready(void* user_data, const struct mocha_object* object)
{
	resolve_arguments_info_arg* self = (resolve_arguments_info_arg*) user_data;
	resolve_arguments_info* root = self->parent;

	root->target_arguments.objects[self->index] = object;
	tyran_free(self);
	if (all_arguments_done(root)) {
		root->callback(root->root_user_data, &root->target_arguments);
		tyran_free(root);
	}
}

void resolve_arguments_callback(const mocha_context* context, const mocha_list* arguments, void* user_data, set_arguments_callback callback_info)
{
	if (context == 0) {
		MOCHA_LOG("Resolve arguments is null!");
	}
	resolve_arguments_info* info = tyran_malloc(sizeof(resolve_arguments_info));

	info->callback = callback_info;
	info->root_user_data = user_data;

	mocha_list_init_prepare(&info->target_arguments, &context->values->object_references, arguments->count);
	for (size_t i = 0; i < arguments->count; ++i) {
		resolve_arguments_info_arg* arg_info = tyran_malloc(sizeof(resolve_arguments_info_arg));
		arg_info->parent = info;
		arg_info->index = i;

		resolve_closure_ex(context, arguments->objects[i], arg_info, argument_ready);
	}
}
