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
