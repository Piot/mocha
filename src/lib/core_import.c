#include <mocha/core_import.h>

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

#include <mocha/eval.h>
#include <tyran/tyran_clib.h>
/*
typedef struct parse_func_info {
	resolve_callback callback_info;
	mocha_context* parse_context;
	const mocha_context* root_context;
	const mocha_keyword* import_name;
} parse_func_info;

static void parse_func_done(void* user_data, const mocha_object* done)
{
	parse_func_info* self = (parse_func_info*) user_data;

	mocha_context_import(((mocha_context*) self->root_context), self->parse_context, self->import_name);
	const mocha_object* nil = mocha_values_create_nil(self->parse_context->values);
	self->callback_info.callback(self->callback_info.user_data, nil);
}

MOCHA_FUNCTION(parse_func)
{
	const mocha_keyword* name = mocha_object_keyword(arguments->objects[1], "import"); // mocha_runtime_eval(runtime, arguments->objects[1], &runtime->error);
	const mocha_string* forms_string = mocha_object_string(arguments->objects[2]);	 // mocha_runtime_eval(runtime, arguments->objects[1], &runtime->error);
	char* temp_buf = tyran_malloc(sizeof(char) * (forms_string->count + 1));
	const char* code_string = mocha_string_to_c_buf(forms_string, temp_buf, forms_string->count + 1);

	mocha_context* parse_context = mocha_context_create(context, "parse");
	const mocha_context* root_context = context->values->runtime->root_context;
	parse_func_info* info = tyran_malloc(sizeof(parse_func_info));

	info->callback_info = result_callback;
	info->root_context = root_context;
	info->parse_context = parse_context;
	info->import_name = name;

	resolve_callback callback;
	callback.user_data = info;
	callback.callback = parse_func_done;
	mocha_eval_string(parse_context, code_string, callback.user_data, callback.callback);
}


static void parse_string_func_done(void* user_data, const mocha_object* done)
{
	parse_func_info* self = (parse_func_info*) user_data;

	self->callback_info.callback(self->callback_info.user_data, done);
}


MOCHA_FUNCTION(parse_string_func)
{
	const mocha_string* forms_string = mocha_object_string(arguments->objects[1]);	 // mocha_runtime_eval(runtime, arguments->objects[1], &runtime->error);
	char* temp_buf = tyran_malloc(sizeof(char) * (forms_string->count + 1));
	const char* code_string = mocha_string_to_c_buf(forms_string, temp_buf, forms_string->count + 1);

	mocha_context* parse_context = mocha_context_create(context, "parse");
	const mocha_context* root_context = context->values->runtime->root_context;
	parse_func_info* info = tyran_malloc(sizeof(parse_func_info));

	info->callback_info = result_callback;
	info->root_context = root_context;
	info->parse_context = parse_context;
	info->import_name = 0;

	resolve_callback callback;
	callback.user_data = info;
	callback.callback = parse_string_func_done;
	mocha_eval_string(parse_context, code_string, callback.user_data, callback.callback);
}



MOCHA_FUNCTION(parse_csv_func)
{
	const mocha_blob* blob = mocha_object_blob(arguments->objects[1]);
	char* temp_buf = tyran_malloc(sizeof(char) * (blob->count + 1));
	tyran_strncpy(temp_buf, blob->count, (const char*)blob->octets, blob->count);

	const char* p = temp_buf;
	const mocha_object* array[1024];
	size_t array_index = 0;
	const mocha_object* objects[1024];
	size_t object_index = 0;
	while (1) {
		const char* next_delimiter = tyran_strpbrk(p, ",\n");
		if (next_delimiter == 0) {
			MOCHA_LOG("Parsing done");
			break;
		}
		// MOCHA_LOG("delimiter:%x (%p)", *next_delimiter, next_delimiter);
		if (*next_delimiter == '\n') {
			const mocha_object* array_object = mocha_values_create_vector(context->values, objects, object_index);
			object_index = 0;
			array[array_index++] = array_object;
		} else {
			char value_string[1024];
			tyran_strncpy(value_string, 1024, p, (next_delimiter - p));

			int value = strtol(value_string, 0, 10);
			objects[object_index++] = mocha_values_create_integer(context->values, value);
		}
		p = next_delimiter + 1;
	}
	tyran_free(temp_buf);

	const mocha_object* result_object = mocha_values_create_vector(context->values, array, array_index);
	return  result_object);
}

void mocha_core_import_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(parse);
	MOCHA_DEF_FUNCTION_EX(parse_csv, "parse-csv");
	MOCHA_DEF_FUNCTION_EX(parse_string, "parse-string");
}
*/
