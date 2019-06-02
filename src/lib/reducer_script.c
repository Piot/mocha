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

const struct mocha_object* mocha_reducer_reduce_script(const struct mocha_context* context, const struct mocha_list* arguments, const char* debug)
{
	(void) debug;
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
