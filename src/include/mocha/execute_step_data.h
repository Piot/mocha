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
#ifndef mocha_execute_step_data_h
#define mocha_execute_step_data_h

#include <tyran/tyran_types.h>

struct mocha_context;
struct mocha_object;

typedef struct execute_step_result {
	tyran_boolean should_eval_fully;
	const struct mocha_object* object;
} execute_step_result;

typedef execute_step_result (*mocha_execute_step_fn)(void* user_data, const struct mocha_context* context, const struct mocha_object* o);

typedef struct mocha_execute_step {
	void* step_state;
	mocha_execute_step_fn fn;
	const char* debug_name;
} mocha_execute_step;

typedef struct mocha_execute_step_data {
	mocha_execute_step step;
	const struct mocha_object* object_to_resolve;
	tyran_boolean should_eval_fully;
} mocha_execute_step_data;

#endif
