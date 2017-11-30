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
