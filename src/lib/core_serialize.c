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
#include <mocha/core_serialize.h>
#include <mocha/def_function.h>
#include <mocha/hashed_strings.h>
#include <mocha/log.h>
#include <mocha/runtime.h>
#include <mocha/serialize_in.h>
#include <mocha/serialize_out.h>
#include <mocha/values.h>

#include <stdio.h>

extern mocha_hashed_strings* g_hashed_strings;

MOCHA_FUNCTION(ser_out_file_func)
{
	const struct mocha_object* state_blob_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_blob* state_blob = mocha_object_blob(state_blob_object);
	FILE* f = fopen("dump.raw", "wb");
	fwrite(state_blob->octets, 1, state_blob->count, f);
	fclose(f);
	return mocha_values_create_nil(context->values);
}

MOCHA_FUNCTION(ser_out_func)
{
	const struct mocha_object* state_object = mocha_runner_eval(context, arguments->objects[1]);
	mocha_serialize_out serializer;

	mocha_serialize_out_init(&serializer, g_hashed_strings);

	uint8_t temp[32 * 1024];
	size_t count = mocha_serialize_out_buf(&serializer, temp, 32 * 1024, state_object);

	const mocha_object* result_object = mocha_values_create_blob(context->values, temp, count);

	return result_object;
}

MOCHA_FUNCTION(ser_in_func)
{
	const struct mocha_object* hex_seq_object = mocha_runner_eval(context, arguments->objects[1]);
	mocha_serialize_in serializer;
	const mocha_sequence* seq = mocha_object_sequence(hex_seq_object);

	mocha_serialize_in_init(&serializer, g_hashed_strings, context->values);

	uint8_t temp[1024];
	const struct mocha_object** objects;
	size_t count;
	mocha_sequence_get_objects(seq, &objects, &count);

	for (size_t i = 0; i < count; ++i) {
		temp[i] = mocha_object_integer(objects[i], "ser_in");
	}

	const mocha_object* state_object = mocha_serialize_in_buf(&serializer, temp, count);

	return state_object;
}

void mocha_core_serialize_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION_EX(ser_out, "ser-out");
	MOCHA_DEF_FUNCTION_EX(ser_in, "ser-in");
	MOCHA_DEF_FUNCTION_EX(ser_out_file, "ser-out-file");
}
