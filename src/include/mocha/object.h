#ifndef mocha_object_h
#define mocha_object_h

#include <mocha/blob.h>
#include <mocha/closure.h>
#include <mocha/context.h>
#include <mocha/function.h>
#include <mocha/keyword.h>
#include <mocha/list.h>
#include <mocha/map.h>
#include <mocha/nil.h>
#include <mocha/string.h>
#include <mocha/symbol.h>
#include <mocha/transducer.h>

#include <mocha/execute_step_data.h>
#include <mocha/recur.h>
#include <mocha/types.h>
#include <mocha/vector.h>

#include "object_type.h"

struct mocha_type;
struct mocha_values;
struct mocha_object;

typedef struct mocha_object {
	mocha_object_type type;
	union {
		mocha_list list;
		mocha_map map;
		mocha_vector vector;
		int32_t integer;
		mocha_string string;
		mocha_char character;
		mocha_symbol symbol;
		mocha_keyword keyword;
		mocha_function function;
		mocha_blob blob;
		mocha_closure closure;
		mocha_context context;
		mocha_recur recur;
		mocha_nil nil;
	} data;
	const struct mocha_type* object_type;
	const char* debug_string;
	void* user_data;
	struct mocha_values* values;
	int reference_count;
} mocha_object;

mocha_boolean mocha_object_boolean(const mocha_object* a);
mocha_boolean mocha_object_equal(const mocha_object* a, const mocha_object* b);
mocha_boolean mocha_object_less(const mocha_object* a, const mocha_object* b);
mocha_boolean mocha_object_truthy(const mocha_object* a);
const mocha_map* mocha_object_map(const mocha_object* a);
const mocha_list* mocha_object_list(const mocha_object* a);
const mocha_vector* mocha_object_vector(const mocha_object* a);
const mocha_keyword* mocha_object_keyword(const mocha_object* a, const char* debug);
const mocha_symbol* mocha_object_symbol(const mocha_object* a);
const mocha_symbol* mocha_object_symbol_without_namespace(const mocha_object* a);
int32_t mocha_object_integer(const mocha_object* a, const char* debug);
size_t mocha_object_unsigned(const mocha_object* a);
const mocha_function* mocha_object_function(const mocha_object* a);
const mocha_blob* mocha_object_blob(const mocha_object* a);
const mocha_string* mocha_object_string(const mocha_object* a);
const mocha_sequence* mocha_object_sequence(const mocha_object* a);
const mocha_closure* mocha_object_closure(const mocha_object* a);
mocha_char mocha_object_character(const mocha_object* a);
const mocha_context* mocha_object_context(const mocha_object* a);
const mocha_execute_step_data* mocha_object_execute_step_data(const mocha_object* a);
mocha_boolean mocha_object_is_nil(const mocha_object* a);
mocha_boolean mocha_object_is_primitive(const mocha_object* a);
mocha_boolean mocha_object_is_invokable(const mocha_object* a);
mocha_boolean mocha_object_is_symbol(const mocha_object* a);
mocha_boolean mocha_object_is_function(const mocha_object* a);
mocha_boolean mocha_object_is_list(const mocha_object* a);
mocha_boolean mocha_object_is_vector(const mocha_object* a);
mocha_boolean mocha_object_is_map(const mocha_object* a);
mocha_boolean mocha_object_is_sequence(const mocha_object* a);
mocha_boolean mocha_object_is_integer(const mocha_object* a);
mocha_boolean mocha_object_is_string(const mocha_object* a);
mocha_boolean mocha_object_is_execute_step_data(const mocha_object* a);
mocha_boolean mocha_object_is_closure(const mocha_object* a);
mocha_boolean mocha_object_is_valid(const mocha_object* a);
mocha_boolean mocha_object_is_blob(const mocha_object* a);
#endif
