#include <mocha/core_def.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/recur.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tyran/tyran_clib.h>

// const mocha_object* mocha_recur_create_step(const mocha_context* context, const mocha_list* arguments, const mocha_function* fn, const char* debug)

MOCHA_FUNCTION(recur_func)
{
	const mocha_function* fn = mocha_object_function(arguments->objects[1]);
	mocha_list rest;
	rest.objects = &arguments->objects[2];
	rest.count = arguments->count - 1;
	return mocha_recur_create_step(context, arguments, fn, "recur");
}

void mocha_core_execution_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(recur);
}
