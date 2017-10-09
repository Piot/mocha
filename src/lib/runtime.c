#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/values.h>
#include <stdlib.h>

#include <mocha/context.h>

#include <tyran/tyran_memory.h>
#include <tyran/tyran_memory_pool.h>

void mocha_runtime_init(mocha_runtime* self, struct mocha_values* values, struct tyran_memory* memory)
{
	(void) memory;
	self->values = values;

	mocha_context fake_context;
	fake_context.parent = 0;
	fake_context.values = values;
	self->root_context = mocha_object_context(mocha_values_create_context(values, &fake_context, "root"));
	((mocha_context*) self->root_context)->parent = 0;
	if (values == 0) {
		MOCHA_LOG("ERRROR VALUES RUNTIME INIT");
	}
	mocha_error_init(&self->error);
}

const mocha_object* mocha_runtime_eval(mocha_runtime* self, const mocha_object* o, mocha_error* error)
{
	(void) error;
	// mocha_runner_eval(mocha_runner *self, const struct mocha_context *context, const struct mocha_object *object)
	return mocha_runner_eval(&self->runner, self->root_context, o);
}
