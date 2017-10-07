#include <mocha/transduce.h>
#include <mocha/execute.h>
#include <mocha/object.h>

const mocha_object *mocha_transduce(const mocha_context *context, const mocha_object *fn, void *user_data, mocha_transducer_adder adder, const mocha_object *state, const mocha_object *item)
{
	(void) user_data;
	(void) adder;
	mocha_list arguments;
	const mocha_object *args[3];
	args[0] = 0;
	args[1] = state;
	args[2] = item;
	arguments.objects = args;
	arguments.count = 3;
	const mocha_object *new_item = execute(context, fn, &arguments);

	// adder(user_data, new_item);

	const mocha_object *new_result = 0;
	return new_result;
}

void mocha_transducer_init_internal(mocha_transducer *self, mocha_c_fn fn, const char *debug_name)
{
	self->c_fn = fn;
	self->debug_name = debug_name;
}

void mocha_transducer_init_script(mocha_transducer *self, const mocha_function *fn)
{
	self->fn = fn;
}
