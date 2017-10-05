#include <mocha/core.h>
#include <mocha/core_arithmetic.h>
#include <mocha/core_collection.h>
#include <mocha/core_def.h>
#include <mocha/core_logic.h>
#include <mocha/core_compare.h>
#include <mocha/core_math.h>
#include <mocha/core_thread.h>
#include <mocha/core_bit.h>
#include <mocha/def_function.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>

#include <mocha/core_import.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
   MOCHA_FUNCTION(dbg_ptr_func)
   {
        const mocha_object* o = arguments->objects[1];
        const mocha_object* result = mocha_values_create_integer(context->values,  (int) o);
        return result;
   }

   MOCHA_FUNCTION(dbg_sleep_func)
   {
        struct timespec requested, remaining;
        const mocha_object* o = arguments->objects[1];
        requested.tv_sec = mocha_object_integer(o);
        requested.tv_nsec = 0;
        nanosleep(&requested, &remaining);
        const mocha_object* result = mocha_values_create_integer(context->values,  0);
        return result;
   }



   MOCHA_FUNCTION(fail_func)
   {
        const mocha_object* argument = arguments->objects[1];
        // if (argument->type != mocha_object_type_string) {
        //      return 0;
        // }
        runtime->error.code = mocha_error_code_fail;
        runtime->error.string = mocha_string_to_c(&argument->data.string);
        const mocha_object* o = mocha_values_create_nil(runtime->values);
        return o;
   }


   MOCHA_FUNCTION(println_func)
   {
        const mocha_object* argument = arguments->objects[1];
        mocha_print_object_debug_no_quotes(argument);
        return mocha_values_create_nil(runtime->values);
   }

 */
/*
MOCHA_FUNCTION(quote_func)
{
	return arguments->objects[1];
}
*/
typedef struct log_state
{
	mocha_values* values;
} log_state;

MOCHA_FUNCTION(log_func)
{
	char temp[1024];

	temp[0] = 0;
	for (size_t i = 1; i < arguments->count; ++i) {
		const mocha_object* argument = arguments->objects[i];

		if (i != 0) {
			strcat(temp, " ");
		}
		strcat(temp, mocha_print_object_debug_str(argument));
	}
	MOCHA_LOG_INFO("log: %s", temp);
	const mocha_object* nil = mocha_values_create_nil(context->values);
	// tyran_free(state);
	return nil;
}

void mocha_core_define_context(mocha_context* context, mocha_values* values)
{
/*	mocha_core_arithmetic_define_context(context, values);
  mocha_core_bit_define_context(context, values);
	mocha_core_collection_define_context(context, values);
	mocha_core_compare_define_context(context, values);
	mocha_core_def_define_context(context, values);
  mocha_core_math_define_context(context, values);
	mocha_core_thread_define_context(context, values);
  mocha_core_import_define_context(context, values);
*/
mocha_core_logic_define_context(context, values);
/*
	   MOCHA_DEF_FUNCTION(unquote, mocha_false);
	   MOCHA_DEF_FUNCTION(fail, mocha_true);
	 */
	// MOCHA_DEF_FUNCTION(println, mocha_true);
	MOCHA_DEF_FUNCTION(log, mocha_true);
//	MOCHA_DEF_FUNCTION(quote, mocha_false);

	// DEBUG
	// MOCHA_DEF_FUNCTION(dbg_ptr, mocha_true);
	// MOCHA_DEF_FUNCTION(dbg_sleep, mocha_true);
}
