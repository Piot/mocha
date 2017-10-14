#include <mocha/context.h>
#include <mocha/error.h>
#include <mocha/eval.h>
#include <mocha/extended_core.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/parser.h>
#include <mocha/print.h>
#include <mocha/runner.h>
#include <stdlib.h>
#include <tyran/tyran_clib.h>

const mocha_object* mocha_eval_string(mocha_context* parse_context, const char* s, void* user_data)
{
	(void) user_data;
	mocha_error error;

	mocha_error_init(&error);
	mocha_string string;
	mocha_string_init_from_c(&string, &parse_context->values->string_content_memory, s);

	// const mocha_context* parse_context = mocha_context_create(self->runtime.root_context);

	mocha_parser parser;
	mocha_parser_init(&parser, parse_context->values, parse_context, string.string, string.count);
	const mocha_object* o = mocha_parser_parse(&parser, &error);
	// self->runtime.root_context = self->parser.context;
	return mocha_runner_eval(parse_context, o);
}
