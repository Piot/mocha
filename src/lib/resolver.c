#include <mocha/execute_list.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/resolve_arguments.h>
#include <mocha/resolve_map.h>
#include <mocha/resolver.h>
#include <mocha/runtime.h>
#include <mocha/values.h>

#include <stdlib.h>
#include <unistd.h>
#include <tyran/tyran_clib.h>

typedef struct resolve_map_info {
	const mocha_context* context;
	resolve_callback callback_info;
} resolve_map_info;

// #define MOCHA_RESOLVER_ENABLE_PERFORMANCE

#if defined MOCHA_RESOLVER_ENABLE_PERFORMANCE

typedef struct performance_stats {
	float ms;
	const mocha_object* form;
} performance_stats;
#define MAX_STATS 600
performance_stats stats[MAX_STATS];
int stats_index = 0;
#endif

extern mocha_boolean g_performance_stats_enabled;
mocha_boolean g_performance_stats_enabled;

static void eval_item_done(mocha_eval_stack* stack, mocha_eval_stack_item* item, const mocha_object* result)
{
#if defined MOCHA_RESOLVER_ENABLE_PERFORMANCE
	float now = NIMBUS_PERFORMANCE_TIMESTAMP();
	float time_to_evaluate = now - item->started_evaluating_at_time;
	if (time_to_evaluate > 1.0) {
		if (stats_index < MAX_STATS) {
			performance_stats* stat = &stats[stats_index++];
			stat->ms = time_to_evaluate;
			stat->form = item->closure.object;
		}
	}
#endif

	resolve_callback callback_info = item->callback_info;
	mocha_eval_stack_delete(stack, item);
	callback_info.callback(callback_info.user_data, result);
}

static void eval_stack_item_done(mocha_eval_stack_item* item, const mocha_object* result)
{
	eval_item_done(item->eval_stack, item, result);
}

/*
static void c_execute_done(void* user_data, const struct mocha_object* result)
{
	c_execute_info* self = (c_execute_info*) user_data;
	tyran_free(self);
	eval_item_done(self->entry->eval_stack, self->entry, result);
}
*/

static void resolve_sequence_done(void* user_data, const mocha_object* seq)
{
	mocha_eval_stack_item* item = (mocha_eval_stack_item*) user_data;
	eval_stack_item_done(item, seq);
}

static void resolve_sequence(const mocha_context* context, mocha_object_type target_type, const mocha_object* sequence_object, mocha_eval_stack_item* item)
{
	resolve_callback callback;
	callback.user_data = item;
	callback.callback = resolve_sequence_done;

	resolve_sequence_callback(context, mocha_object_sequence(sequence_object), target_type, callback.user_data, callback.callback);
}

static void resolve_execute_list_done(void* user_data, const mocha_object* result)
{
	mocha_eval_stack_item* item = (mocha_eval_stack_item*) user_data;
	eval_stack_item_done(item, result);
}

static void resolve_execute_list(const mocha_context* context, const mocha_list* list, mocha_eval_stack_item* item)
{
	resolve_callback callback;
	callback.user_data = item;
	callback.callback = resolve_execute_list_done;

	execute_list(context, list, callback);
}

static void internal_resolve(mocha_runtime* runtime, mocha_eval_stack_item* entry)
{
	const mocha_object* form = entry->closure.object;
	const mocha_context* context = entry->closure.context;
	if (context == 0) {
		MOCHA_LOG("symbol lookup: Context is null");
	}

	if (!form) {
		MOCHA_LOG("ERROR! form is null!");
		return;
	}
/*
	static int g_strange_counter;
	g_strange_counter++;
	if ((g_strange_counter % 10) == 0) {
		MOCHA_LOG(" exec '%s'", mocha_print_object_debug_str(form));
	}
*/
	if (mocha_object_is_list(form)) {
		resolve_execute_list(context, mocha_object_list(form), entry);
	} else if (mocha_object_is_map(form)) {
		resolve_sequence(context, mocha_object_type_map, form, entry);
	} else if (mocha_object_is_vector(form)) {
		resolve_sequence(context, mocha_object_type_vector, form, entry);
	} else if (mocha_object_is_symbol(form)) {
		const mocha_object* resolved = mocha_context_lookup(context, form);
		// MOCHA_LOG(" resolved '%s'", mocha_print_object_debug_str(resolved));
		
		eval_stack_item_done(entry, resolved);
	} else if (mocha_object_is_primitive(form)) {
		eval_stack_item_done(entry, form);
	}
}

#if defined MOCHA_RESOLVER_ENABLE_PERFORMANCE
static int compare_time(const void* a, const void* b)
{
	const performance_stats* sa = (performance_stats*) a;
	const performance_stats* sb = (performance_stats*) b;

	return sb->ms > sa->ms ? 1 : (sb->ms == sa->ms) ? 0 : -1;
}

static void sort_stats()
{
	qsort(stats, stats_index, sizeof(performance_stats), compare_time);
}
#endif

mocha_boolean mocha_runtime_execute_next(mocha_runtime* runtime)
{
	size_t iteration_count = 0;
	mocha_eval_stack_item* last_item = 0;
	while (!mocha_eval_stack_is_empty(&runtime->eval_stack)) {
		mocha_eval_stack_item* item = mocha_eval_stack_next(&runtime->eval_stack);
		if (item) {
			iteration_count++;
#if defined MOCHA_RESOLVER_ENABLE_PERFORMANCE
			item->started_evaluating_at_time = NIMBUS_PERFORMANCE_TIMESTAMP();
#endif
			internal_resolve(runtime, item);
			last_item = item;
		} else {
			if (last_item != 0) {
				// MOCHA_LOG("Last item:%s", mocha_print_object_debug_str(last_item->closure.object));
			}
			if (iteration_count != 0) {
				MOCHA_LOG("early iterations:%zu", iteration_count);
			}
			return mocha_false;
		}
	}

	if (iteration_count != 0) {
		if (last_item != 0) {
			// MOCHA_LOG("Last item:%s", mocha_print_object_debug_str(last_item->closure.object));
		}
		// MOCHA_LOG("done iterations:%d", iteration_count);
	}

#if defined MOCHA_RESOLVER_ENABLE_PERFORMANCE
	if (g_performance_stats_enabled) {
		sort_stats();
		MOCHA_LOG("PERFORMANCE STATS----------------");
		#define MAX_SHOW_STATS (15)
		int show_count = stats_index > MAX_SHOW_STATS ? MAX_SHOW_STATS : stats_index;
		for (int i = 0; i < show_count; ++i) {
			MOCHA_LOG("%.1f ms %s", stats[i].ms, mocha_print_object_debug_str(stats[i].form));
		}
		g_performance_stats_enabled = 0;
		MOCHA_LOG("--------------------------------");
	}
	stats_index = 0;
#endif

	return mocha_true;
}

void resolve_closure(const mocha_closure* self, resolve_callback callback_info)
{
	if (self->context == 0) {
		MOCHA_LOG("resolve_closure null!");
	}
	mocha_runtime* runtime = self->context->values->runtime;

	mocha_eval_stack_push(&runtime->eval_stack, self->context, self->object, callback_info.user_data, callback_info.callback);
}

void resolve_closure_ex(const struct mocha_context* context, const struct mocha_object* object, void* user_data, set_result_callback callback)
{
	mocha_closure temp_closure;

	mocha_closure_init(&temp_closure, context, object);
	resolve_callback callback_info;
	callback_info.callback = callback;
	callback_info.user_data = user_data;
	resolve_closure(&temp_closure, callback_info);
}
