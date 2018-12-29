/*

MIT License

Copyright (c) 2013 Peter Bjorklund

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#include <mocha/context.h>
#include <mocha/execute.h>
#include <mocha/list.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/reducer_internal.h>
#include <mocha/runner.h>
#include <mocha/values.h>
#include <tiny-libc/tiny_libc.h>

const struct mocha_object* mocha_reducer_reduce_internal(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_init init, mocha_c_fn_reducer_work work, const char* debug)
{
    (void)debug;

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

const struct mocha_object* mocha_reducer_reduce_internal_single(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work_single single, mocha_c_fn_reducer_work work, const char* debug)
{
    (void)debug;
	mocha_values* values = context->values;

	if (arguments->count < 2) {
		MOCHA_ERROR("Must have at least one arguments");
		return 0;
	}
	const mocha_object* evaled = mocha_runner_eval_arguments_rest(context, arguments);
	const mocha_sequence* seq = mocha_object_sequence(evaled);
	size_t count = mocha_sequence_count(seq);
	const mocha_object* a = mocha_sequence_get(seq, values, 0);
	if (count == 1) {
		return single(values, a);
	}
	for (size_t i = 1; i < count; ++i) {
		const mocha_object* b = mocha_sequence_get(seq, values, i);
		a = work(values, a, b);
	}

	return a;
}

const struct mocha_object* mocha_reducer_reduce_internal_single_fn(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work_check single, const char* debug)
{
    (void)debug;

	const mocha_object* predicate_fn_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* seq_object = mocha_runner_eval(context, arguments->objects[2]);

	mocha_values* values = context->values;

	if (arguments->count < 2) {
		MOCHA_ERROR("Must have at least one arguments");
		return 0;
	}

	const mocha_sequence* seq = mocha_object_sequence(seq_object);
	size_t count = mocha_sequence_count(seq);

	mocha_boolean should_continue;
	const mocha_object* a = mocha_values_create_nil(context->values);
	for (size_t i = 0; i < count; ++i) {
		a = mocha_sequence_get(seq, values, i);
		const mocha_object* predicate_value = execute_1(context, predicate_fn_object, a);
		a = single(values, predicate_value, a, &should_continue);
		if (!should_continue) {
			break;
		}
	}

	return a;
}

const struct mocha_object* mocha_reducer_reduce_internal_no_init(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work work, const char* debug)
{
    (void)debug;

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
    (void)debug;

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

const struct mocha_object* mocha_reducer_reduce_internal_check_with_init(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_init init, mocha_c_fn_reducer_work_single_check work, const char* debug)
{
    (void)debug;

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
