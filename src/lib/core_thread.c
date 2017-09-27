#include <mocha/context.h>
#include <mocha/core_thread.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/values.h>
#include <stdlib.h>
#include <tyran/tyran_clib.h>

static const mocha_object* thread_first_list(struct mocha_values* values, const mocha_list* list, const mocha_object* a)
{
	mocha_list new_list;

	mocha_list_init_prepare(&new_list, &values->object_references,list->count + 1);
	size_t first_index = 0;

	if (list->count > 0) {
		new_list.objects[first_index++] = list->objects[0];
	}
	new_list.objects[first_index++] = a;

	if (list->count > 1) {
		memcpy(&new_list.objects[first_index], &list->objects[1], sizeof(mocha_object*) * (list->count - 1));
	}
	const mocha_object* value = mocha_values_create_list(values, new_list.objects, new_list.count);

	return value;
}

typedef struct thread_first_info {
	const mocha_list* arguments;
	const mocha_context* context;
	resolve_callback resolve_info;
} thread_first_info;

typedef struct thread_first_argument_info {
	thread_first_info* parent;
	size_t index;
} thread_first_argument_info;

static void thread_next(thread_first_info* parent, int i, const mocha_object* result);

static void thread_pass_done(void* user_data, const mocha_object* result)
{
	thread_first_argument_info* self = (thread_first_argument_info*) user_data;
	thread_first_info* parent = self->parent;
	size_t index = self->index;
	tyran_free(self);
	self = 0;

	if (index + 1 == parent->arguments->count) {
		MOCHA_RESULT_VALUE(parent->resolve_info, result);
		tyran_free(parent);
	} else {
		thread_next(parent, index + 1, result);
	}
}

static void thread_next(thread_first_info* parent, int i, const mocha_object* in_data)
{
	const mocha_object* o = parent->arguments->objects[i];
	mocha_list empty_list;
	const mocha_list* list = &empty_list;

	if (mocha_object_is_list(o)) {
		list = mocha_object_list(o);
	} else {
		mocha_list_init(&empty_list, &parent->context->values->object_references, &o, 1);
	}

	const mocha_object* created_list = thread_first_list(parent->context->values, list, in_data);
	thread_first_argument_info* self = (thread_first_argument_info*) tyran_malloc(sizeof(thread_first_argument_info));
	self->index = i;
	self->parent = parent;
	resolve_closure_ex(parent->context, created_list, self, thread_pass_done);
}

static void thread_start_value_done(void* user_data, const mocha_object* start_value)
{
	thread_first_info* self = (thread_first_info*) user_data;

	thread_next(self, 2, start_value);
}

MOCHA_FUNCTION(thread_first_func)
{
	if (arguments->count < 2) {
		MOCHA_RESULT_VALUE(result_callback, mocha_values_create_nil(context->values));
	}
	thread_first_info* self = (thread_first_info*) tyran_malloc(sizeof(thread_first_info));
	self->resolve_info = result_callback;
	self->context = context;
	self->arguments = arguments;
	const mocha_object* start_value = self->arguments->objects[1];
	resolve_closure_ex(self->context, start_value, self, thread_start_value_done);
}

/*

   static const mocha_object* thread_last_list(mocha_values* values, const mocha_list* list, const mocha_object* a)
   {
		mocha_list new_list;
		mocha_list_init_prepare(&new_list, list->count + 1);
		if (list->count > 1) {
				memcpy(new_list.objects, list->objects, sizeof(mocha_object*) * list->count);
		}
		new_list.objects[new_list.count - 1] = a;
		const mocha_object* value = mocha_values_create_list(values, new_list.objects, new_list.count);

		return value;
   }

   MOCHA_FUNCTION(thread_last_func)
   {
		if (arguments->count < 2) {
				return mocha_values_create_nil(runtime->values);
		}
		const mocha_object* result = arguments->objects[1];
		for (size_t i = 2; i < arguments->count; ++i) {
				const mocha_object* o = arguments->objects[i];
				mocha_list empty_list;
				const mocha_list* list = &empty_list;

				if (mocha_object_is_list(o)) {
						list = mocha_object_list(o);
				} else {
						mocha_list_init(&empty_list, &o, 1);
				}
				const mocha_object* created_list = thread_last_list(context->values,  list, result);

				mocha_error error;
				mocha_error_init(&error);
				result = mocha_runtime_eval(runtime, created_list, &error);
		}

		return result;
   }

 */

void mocha_core_thread_define_context(struct mocha_context* context, struct mocha_values* values)
{
	MOCHA_DEF_FUNCTION_EX(thread_first, "->", mocha_false);
	// MOCHA_DEF_FUNCTION_EX(thread_last, "->>", mocha_false);
}
