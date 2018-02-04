#include <mocha/core.h>
#include <mocha/core_arithmetic.h>
#include <mocha/core_bit.h>
#include <mocha/core_collection.h>
#include <mocha/core_compare.h>
#include <mocha/core_def.h>
#include <mocha/core_execution.h>
#include <mocha/core_import.h>
#include <mocha/core_logic.h>
#include <mocha/core_math.h>
#include <mocha/core_memoize.h>
#include <mocha/core_serialize.h>
#include <mocha/core_thread.h>
#include <mocha/def_function.h>
#include <mocha/hashed_strings.h>
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
MOCHA_FUNCTION(dbg_ptr_func)
{
		const mocha_object* o = arguments->objects[1];
		const mocha_object* result = mocha_values_create_integer(context->values,  (int) o);
		return result;
}

MOCHA_FUNCTION(dbg_sleep_func)
{
		struct timespec requested, remaining;
		const mocha_object* o = arguments->objects[1];
		requested.tv_sec = mocha_object_integer(o);
		requested.tv_nsec = 0;
		nanosleep(&requested, &remaining);
		const mocha_object* result = mocha_values_create_integer(context->values,  0);
		return result;
}
*/

MOCHA_FUNCTION(fail_func)
{
	const mocha_object* argument = arguments->objects[1];
	MOCHA_ERROR("Error:%s", mocha_print_object_debug_str(argument));
	return 0;
}

MOCHA_FUNCTION(println_func)
{
	const mocha_object* rest_object = mocha_runner_eval_arguments_rest(context, arguments);
	MOCHA_OUTPUT("%s", mocha_print_list_debug_str(context->values, mocha_object_list(rest_object)));
	return mocha_values_create_nil(context->values);
}

MOCHA_FUNCTION(ns_func)
{
	const mocha_object* prefix = arguments->objects[1];
	const mocha_symbol* prefix_symbol = mocha_object_symbol(prefix);
	if (prefix_symbol->has_namespace) {
		MOCHA_ERROR("Can not have namespace!");
		return 0;
	}

	mocha_runtime* runtime = context->runtime;

	const mocha_object* namespace_context_object = mocha_values_create_context(context->values, context->runtime->create_context_parent, "ns");
	mocha_context_add((mocha_context*) context->runtime->root_context, prefix, namespace_context_object);
	runtime->namespace_context = mocha_object_context(namespace_context_object);
	return mocha_values_create_nil(context->values);
}

MOCHA_FUNCTION(quote_func)
{
	return arguments->objects[1];
}

MOCHA_FUNCTION(unquote_func)
{
	return mocha_runner_eval(context, arguments->objects[1]);
}

typedef struct log_state {
	int dummy;
} log_state;

static void print_sequence(struct mocha_values* values, const struct mocha_object* sequence_object)
{
	const mocha_sequence* seq = mocha_object_sequence(sequence_object);
	size_t count = mocha_sequence_count(seq);
	char temp[1024];
	temp[0] = 0;

	for (size_t i = 0; i < count; ++i) {
		const mocha_object* argument = mocha_sequence_get(seq, values, i);
		if (i != 0) {
			strcat(temp, " ");
		}
		strcat(temp, mocha_print_object_debug_str(argument));
	}
	MOCHA_OUTPUT("log: %s", temp);
}

MOCHA_FUNCTION(log_func)
{
	const mocha_object* rest_object = mocha_runner_eval_arguments_rest(context, arguments);
	print_sequence(context->values, rest_object);

	const mocha_object* nil = mocha_values_create_nil(context->values);
	return nil;
}

#if 1
MOCHA_FUNCTION(require_func)
{
	const mocha_object* nil = mocha_values_create_nil(context->values);
	return nil;
}

MOCHA_FUNCTION(int_func)
{
	return arguments->objects[0];
}

#endif

void mocha_core_define_context(mocha_context* context, mocha_values* values)
{
	mocha_core_arithmetic_define_context(context, values);
	mocha_core_bit_define_context(context, values);
	mocha_core_collection_define_context(context, values);
	mocha_core_compare_define_context(context, values);
	mocha_core_math_define_context(context, values);
	mocha_core_thread_define_context(context, values);
	mocha_core_serialize_define_context(context, values);
	/*
	mocha_core_import_define_context(context, values);
	*/
	mocha_core_def_define_context(context, values);
	mocha_core_execution_define_context(context, values);
	mocha_core_logic_define_context(context, values);
	mocha_core_memoize_define_context(context, values);

	MOCHA_DEF_FUNCTION(unquote);
	MOCHA_DEF_FUNCTION(fail);
	MOCHA_DEF_FUNCTION(println);
	MOCHA_DEF_FUNCTION(log);
	MOCHA_DEF_FUNCTION(quote);
	MOCHA_DEF_FUNCTION(ns);

#if 1
	MOCHA_DEF_FUNCTION(require);
	MOCHA_DEF_FUNCTION(int);
#endif
	// DEBUG
	// MOCHA_DEF_FUNCTION(dbg_ptr);
	// MOCHA_DEF_FUNCTION(dbg_sleep);
}
