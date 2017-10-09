#include <mocha/execute.h>
#include <mocha/execute_step.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runner.h>
#include <mocha/runtime.h>
#include <mocha/values.h>
#include <stdlib.h>
#include <tyran/tyran_clib.h>
#include <unistd.h>

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

typedef struct execute_form_info {
	const mocha_list* arguments;
} execute_form_info;

static const mocha_object* execute_form(void* user_data, const struct mocha_context* context, const struct mocha_object* fn_object)
{
	execute_form_info* self = (execute_form_info*) user_data;

	if (!mocha_object_is_invokable(fn_object)) {
		MOCHA_LOG("ERROR: Object resolved is not an invokable!");
		return 0;
	}

	return execute(context, fn_object, self->arguments);
}

static const mocha_object* resolve_execute_list(const mocha_context* context, const mocha_list* list)
{
	execute_form_info* info = tyran_malloc(sizeof(execute_form_info));
	info->arguments = list;

	const mocha_object* next_step = mocha_values_create_execute_step_data(context->values, execute_form, info, list->objects[0], "execute_form");

	return next_step;
}

typedef struct result_info {
	const mocha_object* form;
	const mocha_context* context;
	mocha_boolean should_eval_fully;
} result_info;

static result_info internal_eval(mocha_runner* self, const mocha_context* context, const mocha_object* form, int should_eval_fully)
{
	result_info r_info;

	if (!context) {
		MOCHA_ERROR("symbol lookup: Context is null");
	}

	if (!form) {
		MOCHA_ERROR("ERROR! form is null!");
	}

	const mocha_object* result = form;

	r_info.form = form;
	r_info.should_eval_fully = should_eval_fully;
	/*	if (should_eval_fully && mocha_object_is_symbol(result))
	{
		result = mocha_context_lookup(context, result);
		MOCHA_LOG(" resolved '%s'", mocha_print_object_debug_str(result));
	}
*/
	if (mocha_object_is_list(form)) {
		if (should_eval_fully) {
			result = resolve_execute_list(context, mocha_object_list(form));
		} else {
			result = form;
		}
	} else if (mocha_object_is_execute_step_data(form)) {
		const mocha_execute_step_data* step_data = mocha_object_execute_step_data(form);
		self->steps[self->steps_count++] = &step_data->step;
		result = step_data->object_to_resolve;
		r_info.should_eval_fully = 1;
	} else if (mocha_object_is_map(form)) {
		// TODO: Eval-sequence
		MOCHA_ERROR("map not implemented yet");
	} else if (mocha_object_is_vector(form)) {
		// TODO: Eval-sequence
		MOCHA_ERROR("vector not implemented yet");
	} else if (mocha_object_is_symbol(form)) {
		const mocha_object* resolved = mocha_context_lookup(context, form);
		MOCHA_LOG(" resolved '%s'", mocha_print_object_debug_str(resolved));
		result = resolved;
	} else if (mocha_object_is_primitive(form)) {
		result = form;
	} else if (mocha_object_is_closure(form)) {
		if (should_eval_fully) {
			const mocha_closure* closure = mocha_object_closure(form);
			result = closure->object;
			context = closure->context;
			should_eval_fully = r_info.should_eval_fully;
		} else {
			MOCHA_LOG("Should not eval closure");
			result = form;
		}
	}

	if (should_eval_fully && mocha_object_is_symbol(result)) {
		result = mocha_context_lookup(context, result);
		MOCHA_LOG(" resolved '%s'", mocha_print_object_debug_str(result));
	}

	r_info.form = result;
	r_info.context = context;

	return r_info;
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

/*
#if defined MOCHA_RESOLVER_ENABLE_PERFORMANCE
	if (g_performance_stats_enabled) {
		sort_stats();
		MOCHA_LOG("PERFORMANCE STATS----------------");
#define MAX_SHOW_STATS (15)
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
*/

const mocha_object* mocha_runner_eval(mocha_runner* self, const struct mocha_context* context, const struct mocha_object* object)
{
	const mocha_object* next_eval = object;
	int should_eval_fully = 1;
	result_info result;
	while (1) {
		MOCHA_LOG("--- Before context:%s eval:%s fully:%d", mocha_context_print_debug_short(context), mocha_print_object_debug_str(next_eval), should_eval_fully);
		result = internal_eval(self, context, next_eval, should_eval_fully);
		next_eval = result.form;
		context = result.context;
		// should_eval_fully = result.should_eval_fully;
		MOCHA_LOG("eval returned:'%s'", mocha_print_object_debug_str(next_eval));
		if (mocha_object_is_primitive(next_eval)) {
			if (self->steps_count == 0) {

				MOCHA_LOG("Nothing pushed. Returning '%s'", mocha_print_object_debug_str(next_eval));
				return next_eval;
			} else {
				const mocha_execute_step* step = self->steps[--self->steps_count];
				const mocha_object* next_step_object = mocha_execute_step_exec(step, context, next_eval);
				next_eval = next_step_object;
				should_eval_fully = 1;
			}
		}
	}

	// return next_eval;
}

void mocha_runner_init(mocha_runner* self)
{
	(void) self;
}
