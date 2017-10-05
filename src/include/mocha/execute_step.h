#ifndef mocha_execute_step_h
#define mocha_execute_step_h

#include <mocha/execute_step_data.h>
#include <mocha/object.h>

struct mocha_context;

void mocha_execute_step_data_init(mocha_execute_step_data* self, mocha_execute_step_fn fn, void* step_state, const mocha_object* object_to_resolve, const char* debug_name);
execute_step_result mocha_execute_step_exec(const mocha_execute_step* self, const struct mocha_context* context, const mocha_object* input);

#endif
