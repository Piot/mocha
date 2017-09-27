#include <tyran/tyran_clib.h>
#include <mocha/core_def.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const mocha_object* fn(const mocha_context* context, const mocha_object* name, const mocha_object* arguments, const mocha_object* body)
{
	const mocha_object* r = mocha_values_create_function(context->values, context, name, arguments, body);

	return r;
}

MOCHA_FUNCTION(fn_func)
{
	const mocha_object* default_name = mocha_values_create_symbol(context->values, "_fn");
	const mocha_object* r = fn(context, default_name, arguments->objects[1], arguments->objects[2]);
	MOCHA_RESULT_VALUE(result_callback, r);
}

MOCHA_FUNCTION(defmacro_func)
{
	const mocha_object* name = arguments->objects[1];
	const mocha_object* macro_arguments = arguments->objects[2];
	const mocha_object* body = arguments->objects[3];

	const mocha_object* macro = mocha_values_create_macro(context->values, context, name, macro_arguments, body);

	mocha_context_add_or_replace((mocha_context*) context, name, macro);

	// def(runtime, context, name, macro);
	// mocha_print_object_debug(result);

	MOCHA_RESULT_VALUE(result_callback, macro);
}
/*
   typedef struct defn_info {
		mocha_context* context;
		resolve_callback call_stack;
		const mocha_object* name;
   } def_info;

   static void defn_done(void* user_data, const mocha_object* result)
   {
		def_info* info = (def_info*) user_data;
		mocha_context_add_or_replace(info->context, info->name, result);
		MOCHA_RESULT_VALUE(info->result, result);
   }
 */

MOCHA_FUNCTION(defn_func)
{
	if (arguments->count < 4) {
		MOCHA_LOG("defn error");
		return;
	}
	const mocha_object* name = arguments->objects[1];
	size_t index = 2;
	if (mocha_object_is_string(arguments->objects[index])) {
		// Skip documentation
		index++;
	}
	const mocha_object* func = fn(context, name, arguments->objects[index], arguments->objects[index+1]);

	mocha_context_add_or_replace((mocha_context*) context, name, func);
	MOCHA_RESULT_VALUE(result_callback, func);
}

typedef struct def_info {
	mocha_context* context;
	resolve_callback resolve_info;
	const mocha_object* name;
} def_info;

static void def_done(void* user_data, const mocha_object* result)
{
	def_info* info = (def_info*) user_data;

	mocha_context_add_or_replace(info->context, info->name, result);
	resolve_callback callback = info->resolve_info;
	tyran_free(info);
	MOCHA_RESULT_VALUE(callback, result);
}

static void def(mocha_context* context, resolve_callback resolve_info, const mocha_object* name, const mocha_object* body)
{
	if (context == 0) {
		MOCHA_LOG("def is null!");
	}
	def_info* info = tyran_malloc(sizeof(def_info));

	info->resolve_info = resolve_info;
	info->context = context;
	info->name = name;

	resolve_closure_ex(context, body, info, def_done);
}

MOCHA_FUNCTION(def_func)
{
	def((mocha_context*) context, result_callback, arguments->objects[1], arguments->objects[2]);
}

typedef struct let_func_info {
	mocha_context* context;
	const mocha_object* body;

	const mocha_vector* assignment_vector;
	resolve_callback callback_info;
} let_func_info;

typedef struct let_func_argument_info {
	let_func_info* parent;
	size_t index;
} let_func_argument_info;

static void let_func_done(void* user_data, const struct mocha_object* result)
{
	// MOCHA_LOG("let_func_done %s", mocha_print_object_debug_str(result));
	let_func_info* self = (let_func_info*) user_data;

	self->callback_info.callback(self->callback_info.user_data, result);
	tyran_free(self);
}

static void let_func_execute_body(let_func_info* self)
{
	// MOCHA_LOG("let_func_execute_body");
	if (self->context == 0) {
		// MOCHA_LOG("let func context is null");
	}
	resolve_closure_ex(self->context, self->body, self, let_func_done);
}

static void resolve_next_argument(let_func_info* self, size_t index);

static void let_func_argument_done(void* user_data, const struct mocha_object* result)
{
	let_func_argument_info* self = (let_func_argument_info*) user_data;
	let_func_info* parent = self->parent;

	const mocha_object* symbol = parent->assignment_vector->objects[self->index * 2];

	if (symbol->type != mocha_object_type_symbol) {
		MOCHA_LOG("must have symbol in let");
	}

	if (parent->context == 0) {
		MOCHA_LOG("let_func_argument_done null");
	}

	// MOCHA_LOG("let_func_argument_done");
	mocha_context_add((mocha_context*) parent->context, symbol, result);
	size_t next_index = self->index + 1;
	tyran_free(self);
	if (next_index * 2 >= parent->assignment_vector->count) {
		let_func_execute_body(parent);
	} else {
		resolve_next_argument(parent, next_index);
	}
}

static void resolve_next_argument(let_func_info* self, size_t index)
{
	let_func_argument_info* info = tyran_malloc(sizeof(let_func_argument_info));

	info->index = index;
	info->parent = self;
	const mocha_object* to_resolve = self->assignment_vector->objects[index * 2 + 1];

	if (self->context == 0) {
		MOCHA_LOG("resolve_next_argument is null");
	}
	resolve_closure_ex(self->context, to_resolve, info, let_func_argument_done);
}

MOCHA_FUNCTION(let_func)
{
	const mocha_object* assignments = arguments->objects[1]; // mocha_runtime_eval(runtime, arguments->objects[1], &runtime->error);

	if (!assignments || assignments->type != mocha_object_type_vector) {
		MOCHA_LOG("must have vector in let!");
		return;
	}

	const mocha_vector* assignment_vector = &assignments->data.vector;

	if ((assignment_vector->count % 2) != 0) {
		MOCHA_LOG("Wrong number of assignments");
		return;
	}
	const mocha_object* body = arguments->objects[2];

	mocha_context* new_context = mocha_context_create(context, "core_def");
	let_func_info* info = tyran_malloc(sizeof(let_func_info));
	info->context = new_context;
	info->callback_info = result_callback;
	info->body = body;
	info->assignment_vector = assignment_vector;

	resolve_next_argument(info, 0);
}

void mocha_core_def_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(defn, mocha_false);
	MOCHA_DEF_FUNCTION(fn, mocha_false);
	MOCHA_DEF_FUNCTION(def, mocha_false);
	MOCHA_DEF_FUNCTION(defmacro, mocha_false);
	MOCHA_DEF_FUNCTION(let, mocha_false);
}
