#include <mocha/copy_object.h>
#include <mocha/core_memoize.h>
#include <mocha/def_function.h>
#include <mocha/execute.h>
#include <mocha/log.h>
#include <mocha/map_utils.h>
#include <mocha/print.h>
#include <mocha/reducer_internal.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tiny_libc/tiny_libc.h>

typedef struct memoize_some_info {
	const mocha_object* invokable;
	const mocha_object* cached_map_object;
} memoize_some_info;

static const struct mocha_object* memoize_some_fn(const struct mocha_context* context, const struct mocha_list* arguments)
{
	const mocha_object* self_object = arguments->objects[0];
	memoize_some_info* self = (memoize_some_info*) self_object->user_data;
	const mocha_object* a = mocha_runner_eval(context, arguments->objects[1]);
	if (mocha_object_is_nil(a)) {
		return a;
	}
	if (!a) {
		MOCHA_ERROR("A can not be null!");
	}

	const mocha_object* cached_result = mocha_map_lookup(mocha_object_map(self->cached_map_object), a);
	if (cached_result) {
		return cached_result;
	}

	const mocha_object* result = execute_1(context, self->invokable, a);
	if (mocha_object_is_nil(result)) {
		// MOCHA_ERROR("Can not return nil!");
		return result;
	}

	if (!result) {
		MOCHA_ERROR("A result can not be null!");
	}

	mocha_values* persistent_values = context->runtime->cached_values;

	const mocha_object* adds[2];

	adds[0] = mocha_values_deep_copy(persistent_values, a);
	adds[1] = mocha_values_deep_copy(persistent_values, result);
	const mocha_object* new_map_object = mocha_map_assoc(mocha_object_map(self->cached_map_object), persistent_values, adds, 2);
	self->cached_map_object = new_map_object;
	return result;
}

MOCHA_FUNCTION(memoize_some_func)
{
	const mocha_object* fn = mocha_runner_eval(context, arguments->objects[1]);

	memoize_some_info* info = tc_malloc_type(memoize_some_info);
	mocha_values* persistent_values = context->runtime->cached_values;
	info->cached_map_object = mocha_values_create_map(persistent_values, 0, 0);
	info->invokable = fn;

	static mocha_type memoize_type;
	memoize_type.invoke = memoize_some_fn;
	const mocha_object* o = mocha_values_create_internal_function_ex(context->values, &memoize_type, "memoize-some", info);
	return o;
}

void mocha_core_memoize_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION_EX(memoize_some, "memoize-some");
}
