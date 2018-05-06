#include <mocha/context.h>
#include <mocha/error.h>
#include <mocha/eval.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/parser.h>
#include <mocha/print.h>
#include <mocha/runner.h>
#include <mocha/runtime.h>
#include <stdlib.h>
#include <tiny_libc/tiny_libc.h>

const mocha_object* mocha_eval_string(mocha_context* parse_context, const char* s)
{
	mocha_error error;

	mocha_error_init(&error);
	mocha_string string;
	mocha_string_init_from_c(&string, &parse_context->values->string_content_memory, s);

	mocha_parser parser;
	mocha_parser_init(&parser, parse_context->values, parse_context, string.string, string.count);
	parse_context->runtime->namespace_context = parse_context;
	parse_context->runtime->create_context_parent = parse_context;
	const mocha_object* o = mocha_parser_parse(&parser, &error);
	const mocha_object* result = 0;
	const mocha_runtime* runtime = parse_context->runtime;
	if (o && o->type == mocha_object_type_list) {
		const mocha_list* list = &o->data.list;
		for (size_t i = 0; i < list->count; ++i) {
			result = mocha_runner_eval(runtime->namespace_context, list->objects[i]);
		}
	} else {
	}
	return result;
}
