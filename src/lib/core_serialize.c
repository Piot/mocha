#include <mocha/core_serialize.h>
#include <mocha/def_function.h>
#include <mocha/hashed_strings.h>
#include <mocha/runtime.h>
#include <mocha/serialize_out.h>
#include <mocha/values.h>

extern mocha_hashed_strings* g_hashed_strings;

MOCHA_FUNCTION(ser_out_func)
{
	const struct mocha_object* state_object = mocha_runner_eval(context, arguments->objects[1]);
	mocha_serialize_out serializer;

	mocha_serialize_out_init(&serializer, g_hashed_strings);

	uint8_t temp[1024];
	size_t count = mocha_serialize_out_buf(&serializer, temp, 1024, state_object);

	const mocha_object* result_object = mocha_values_create_blob(context->values, temp, count);

	return result_object;
}

void mocha_core_serialize_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION_EX(ser_out, "ser-out");
}
