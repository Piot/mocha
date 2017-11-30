#include <mocha/core.h>
#include <mocha/log.h>
#include <mocha/setup.h>

void mocha_setup_init(mocha_setup* self, void* user_data)
{
	tyran_memory* memory = &self->memory;
	tyran_memory_init(memory, 100 * 1024 * 1024, "root-setup");

	mocha_values_config root_config;
	root_config.objects_count = 256 * 1024;
	root_config.object_reference_count = 256 * 1024;
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
