#ifndef mocha_runtime_h
#define mocha_runtime_h

#include <mocha/context.h>
#include <mocha/error.h>
#include <mocha/eval_stack.h>
#include <mocha/type.h>

struct mocha_object;
struct mocha_error;
struct mocha_values;
struct tyran_memory;
struct tyran_memory_pool;
struct mocha_eval_stack;

typedef struct mocha_runtime {
	const struct mocha_context* root_context;
	struct mocha_values* values;
	struct mocha_error error;
	struct mocha_eval_stack eval_stack;
	void* user_data;
} mocha_runtime;

#define MOCHA_RESULT_VALUE(__resolve_callback, __result) __resolve_callback.callback(__resolve_callback.user_data, __result);

void mocha_runtime_init(mocha_runtime* self, struct mocha_values* values, struct tyran_memory* memory);

const mocha_object* mocha_runtime_invoke(mocha_runtime* self, mocha_context* context, const mocha_object* fn, const mocha_list* arguments_list);

void mocha_runtime_eval(const mocha_runtime* runtime, const struct mocha_object* o, set_result_callback callback, void* user_data);
mocha_boolean mocha_runtime_execute_next(mocha_runtime* runtime);

#endif
