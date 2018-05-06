#include <mocha/context.h>
#include <mocha/execute.h>
#include <mocha/list.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/reducer_internal.h>
#include <mocha/runner.h>
#include <mocha/values.h>
#include <tiny_libc/tiny_libc.h>

const struct mocha_object* mocha_reducer_reduce_script(const struct mocha_context* context, const struct mocha_list* arguments, const char* debug)
{
	mocha_values* values = context->values;

	const mocha_object* invokable_object = mocha_runner_eval(context, arguments->objects[1]);
	size_t sequence_index = 2;
	if (arguments->count > 3) {
		sequence_index = 3;
	}
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[sequence_index]);
	const mocha_sequence* seq = mocha_object_sequence(sequence_object);

	size_t count = mocha_sequence_count(seq);
	const mocha_object* start;
	if (sequence_index == 3) {
		start = mocha_runner_eval(context, arguments->objects[2]);
	} else {
		if (invokable_object->type == mocha_object_type_internal_function) {
			start = execute_0(context, invokable_object);
		}
	}

	if (count == 0) {
		return start;
	}
	const mocha_object* a = start;
	size_t index = 0;
	if (count > 1 && sequence_index == 2) {
		index = 1;
		a = mocha_sequence_get(seq, values, 0);
	}
	for (size_t i = index; i < count; ++i) {
		const mocha_object* b = mocha_sequence_get(seq, values, i);
		a = execute_2(context, invokable_object, a, b);
	}

	return a;
}
