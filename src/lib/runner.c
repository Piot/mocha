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

// #define MOCHA_RUNNER_DEBUG_OUTPUT

static const mocha_object* eval(const struct mocha_context* context, const struct mocha_object* form);

typedef const mocha_object* (*sequence_create_collection)(mocha_values* values, const struct mocha_object* o[], size_t count);

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

static const mocha_object* eval_execute_list(const mocha_context* context, const mocha_list* arguments)
{
	const mocha_object* invokable_object = eval(context, arguments->objects[0]);
	const mocha_object* result = execute(context, invokable_object, arguments);

	return result;
}

static const mocha_object* eval_sequence(const mocha_context* context, const mocha_sequence* seq, sequence_create_collection fn)
{
	const mocha_object** objects;
	size_t count;
	mocha_sequence_get_objects(seq, &objects, &count);

	mocha_list resolved_list;
	mocha_list_init_prepare(&resolved_list, &context->values->object_references, count);

	for (size_t i = 0; i < count; ++i) {
		const mocha_object* evaled = eval(context, objects[i]);
		resolved_list.objects[i] = evaled;
	}
	const mocha_object* result = fn(context->values, resolved_list.objects, count);

	return result;
}

int g_depth;

const char* tabs_helper(size_t count)
{
	static char temp[1024];

	for (size_t i = 0; i < count; ++i) {
		temp[i] = '.';
	}

	temp[count] = 0;
	return temp;
}

const char* tabs()
{
	return tabs_helper(g_depth);
}

static const mocha_object* internal_eval(const mocha_context* context, const mocha_object* form)
{
	g_depth++;
	if (g_depth > 32) {
		MOCHA_ERROR("Something is very wrong. Out of stack space");
	}
	if (!context) {
		MOCHA_ERROR("symbol lookup: Context is null");
	}

	if (!form) {
		MOCHA_ERROR("ERROR! form is null!");
	}

	const mocha_object* result;

redo:

#if defined MOCHA_RUNNER_DEBUG_OUTPUT
	MOCHA_LOG("%s eval %d %s", tabs(), g_depth, mocha_print_object_debug_str(form));
#endif
	switch (form->type) {
		case mocha_object_type_list: {
			const mocha_list* arguments = mocha_object_list(form);
			const mocha_object* object_fn = eval(context, arguments->objects[0]);
			const mocha_c_fn c_fn = object_fn->object_type ? object_fn->object_type->invoke : 0;
			if (c_fn != 0) {
#if defined MOCHA_RUNNER_DEBUG_OUTPUT
				MOCHA_LOG("%s CFN! %s", tabs(), mocha_print_object_debug_str(object_fn));
#endif
				result = c_fn(context, arguments);
				if (result->type == mocha_object_type_recur) {
					const mocha_recur* recur = &result->data.recur;
					const mocha_list* list = mocha_object_list(recur->arguments);
					const mocha_context* new_context = mocha_context_create_invoke_context(context, context->script_fn, list);
					form = context->script_fn->code;
					context = new_context;
					goto redo;
				} else if (result->type == mocha_object_type_eval) {
					form = result->data.closure.object;
					goto redo;
				}
			} else if (mocha_object_is_function(object_fn)) {
				const mocha_function* script_fn = mocha_object_function(object_fn);
				const mocha_context* new_context = mocha_context_create_invoke_context(context, script_fn, arguments);
				context = new_context;
				form = script_fn->code;
				goto redo;
				/*

				}*/
			} else {
				MOCHA_ERROR("RERRR");
			}
		} break;
		case mocha_object_type_map:
			result = eval_sequence(context, mocha_object_sequence(form), mocha_values_create_map);
			break;
		case mocha_object_type_vector:
			result = eval_sequence(context, mocha_object_sequence(form), mocha_values_create_vector);
			break;
		case mocha_object_type_symbol:
			result = mocha_context_lookup(context, form);
#if defined MOCHA_RUNNER_DEBUG_OUTPUT

			MOCHA_LOG("%s symbol lookup! %s", tabs(), mocha_print_object_debug_str(form));
#endif
			break;
		default:
			result = form;
	}

#if defined MOCHA_RUNNER_DEBUG_OUTPUT
	MOCHA_LOG("%s Leaving %d", tabs(), g_depth);
#endif
	g_depth--;
	return result;
}

static const mocha_object* eval(const mocha_context* context, const mocha_object* form)
{
	return internal_eval(context, form);
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

const mocha_object* mocha_runner_eval(const struct mocha_context* context, const struct mocha_object* object)
{
	return internal_eval(context, object);
}

const struct mocha_object* mocha_runner_eval_arguments(const struct mocha_context* context, const struct mocha_list* arguments)
{
	size_t rest_count = arguments->count;
	mocha_list resolved_list;
	mocha_list_init_prepare(&resolved_list, &context->values->object_references, rest_count);

	resolved_list.objects[0] = arguments->objects[0];

	for (size_t i = 1; i < rest_count; ++i) {
		const mocha_object* o = arguments->objects[i];
		resolved_list.objects[i] = mocha_runner_eval(context, o);
	}

	return mocha_values_create_list(context->values, resolved_list.objects, resolved_list.count);
}

const struct mocha_object* mocha_runner_eval_arguments_rest(const struct mocha_context* context, const struct mocha_list* arguments)
{
	size_t rest_count = arguments->count - 1;
	mocha_list resolved_list;
	mocha_list_init_prepare(&resolved_list, &context->values->object_references, rest_count);

	for (size_t i = 0; i < rest_count; ++i) {
		const mocha_object* o = arguments->objects[i + 1];
		resolved_list.objects[i] = mocha_runner_eval(context, o);
	}

	return mocha_values_create_list(context->values, resolved_list.objects, resolved_list.count);
}
