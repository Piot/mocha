/*

MIT License

Copyright (c) 2013 Peter Bjorklund

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
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
#include <tiny-libc/tiny_libc.h>

const mocha_object* mocha_eval_string(mocha_context* parse_context, const char* s)
{
	mocha_error error;

	mocha_error_init(&error);
	mocha_string string;

	if (parse_context->values == 0) {
		MOCHA_SOFT_ERROR("Values can not be zero in context!");
		return 0;
	}
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
