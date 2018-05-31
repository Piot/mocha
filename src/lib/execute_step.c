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
#include <mocha/execute_step.h>
#include <mocha/log.h>
#include <mocha/print.h>

void mocha_execute_step_data_init(mocha_execute_step_data* self, mocha_execute_step_fn fn, void* step_state, const mocha_object* object_to_resolve, const char* debug_name)
{
	self->step.fn = fn;
	self->step.step_state = step_state;
	self->step.debug_name = debug_name;
	self->object_to_resolve = object_to_resolve;
	self->should_eval_fully = TYRAN_TRUE;
}

execute_step_result mocha_execute_step_exec(const mocha_execute_step* self, const mocha_context* context, const mocha_object* input)
{
	MOCHA_LOG("Execute step '%s' '%s'", self->debug_name, mocha_print_object_debug_str(input));
	execute_step_result result = self->fn(self->step_state, context, input);
	MOCHA_LOG("execute step '%s' returned %s", self->debug_name, mocha_print_object_debug_str(result.object));
	return result;
}
