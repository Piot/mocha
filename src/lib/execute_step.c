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
