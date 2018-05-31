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
#include <mocha/execute.h>
#include <mocha/function.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runner.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/values.h>
#include <tiny-libc/tiny_libc.h>

static const mocha_object* c_execute(const mocha_context* context, const mocha_c_fn c_fn, const mocha_list* arguments_list)
{
	return c_fn(context, arguments_list);
}

static const mocha_context* create_invoke_context(const mocha_context* context, const mocha_function* fn, const mocha_list* arguments_list)
{
	if (!fn) {
		MOCHA_ERROR("No fn!");
	}
	if (!fn->arguments) {
		MOCHA_ERROR("NO FN arguments!");
	}
	const mocha_list* args = &fn->arguments->data.list;
	mocha_context* new_context = (mocha_context*) mocha_object_context(mocha_values_create_context(context->values, fn->context, fn->debug_name)); //, "fn->context");
	new_context->script_fn = fn;

	// mocha_context_print_debug("function context:", new_context);
	size_t required_number_of_arguments = args->count;

	// mocha_boolean var_args = mocha_false;

	for (size_t arg_count = 0; arg_count < args->count; ++arg_count) {
		const mocha_object* arg = args->objects[arg_count];

		if (arg->type != mocha_object_type_symbol) {
			MOCHA_SOFT_ERROR("It is not a symbol what is up with that!? Illegal argument:%s", mocha_print_object_debug_str(arg));
			return 0;
		}

		const mocha_object* value = arguments_list->objects[1 + arg_count];
		mocha_context_add(new_context, arg, value);
		//}
	}

	if (arguments_list->count - 1 != required_number_of_arguments) {
		MOCHA_SOFT_ERROR("Illegal number of arguments: %lu (expected %d '%s'", arguments_list->count, (int) required_number_of_arguments, mocha_print_object_debug_str(fn->arguments));
		return 0;
	}
	// mocha_runtime_push_context(self, new_context);
	return new_context;
}

const mocha_context* mocha_context_create_invoke_context(const mocha_context* context, const mocha_function* fn, const mocha_list* arguments_list)
{
	return create_invoke_context(context, fn, arguments_list);
}

const mocha_object* script_execute(const mocha_context* context, const mocha_function* script_fn, const mocha_list* arguments_list)
{
	if (context == 0) {
		MOCHA_SOFT_ERROR("Script_Execute is null");
	}
	const mocha_context* fn_context = create_invoke_context(context, script_fn, arguments_list);

	if (fn_context == 0) {
		MOCHA_SOFT_ERROR("Script-execute fn-context is null");
	}

	if (fn_context->parent == 0) {
		MOCHA_SOFT_ERROR("Script-execute fn-parent is null");
	}

	// node->is_special = script_fn->is_special;
	const mocha_object* r = mocha_runner_eval(fn_context, script_fn->code);

	return r;
}

const mocha_object* execute(const mocha_context* context, const mocha_object* object_fn, const mocha_list* arguments)
{
	// o = find_cache(self, l);
	// const mocha_object* o = 0;
	// if (o) { push_result(o); }
	// if (!o) {
	if (!mocha_object_is_invokable(object_fn)) {
		MOCHA_SOFT_ERROR("This object isn't invokable. Missing eval? '%s'", mocha_print_object_debug_str(object_fn));
		return 0;
	}

	const mocha_c_fn c_fn = object_fn->object_type ? object_fn->object_type->invoke : 0;
	const mocha_object* result_object = 0;

	if (c_fn != 0) {
		result_object = c_execute(context, c_fn, arguments);
	} else if (mocha_object_is_function(object_fn)) {
		const mocha_function* fn = mocha_object_function(object_fn);
		result_object = script_execute(context, fn, arguments);
	} else {
		CLOG_SOFT_ERROR("Invoke failed");
	}
	return result_object;
}

const mocha_object* execute_0(const mocha_context* context, const mocha_object* object_fn)
{
	const mocha_object* objects[1];

	objects[0] = object_fn;

	mocha_list arguments;
	arguments.objects = objects;
	arguments.count = 1;

	return execute(context, object_fn, &arguments);
}

const mocha_object* execute_1(const mocha_context* context, const mocha_object* object_fn, const mocha_object* a)
{
	const mocha_object* objects[2];

	objects[0] = object_fn;
	objects[1] = a;

	mocha_list arguments;
	arguments.objects = objects;
	arguments.count = 2;

	return execute(context, object_fn, &arguments);
}

const mocha_object* execute_2(const mocha_context* context, const mocha_object* object_fn, const mocha_object* a, const mocha_object* b)
{
	const mocha_object* objects[3];

	objects[0] = object_fn;
	objects[1] = a;
	objects[2] = b;

	mocha_list arguments;
	arguments.objects = objects;
	arguments.count = 3;

	return execute(context, object_fn, &arguments);
}

const mocha_object* execute_3(const mocha_context* context, const mocha_object* object_fn, const mocha_object* a, const mocha_object* b, const mocha_object* c)
{
	const mocha_object* objects[4];

	objects[0] = object_fn;
	objects[1] = a;
	objects[2] = b;
	objects[3] = c;

	mocha_list arguments;
	arguments.objects = objects;
	arguments.count = 4;

	return execute(context, object_fn, &arguments);
}

const mocha_object* script_function_object_from_c_string(const struct mocha_context* context, const char* function_name)
{
	const mocha_object* symbol_object = mocha_values_create_symbol(context->values, function_name);
	const mocha_object* function_object = mocha_context_lookup(context, symbol_object);
	if (!function_object) {
		MOCHA_ERROR("No script function found %s", function_name);
		return 0;
	}
	return function_object;
}
