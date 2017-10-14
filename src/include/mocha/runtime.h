#ifndef mocha_runtime_h
#define mocha_runtime_h

#include <mocha/context.h>
#include <mocha/error.h>
#include <mocha/runner.h>
#include <mocha/type.h>

struct mocha_object;
struct mocha_error;
struct mocha_values;
struct tyran_memory;
struct tyran_memory_pool;

typedef struct mocha_runtime {
	const struct mocha_context* root_context;
	struct mocha_values* values;
	struct mocha_error error;
	void* user_data;
} mocha_runtime;

#define MOCHA_RESULT_VALUE(__resolve_callback, __result) __resolve_callback.callback(__resolve_callback.user_data, __result);

void mocha_runtime_init(mocha_runtime* self, struct mocha_values* values, struct tyran_memory* memory);

const struct mocha_object* mocha_runtime_eval(mocha_runtime* self, const struct mocha_object* o, mocha_error* error);

#endif
