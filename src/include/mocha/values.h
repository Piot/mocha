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
#ifndef mocha_values_h
#define mocha_values_h

#include <imprint/memory.h>
#include <mocha/map.h>
#include <mocha/object.h>
#include <mocha/string.h>
#include <mocha/type.h>

struct mocha_context;
struct imprint_memory_pool;
struct mocha_runtime;
struct mocha_hashed_strings;

typedef struct mocha_values_config {
	size_t objects_count;
	size_t object_reference_count;
	size_t blob_memory_size;
	size_t string_memory_size;
} mocha_values_config;

typedef struct mocha_values {
	mocha_type keyword_def;
	mocha_type map_def;

	struct imprint_memory memory;
	struct imprint_memory objects;
	struct imprint_memory object_references;
	struct imprint_memory blob_memory;
	struct imprint_memory string_content_memory;
	struct mocha_hashed_strings* hashed_strings;

	mocha_object nil_object;
	mocha_object true_object;
	const char* debug;
} mocha_values;

void mocha_values_init(mocha_values* self, struct mocha_hashed_strings* hashed_strings, struct imprint_memory* memory, mocha_values_config config, const char* debugstring);

const struct mocha_object* mocha_values_create_boolean(mocha_values* values, mocha_boolean value);

const struct mocha_object* mocha_values_create_internal_function(mocha_values* self, const struct mocha_type* type, const char* name);

const struct mocha_object* mocha_values_create_internal_function_ex(mocha_values* self, const struct mocha_type* type, const char* name, void* user_data);

const struct mocha_object* mocha_values_create_macro(mocha_values* self, const struct mocha_context* context, const mocha_object* name, const mocha_object* arguments, const mocha_object* body);

const struct mocha_object* mocha_values_create_function(mocha_values* self, const struct mocha_context* context, const mocha_object* name, const struct mocha_object* arguments, const struct mocha_object* body);

const struct mocha_object* mocha_values_create_symbol(mocha_values* self, const char* key_string);

const struct mocha_object* mocha_values_create_symbol_from_hash(mocha_values* self, mocha_string_hash hash);

const struct mocha_object* mocha_values_create_map(mocha_values* self, const struct mocha_object** args, size_t count);

const struct mocha_object* mocha_values_create_list(mocha_values* self, const struct mocha_object** args, size_t count);

const struct mocha_object* mocha_values_create_list_cons(mocha_values* self, const mocha_list* list, const mocha_object* a);

const struct mocha_object* mocha_values_create_vector(mocha_values* self, const struct mocha_object** args, size_t count);

const struct mocha_object* mocha_values_create_string_from_cstr(mocha_values* self, const char* s);

const struct mocha_object* mocha_values_create_string_from_cstr_ex(mocha_values* self, const char* s, size_t count);

const struct mocha_object* mocha_values_create_string(mocha_values* self, const mocha_char* s, size_t count);

const struct mocha_object* mocha_values_create_character(mocha_values* self, mocha_char s);

const struct mocha_object* mocha_values_create_integer(mocha_values* self, int32_t v);

const struct mocha_object* mocha_values_create_nil(mocha_values* self);

const struct mocha_object* mocha_values_create_blob(mocha_values* self, const uint8_t* octets, size_t count);

const struct mocha_object* mocha_values_create_keyword(mocha_values* self, const char* s);
const struct mocha_object* mocha_values_create_keyword_from_hash(mocha_values* self, mocha_string_hash hash);

const struct mocha_object* mocha_values_create_closure(mocha_values* self, const struct mocha_context* context, const mocha_object* object);
const struct mocha_object* mocha_values_create_eval(mocha_values* self, const struct mocha_context* parent, const mocha_object* object);
const struct mocha_object* mocha_values_create_context(mocha_values* self, const struct mocha_context* parent, const char* debugstring);
const struct mocha_object* mocha_values_create_sequence(mocha_values* self, mocha_object_type type, const mocha_object* objects[], size_t count);
const struct mocha_object* mocha_values_create_recur(mocha_values* self, const mocha_object* arguments);

void mocha_values_delete(mocha_values* self, const mocha_object* o);
void mocha_values_clear(mocha_values* self);

void mocha_values_print_debug(const mocha_values* self, const char* debugstring);

#endif
