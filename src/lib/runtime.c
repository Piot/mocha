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
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/values.h>
#include <stdlib.h>

#include <mocha/context.h>

#include <imprint/memory.h>
#include <imprint/memory_pool.h>

void mocha_runtime_init_ex(mocha_runtime* self, const mocha_context* root_context, void* user_data)
{
	self->root_context = root_context;
	self->user_data = user_data;
	mocha_error_init(&self->error);
}

void mocha_runtime_init(mocha_runtime* self, struct mocha_values* values, void* user_data)
{
	if (values == 0) {
		MOCHA_ERROR("VALUES RUNTIME INIT");
	}

	mocha_context fake_context;
	fake_context.parent = 0;
	fake_context.values = values;
	fake_context.runtime = self;
	const mocha_context* root_context = mocha_object_context(mocha_values_create_context(values, &fake_context, "root"));
	mocha_context* mutable_context = ((mocha_context*) root_context);
	mutable_context->parent = 0;
	mutable_context->runtime = self;
	mocha_runtime_init_ex(self, root_context, user_data);
}

const mocha_object* mocha_runtime_eval(mocha_runtime* self, const mocha_object* o, mocha_error* error)
{
	(void) error;
	// mocha_runner_eval(mocha_runner *self, const struct mocha_context *context, const struct mocha_object *object)
	return mocha_runner_eval(self->root_context, o);
}

void mocha_runtime_debug(const mocha_runtime* self)
{
	MOCHA_OUTPUT("Runtime %p root_context:%p, runtime:%p, namespace_context:%p, create_context_parent:%p cached_values:%p", (void*) self, (void*) self->root_context, (void*) self->namespace_context, (void*) self->create_context_parent,
				 (void*) self->root_context->runtime, (void*) self->cached_values);
}
