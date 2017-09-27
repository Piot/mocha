#ifndef mocha_eval_stack_h
#define mocha_eval_stack_h

#include <mocha/closure.h>
#include <mocha/list.h>
#include <mocha/resolver.h>
#include <mocha/types.h>

struct mocha_object;
struct mocha_runtime;
struct tyran_memory;
struct tyran_memory_pool;

typedef struct mocha_eval_stack_item {
	const struct mocha_closure closure;
	mocha_boolean is_special;
	struct mocha_eval_stack_item* parent;
	resolve_callback callback_info;
	struct mocha_eval_stack* eval_stack;
	mocha_boolean is_evaluating;
	float started_evaluating_at_time;
} mocha_eval_stack_item;

typedef struct mocha_eval_stack {
	struct tyran_memory_pool* items;
	struct mocha_runtime* runtime;
} mocha_eval_stack;

void mocha_eval_stack_init(mocha_eval_stack* self, struct tyran_memory* memory, size_t max_size, struct mocha_runtime* runtime);
mocha_eval_stack_item* mocha_eval_stack_push(mocha_eval_stack* self, const struct mocha_context* context, const struct mocha_object* form, void* user_data, set_result_callback callback);
mocha_eval_stack_item* mocha_eval_stack_next(mocha_eval_stack* self);
mocha_boolean mocha_eval_stack_is_empty(mocha_eval_stack* self);
void mocha_eval_stack_delete(mocha_eval_stack* self, mocha_eval_stack_item* node);
void mocha_eval_stack_debug(const mocha_eval_stack_item* self, const char* debug_string);

#endif
