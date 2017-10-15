#include <mocha/core_logic.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/reducer_internal.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tyran/tyran_clib.h>

/*
typedef struct and_next_info
{
	const mocha_list* arguments;
	resolve_callback callback_info;
	const mocha_context* context;
	mocha_boolean check_against;
} and_next_info;

typedef struct and_next_argument_info
{
	int index;
	and_next_info* parent;
} and_next_argument_info;

static void and_next_func_done(and_next_info* self, const mocha_object* o)
{
	MOCHA_RESULT_VALUE(self->callback_info, o);
	tyran_free(self);
}

static void and_next_func(and_next_info* self, int index);

static void and_next_argument_done(void* user_data, const mocha_object* result)
{
	and_next_argument_info* self = (and_next_argument_info*) user_data;
	int last_index = self->parent->arguments->count - 1;

	if (self->index == last_index || self->parent->check_against == mocha_object_truthy(result)) {
		and_next_func_done(self->parent, result);
		tyran_free(self);
	} else {
		and_next_func(self->parent, self->index + 1);
		tyran_free(self);
	}
}

static void and_next_func(and_next_info* self, int index)
{
	const mocha_object* object_to_be_evaluated = self->arguments->objects[index];
	and_next_argument_info* info = tyran_malloc(sizeof(and_next_argument_info));

	info->parent = self;
	info->index = index;
	resolve_closure_ex(self->context, object_to_be_evaluated, info, and_next_argument_done);
}



MOCHA_FUNCTION(or_func)
{
	if (arguments->count == 1) {
		const mocha_object* result = mocha_values_create_boolean(context->values);
		return  result);
		return;
	}

	and_next_info* info = tyran_malloc(sizeof(and_next_info));
	info->arguments = arguments;
	info->callback_info = result_callback;
	info->context = context;
	info->check_against = mocha_true;
	and_next_func(info, 1);
}

MOCHA_FUNCTION(not_func)
{
	const mocha_object* argument = arguments->objects[1];

	if (argument->type != mocha_object_type_nil && argument->type != mocha_object_type_true) {
		MOCHA_LOG("Error: Not boolean. Can not do not()");
		// error->code = mocha_error_code_expected_boolean_value;
		return;
	}
	const mocha_object* o = mocha_values_create_boolean(context->values, !mocha_object_boolean(argument));
	return  o);
}

*/

const struct mocha_object* do_and_init(struct mocha_values* values)
{
	return mocha_values_create_boolean(values, mocha_true);
}

const struct mocha_object* do_and(struct mocha_values* values, const struct mocha_object* a, mocha_boolean* should_continue)
{
	mocha_boolean is_truthy = mocha_object_truthy(a);
	*should_continue = is_truthy;
	return a;
}

MOCHA_FUNCTION(and_func)
{
	return mocha_reducer_reduce_internal_check_with_init(context, arguments, do_and_init, do_and, "and");
}

MOCHA_FUNCTION(if_func)
{
	const mocha_object* condition = mocha_runner_eval(context, arguments->objects[1]);

	mocha_boolean satisfied = mocha_object_truthy(condition);
	size_t eval_index = satisfied ? 2 : 3;
	const mocha_object* eval_object;

	if (eval_index >= arguments->count) {
		eval_object = mocha_values_create_nil(context->values);
	} else {
		eval_object = mocha_values_create_eval(context->values, arguments->objects[eval_index]);
	}

	return eval_object;
}

/*
   MOCHA_FUNCTION(case_func)
   {
		const mocha_object* compare_value = mocha_runtime_eval(runtime, arguments->objects[1], &runtime->error);
		for (size_t i = 2; i < arguments->count; i += 2) {
				const mocha_object* when_value = arguments->objects[i];
				if (mocha_object_equal(compare_value, when_value)) {
						const mocha_object* when_argument = mocha_runtime_eval(runtime, arguments->objects[i + 1], &runtime->error);
						return when_argument;
				}
		}

		if ((arguments->count % 2) != 0) {
				const mocha_object* default_value = mocha_runtime_eval(runtime, arguments->objects[arguments->count - 1], &runtime->error);
				return default_value;
		}

		return mocha_values_create_nil(runtime->values);
   }

*/
void mocha_core_logic_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(if);
	MOCHA_DEF_FUNCTION(and);
	//	MOCHA_DEF_FUNCTION(or);
	//	MOCHA_DEF_FUNCTION(not);
}
