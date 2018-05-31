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
#ifndef mocha_runtime_h
#define mocha_runtime_h

#include <mocha/context.h>
#include <mocha/error.h>
#include <mocha/runner.h>
#include <mocha/type.h>

struct mocha_object;
struct mocha_error;
struct mocha_values;
struct imprint_memory;
struct imprint_memory_pool;
struct mocha_context;

typedef struct mocha_runtime {
	const struct mocha_context* root_context;
	const struct mocha_context* create_context_parent;
	struct mocha_error error;
	void* user_data;
	const struct mocha_context* namespace_context;
	struct mocha_values* cached_values;
} mocha_runtime;

#define MOCHA_RESULT_VALUE(__resolve_callback, __result) __resolve_callback.callback(__resolve_callback.user_data, __result);

void mocha_runtime_init(mocha_runtime* self, struct mocha_values* values, void* user_data);
void mocha_runtime_init_ex(mocha_runtime* self, const struct mocha_context* root_context, void* user_data);

const struct mocha_object* mocha_runtime_eval(mocha_runtime* self, const struct mocha_object* o, mocha_error* error);

#endif
