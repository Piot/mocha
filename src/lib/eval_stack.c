#include <mocha/context.h>
#include <mocha/eval_stack.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <tyran/tyran_memory_pool.h>
#include <stdlib.h>

static void stack_item_init(mocha_eval_stack_item* self)
{
}

void mocha_eval_stack_init(mocha_eval_stack* self, struct tyran_memory* memory, size_t max_size, struct mocha_runtime* runtime)
{
	self->runtime = runtime;
	self->items = TYRAN_MEMORY_POOL_CONSTRUCT(memory, mocha_eval_stack_item, max_size);
}
/*
static void debug_item(const char* debug_string, mocha_eval_stack_item* self)
{
	//	mocha_boolean has_callback = (self->result_callback != 0);

	// MOCHA_LOG("%s context:%p callback:%d    %s", debug_string, (void*) self->context, has_callback, mocha_print_list_debug_str(self->source_form_for_debug));

	// mocha_context_print_debug("CTX ", self->context, mocha_false);
}

static mocha_boolean all_is_evaluating(mocha_eval_stack* self)
{
	for (size_t i = 0; i < self->items->max_count; ++i) {
		const tyran_memory_pool_entry* e = &self->items->entries[i];
		if (e->allocated) {
			mocha_eval_stack_item* item = (mocha_eval_stack_item*) tyran_memory_pool_pointer(self->items, i);
			if (!item->is_evaluating) {
				return mocha_false;
			}
		}
	}

	return mocha_true;
}
*/

mocha_boolean mocha_eval_stack_is_empty(mocha_eval_stack* self)
{
	return self->items->count == 0;
}

mocha_eval_stack_item* mocha_eval_stack_push(mocha_eval_stack* self, const mocha_context* context, const struct mocha_object* form, void* user_data, set_result_callback callback)
{
	mocha_eval_stack_item* item = TYRAN_CALLOC_TYPE(self->items, mocha_eval_stack_item);
	//MOCHA_LOG("Push on stack: %p '%s' %d", (void*) self, mocha_print_object_debug_str(form), self->items->count);
	stack_item_init(item);
	mocha_closure* closure = (mocha_closure*) &item->closure;
	item->is_evaluating = mocha_false;
	closure->object = form;
	closure->context = context;
	item->eval_stack = self;
	resolve_callback* callback_info = &item->callback_info;
	callback_info->callback = callback;
	callback_info->user_data = user_data;
	return item;
}

mocha_eval_stack_item* mocha_eval_stack_next(mocha_eval_stack* self)
{
	//MOCHA_LOG("checking for something in stack:%p", (void*)self);
	for (size_t i = 0; i < self->items->max_count; ++i) {
		const tyran_memory_pool_entry* e = &self->items->entries[i];
		if (e->allocated) {
			mocha_eval_stack_item* item = (mocha_eval_stack_item*) tyran_memory_pool_pointer(self->items, i);
			if (!item->is_evaluating) {
				// MOCHA_LOG("returning item %p %s", (void*)self, mocha_print_object_debug_str(item->closure.object));
				item->is_evaluating = mocha_true;
				return item;
			}
		}
	}
	// MOCHA_LOG("Couldn't find andything in %p", (void*)self);
	return 0;
}

void mocha_eval_stack_delete(mocha_eval_stack* self, mocha_eval_stack_item* item)
{
	// MOCHA_LOG("Delete from stack: %p '%s'", (void*) self, mocha_print_object_debug_str(item->closure.object));
	TYRAN_ALLOC_FREE(self->items, item);
}

void mocha_eval_stack_debug(const mocha_eval_stack_item* self, const char* debug_string)
{
	MOCHA_LOG("Evalstack ------ %s ---", debug_string);
	MOCHA_LOG("%s", "------------- Evalstack");
}
