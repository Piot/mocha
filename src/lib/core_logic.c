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

const struct mocha_object* do_or_init(struct mocha_values* values)
{
	return mocha_values_create_boolean(values, mocha_false);
}

const struct mocha_object* do_or(struct mocha_values* values, const struct mocha_object* a, mocha_boolean* should_continue)
{
	mocha_boolean is_truthy = mocha_object_truthy(a);
	*should_continue = !is_truthy;
	return a;
}

MOCHA_FUNCTION(or_func)
{
	return mocha_reducer_reduce_internal_check_with_init(context, arguments, do_or_init, do_or, "or");
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
		eval_object = mocha_values_create_eval(context->values, context, arguments->objects[eval_index]);
	}

	return eval_object;
}

MOCHA_FUNCTION(case_func)
{
	const mocha_object* compare_value = mocha_runner_eval(context, arguments->objects[1]);
	for (size_t i = 2; i < arguments->count; i += 2) {
		const mocha_object* when_value = arguments->objects[i];
		if (mocha_object_equal(compare_value, when_value)) {
			const mocha_object* when_argument = mocha_runner_eval(context, arguments->objects[i + 1]);
			return when_argument;
		}
	}

	if ((arguments->count % 2) != 0) {
		const mocha_object* default_value = mocha_runner_eval(context, arguments->objects[arguments->count - 1]);
		return default_value;
	}

	return mocha_values_create_nil(context->values);
}

MOCHA_FUNCTION(not_func)
{
	const mocha_object* argument = mocha_runner_eval(context, arguments->objects[1]);

	if (argument->type != mocha_object_type_nil && argument->type != mocha_object_type_true) {
		MOCHA_LOG("Error: Not boolean. Can not do not()");
		return 0;
	}
	const mocha_object* o = mocha_values_create_boolean(context->values, !mocha_object_boolean(argument));
	return o;
}

void mocha_core_logic_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(if);
	MOCHA_DEF_FUNCTION(and);
	MOCHA_DEF_FUNCTION(or);
	MOCHA_DEF_FUNCTION(not);
	MOCHA_DEF_FUNCTION(case);
}
