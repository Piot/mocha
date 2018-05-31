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
