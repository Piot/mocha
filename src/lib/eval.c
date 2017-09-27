#include <mocha/eval.h>
#include <mocha/context.h>
#include <mocha/error.h>
#include <mocha/extended_core.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/parser.h>
#include <mocha/print.h>
#include <stdlib.h>
#include <tyran/tyran_clib.h>

typedef struct eval_info {
	struct mocha_context* parent_context;
	size_t index;
	const struct mocha_list* list;
	resolve_callback resolve_info;
} eval_info;

static void eval_done(void* user_data, const struct mocha_object* result);

static void eval(const eval_info* self, const struct mocha_object* object, void* user_data, set_result_callback callback)
{
	const struct mocha_object* closure_object = mocha_values_create_closure(self->parent_context->values, self->parent_context, object);
	resolve_callback eval_callback;

	eval_callback.callback = callback;
	eval_callback.user_data = user_data;
	resolve_closure(&closure_object->data.closure, eval_callback);
}

static void eval_next(eval_info* self)
{
	const mocha_object* form = self->list->objects[self->index];

	//MOCHA_LOG("eval next:%d of %d %s", self->index, self->list->count, mocha_print_object_debug_str(form));
	eval(self, form, self, eval_done);
}

static void eval_done(void* user_data, const struct mocha_object* result)
{
	eval_info* self = (eval_info*) user_data;

	self->index++;

	if (self->index == self->list->count) {
		// MOCHA_LOG("Eval list done!");
		resolve_callback eval_callback = self->resolve_info;
		tyran_free(self);
		eval_callback.callback(eval_callback.user_data, result);
	} else {
		eval_next(self);
	}
}

void mocha_eval_string(mocha_context* parse_context, const char* s, void* user_data, set_result_callback callback)
{
	mocha_error error;

	mocha_error_init(&error);
	mocha_string string;
	mocha_string_init_from_c(&string, &parse_context->values->string_content_memory, s);

	// const mocha_context* parse_context = mocha_context_create(self->runtime.root_context);

	mocha_parser parser;
	mocha_parser_init(&parser, parse_context->values, parse_context, string.string, string.count);

	const mocha_object* o = mocha_parser_parse(&parser, &error);
	// self->runtime.root_context = self->parser.context;

	if (o && o->type == mocha_object_type_list) {
		const mocha_list* list = &o->data.list;

		if (list->count > 0) {
			resolve_callback resolve_info;
			resolve_info.callback = callback;
			resolve_info.user_data = user_data;
			eval_info* info = tyran_malloc(sizeof(eval_info));
			info->list = list;
			info->index = 0;
			info->resolve_info = resolve_info;
			info->parent_context = parse_context;
			eval_next(info);
		}
	}

	if (error.code != mocha_error_code_ok) {
		mocha_error_show(&error);
		mocha_error_init(&error);
	}
}
