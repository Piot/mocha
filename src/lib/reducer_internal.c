#include <mocha/context.h>
#include <mocha/list.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/reducer_internal.h>
#include <mocha/runner.h>
#include <mocha/values.h>
#include <tyran/tyran_clib.h>

const struct mocha_object* mocha_reducer_reduce_internal(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_init init, mocha_c_fn_reducer_work work, const char* debug)
{
	mocha_values* values = context->values;
	const mocha_object* start = init(values);

	const mocha_object* evaled = mocha_runner_eval_arguments_rest(context, arguments);
	const mocha_sequence* seq = mocha_object_sequence(evaled);
	size_t count = mocha_sequence_count(seq);
	const mocha_object* a = start;
	size_t index = 0;
	if (count > 1) {
		index = 1;
		a = mocha_sequence_get(seq, values, 0);
	}
	for (size_t i = index; i < count; ++i) {
		const mocha_object* b = mocha_sequence_get(seq, values, i);
		a = work(values, a, b);
	}

	return a;
}

const struct mocha_object* mocha_reducer_reduce_internal_no_init(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work work, const char* debug)
{
	mocha_values* values = context->values;

	if (arguments->count < 3) {
		MOCHA_ERROR("Must have at least two arguments");
		return 0;
	}
	const mocha_object* evaled = mocha_runner_eval_arguments_rest(context, arguments);
	const mocha_sequence* seq = mocha_object_sequence(evaled);
	size_t count = mocha_sequence_count(seq);
	const mocha_object* a = mocha_sequence_get(seq, values, 0);
	for (size_t i = 1; i < count; ++i) {
		const mocha_object* b = mocha_sequence_get(seq, values, i);
		a = work(values, a, b);
	}

	return a;
}

const struct mocha_object* mocha_reducer_reduce_internal_check(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work_check work, const char* debug)
{
	mocha_values* values = context->values;
	mocha_boolean should_continue;

	if (arguments->count < 2) {
		MOCHA_ERROR("Must have at least one arguments");
		return 0;
	}
	const mocha_object* evaled = mocha_runner_eval_arguments_rest(context, arguments);
	const mocha_sequence* seq = mocha_object_sequence(evaled);
	size_t count = mocha_sequence_count(seq);
	const mocha_object* a = mocha_sequence_get(seq, values, 0);
	const mocha_object* result = mocha_values_create_boolean(values, mocha_true);

	for (size_t i = 1; i < count; ++i) {
		const mocha_object* b = mocha_sequence_get(seq, values, i);
		result = work(values, a, b, &should_continue);
		if (!should_continue) {
			break;
		}
		a = b;
	}

	return result;
}

const struct mocha_object* mocha_reducer_reduce_internal_check_with_init(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_init init, mocha_c_fn_reducer_check work, const char* debug)
{
	mocha_values* values = context->values;
	mocha_boolean should_continue;
	const mocha_object* start = init(values);
	if (arguments->count < 2) {
		return start;
	}
	const mocha_object* evaled = mocha_runner_eval_arguments_rest(context, arguments);
	const mocha_sequence* seq = mocha_object_sequence(evaled);
	size_t count = mocha_sequence_count(seq);
	const mocha_object* result = mocha_values_create_boolean(values, mocha_true);

	for (size_t i = 0; i < count; ++i) {
		const mocha_object* a = mocha_sequence_get(seq, values, i);
		result = work(values, a, &should_continue);
		if (!should_continue) {
			break;
		}
	}

	return result;
}