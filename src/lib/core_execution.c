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
	const mocha_object* recur_arguments = mocha_runner_eval_arguments(context, arguments);
	const mocha_object* recur = mocha_values_create_recur(context->values, recur_arguments);

	return recur;
}

void mocha_core_execution_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(recur);
}
