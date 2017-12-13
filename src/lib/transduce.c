#include <mocha/execute.h>
#include <mocha/list.h>
#include <mocha/object.h>
#include <mocha/runner.h>
#include <mocha/transduce.h>
#include <mocha/values.h>

const struct mocha_object* mocha_transduce_internal(const struct mocha_context* context, mocha_transducer_stepper stepper, const struct mocha_list* arguments, tyran_boolean insert_index)
{
	const mocha_object* predicate_fn_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* evaled_arguments = mocha_runner_eval(context, arguments->objects[2]);
	const mocha_sequence* seq = mocha_object_sequence(evaled_arguments);
	size_t count = mocha_sequence_count(seq);
	const mocha_object* temp[1024];
	size_t target_index = 0;
	for (size_t i = 0; i < count; ++i) {
		const mocha_object* item = mocha_sequence_get(seq, context->values, i);
		const struct mocha_object* predicate_value;
		if (insert_index) {
			const mocha_object* index_object = mocha_values_create_integer(context->values, i);
			predicate_value = execute_2(context, predicate_fn_object, index_object, item);
		} else {
			predicate_value = execute_1(context, predicate_fn_object, item);
		}
		mocha_boolean should_add_it;
		mocha_boolean should_continue;
		const mocha_object* object_to_add = stepper(predicate_value, item, &should_add_it, &should_continue);
		if (should_add_it) {
			temp[target_index++] = object_to_add;
		}
		if (!should_continue) {
			break;
		}
	}
	const mocha_object* result_list = mocha_values_create_list(context->values, temp, target_index);
	return result_list;
}
