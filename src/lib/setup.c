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
#include <mocha/core.h>
#include <mocha/log.h>
#include <mocha/setup.h>

void mocha_setup_init(mocha_setup* self, void* user_data)
{
	imprint_memory* memory = &self->memory;
	imprint_memory_init(memory, 256 * 1024 * 1024, "root-setup");

	mocha_values_config root_config;
	root_config.objects_count = 512 * 1024;
	root_config.object_reference_count = 1024 * 1024;
	root_config.string_memory_size = 512 * 1024;
	root_config.blob_memory_size = 32 * 1024 * 1024;

	mocha_hashed_strings_init(&self->hashed_strings);

	mocha_values_init(&self->values, &self->hashed_strings, memory, root_config, "root");
	mocha_runtime_init(&self->runtime, &self->values, user_data);

	mocha_context* root_context = (mocha_context*) self->runtime.root_context;
	if (root_context == 0) {
		MOCHA_LOG("Root context is null!");
	}
	root_context->values = &self->values;

	mocha_core_define_context(root_context, &self->values);
}
