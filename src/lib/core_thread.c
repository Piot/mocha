#include <mocha/context.h>
#include <mocha/core_thread.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/runtime.h>
#include <mocha/runner.h>
#include <mocha/print.h>
#include <mocha/type.h>
#include <mocha/values.h>
#include <stdlib.h>
#include <tyran/tyran_clib.h>

static const mocha_object* thread_last_list(mocha_values* values, const mocha_list* list, const mocha_object* a)
{
	     mocha_list new_list;

	     mocha_list_init_prepare(&new_list,  &values->object_references, list->count + 1);
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
		return 0;
	}
	const mocha_object* a = mocha_runner_eval(context, arguments->objects[1]);
	size_t count = arguments->count;
	mocha_list empty_list;
	for (size_t i=2; i<count; ++i) {
		const mocha_object* o = arguments->objects[i];
		const mocha_list* call_list;
		if (mocha_object_is_list(o)) {
			call_list = mocha_object_list(o);
		} else {
			mocha_list_init(&empty_list, &context->values->object_references, &o, 1);
			call_list = &empty_list;
		}
		const mocha_object* execute_list_object = thread_last_list(context->values, call_list, a);
		a = mocha_runner_eval(context, execute_list_object);
	}
	return a;
}


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

MOCHA_FUNCTION(thread_first_func)
{
	if (arguments->count < 2) {
		return 0;
	}
	const mocha_object* a = mocha_runner_eval(context, arguments->objects[1]);
	size_t count = arguments->count;
	mocha_list empty_list;
	for (size_t i=2; i<count; ++i) {
		const mocha_object* o = arguments->objects[i];
		const mocha_list* call_list;
		if (mocha_object_is_list(o)) {
			call_list = mocha_object_list(o);
		} else {
			mocha_list_init(&empty_list, &context->values->object_references, &o, 1);
			call_list = &empty_list;
		}
		const mocha_object* execute_list_object = thread_first_list(context->values, call_list, a);
		a = mocha_runner_eval(context, execute_list_object);
	}
	return a;
}

void mocha_core_thread_define_context(struct mocha_context* context, struct mocha_values* values)
{
	MOCHA_DEF_FUNCTION_EX(thread_first, "->");
	MOCHA_DEF_FUNCTION_EX(thread_last, "->>");
}
