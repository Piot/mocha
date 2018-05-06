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
#include <tiny_libc/tiny_libc.h>

static const mocha_object* fn(const mocha_context* context, const mocha_object* name, const mocha_object* arguments, const mocha_object* body)
{
	const mocha_object* r = mocha_values_create_function(context->values, context, name, arguments, body);

	return r;
}

MOCHA_FUNCTION(fn_func)
{
	const mocha_object* default_name = mocha_values_create_symbol(context->values, "_fn");
	const mocha_object* r = fn(context, default_name, arguments->objects[1], arguments->objects[2]);

	return r;
}

MOCHA_FUNCTION(defn_func)
{
	if (arguments->count < 4) {
		MOCHA_LOG("defn error %s", "");
		return 0;
	}
	const mocha_object* name = arguments->objects[1];

	size_t index = 2;

	if (mocha_object_is_string(arguments->objects[index])) {
		// Skip documentation
		index++;
	}
	const mocha_object* func = fn(context, name, arguments->objects[index], arguments->objects[index + 1]);
	mocha_context_add_or_replace((mocha_context*) context, name, func);
	return func;
}

static void def(struct mocha_context* context, const mocha_object* name, const mocha_object* value)
{
	mocha_context_add_or_replace(context, name, value);
}

MOCHA_FUNCTION(def_func)
{
	const mocha_object* def_value_object = arguments->objects[2];
	const mocha_object* evaled_object = mocha_runner_eval(context, def_value_object);
	def((mocha_context*) context, arguments->objects[1], evaled_object);
	const mocha_object* r = mocha_values_create_nil(context->values);
	return r;
}

MOCHA_FUNCTION(let_func)
{
	const mocha_vector* let_vector = mocha_object_vector(arguments->objects[1]);
	if ((let_vector->count % 2) != 0) {
		MOCHA_LOG("Wrong number of assignments");
		return 0;
	}

	mocha_context* new_context = mocha_context_create(context, "let_def");

	for (size_t i = 0; i < let_vector->count; i += 2) {
		const mocha_object* symbol = let_vector->objects[i];
		const mocha_object* value = mocha_runner_eval(new_context, let_vector->objects[i + 1]);
		mocha_context_add(new_context, symbol, value);
	}

	const mocha_object* eval_object = mocha_values_create_eval(context->values, new_context, arguments->objects[2]);

	return eval_object;
}

void mocha_core_def_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(defn);
	MOCHA_DEF_FUNCTION(fn);
	MOCHA_DEF_FUNCTION(def);
	MOCHA_DEF_FUNCTION(let);
}
