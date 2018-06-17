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
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/print.h>
#include <mocha/type.h>
#include <mocha/values.h>

mocha_boolean mocha_object_equal(const mocha_object* a, const mocha_object* b)
{
	if (a->type != b->type) {
		return mocha_false;
	}

	switch (a->type) {
		case mocha_object_type_integer:
			return a->data.integer == b->data.integer;
		case mocha_object_type_string:
			return mocha_string_equal(&a->data.string, &b->data.string);
		case mocha_object_type_keyword:
			return a->data.keyword.hash == b->data.keyword.hash;
		case mocha_object_type_true:
			return mocha_true;
		case mocha_object_type_symbol:
			return a->data.symbol.total_hash == b->data.symbol.total_hash;
		case mocha_object_type_map:
			return mocha_map_equal(&a->data.map, &b->data.map);
		case mocha_object_type_vector:
			return mocha_vector_equal(&a->data.vector, &b->data.vector);
		case mocha_object_type_list:
			return mocha_list_equal(&a->data.list, &b->data.list);
		case mocha_object_type_nil:
			return mocha_true;
		case mocha_object_type_blob:
			return mocha_false;
		case mocha_object_type_character:
			return (a->data.character == b->data.character);
		case mocha_object_type_function:
			return a == b;
		case mocha_object_type_internal_function:
			return a == b;
		default:
			return mocha_false;
	}
}

mocha_boolean mocha_object_less(const mocha_object* a, const mocha_object* b)
{
	if (a->type != b->type) {
		return mocha_false;
	}

	switch (a->type) {
		case mocha_object_type_integer:
			return a->data.integer < b->data.integer;
		case mocha_object_type_string:
			return mocha_string_less(&a->data.string, &b->data.string);
		case mocha_object_type_keyword:
			return a->data.keyword.hash < b->data.keyword.hash;
		case mocha_object_type_true:
			return mocha_false;
		case mocha_object_type_symbol:
			return a->data.symbol.total_hash < b->data.symbol.total_hash;
		case mocha_object_type_map:
			return mocha_false; // mocha_map_less(&a->data.map, &b->data.map);
		case mocha_object_type_vector:
			return mocha_false; // return mocha_vector_less(&a->data.vector, &b->data.vector);
		case mocha_object_type_list:
			return mocha_false; // return mocha_list_less(&a->data.list, &b->data.list);
		case mocha_object_type_nil:
			return mocha_true;
		case mocha_object_type_function:
			return mocha_false;
		case mocha_object_type_blob:
			return mocha_false;
		case mocha_object_type_character:
			return (a->data.character < b->data.character);
		case mocha_object_type_internal_function:
			return mocha_false;
		case mocha_object_type_context:
			return mocha_false;
		case mocha_object_type_eval:
			return mocha_false;
		case mocha_object_type_recur:
			return mocha_false;
	}

	return mocha_false;
}

mocha_boolean mocha_object_boolean(const mocha_object* a)
{
	if (a->type == mocha_object_type_nil) {
		return mocha_false;
	} else if (a->type == mocha_object_type_true) {
		return mocha_true;
	}
	MOCHA_ERROR("ERROR NOT A BOOLEAN!");
	return mocha_false;
}

mocha_boolean mocha_object_truthy(const mocha_object* a)
{
	return (a->type != mocha_object_type_nil);
}

mocha_boolean mocha_object_is_nil(const mocha_object* a)
{
	return a->type == mocha_object_type_nil;
}

const mocha_map* mocha_object_map(const mocha_object* a)
{
	if (a->type == mocha_object_type_map) {
		return &a->data.map;
	}

	MOCHA_ERROR("Error: wasn't map %s", mocha_print_object_debug_str(a));
	return 0;
}

const mocha_list* mocha_object_list(const mocha_object* a)
{
	if (a->type == mocha_object_type_list) {
		return &a->data.list;
	}

	MOCHA_ERROR("Error: wasn't list");
	return 0;
}

const mocha_vector* mocha_object_vector(const mocha_object* a)
{
	if (a->type == mocha_object_type_vector) {
		return &a->data.vector;
	}

	MOCHA_ERROR("Error: wasn't vector");
	return 0;
}

const mocha_keyword* mocha_object_keyword(const mocha_object* a, const char* debug)
{
	if (a->type == mocha_object_type_keyword) {
		return &a->data.keyword;
	}

	MOCHA_ERROR("Error: wasn't a keyword '%s' %p %d", debug, (const void*) a, a->type);
	return 0;
}

const mocha_symbol* mocha_object_symbol(const mocha_object* a)
{
	if (a->type == mocha_object_type_symbol) {
		return &a->data.symbol;
	}

	MOCHA_ERROR("Error: wasn't a symbol");
	return 0;
}

const mocha_symbol* mocha_object_symbol_without_namespace(const mocha_object* a)
{
	if (a->type == mocha_object_type_symbol) {
		const mocha_symbol* s = &a->data.symbol;
		if (s->has_namespace) {
			MOCHA_ERROR("Error: included a namespace!");
			return 0;
		}
		return s;
	}

	MOCHA_ERROR("Error: wasn't a symbol");
	return 0;
}

int32_t mocha_object_integer(const mocha_object* a, const char* debug)
{
	if (a == 0) {
		MOCHA_ERROR("ERROR: '%s'", debug);
		return -99999999;
	}
	if (a->type == mocha_object_type_integer) {
		return a->data.integer;
	}
	MOCHA_ERROR("Not an (%p) integer:%d %s", (const void*) a, a->type, debug);
	return -9999999;
}

size_t mocha_object_unsigned(const mocha_object* a)
{
	int v = mocha_object_integer(a, "unsigned");

	if (v < 0) {
		MOCHA_ERROR("Unsigned problem");
		v = 0;
	}
	return (size_t) v;
}

const mocha_function* mocha_object_function(const mocha_object* a)
{
	if (a->type == mocha_object_type_function) {
		return &a->data.function;
	}

	return 0;
}

const mocha_blob* mocha_object_blob(const mocha_object* a)
{
	if (a->type == mocha_object_type_blob) {
		return &a->data.blob;
	}

	return 0;
}

const mocha_string* mocha_object_string(const mocha_object* a)
{
	if (a->type == mocha_object_type_string) {
		return &a->data.string;
	}

	return 0;
}

mocha_char mocha_object_character(const mocha_object* a)
{
	if (a->type == mocha_object_type_character) {
		return a->data.character;
	}

	return 0;
}

mocha_boolean mocha_object_is_primitive(const mocha_object* a)
{
	switch (a->type) {
		case mocha_object_type_integer:
			return mocha_true;
		case mocha_object_type_string:
			return mocha_true;
		case mocha_object_type_keyword:
			return mocha_true;
		case mocha_object_type_true:
			return mocha_true;
		case mocha_object_type_symbol:
			return mocha_true;
		case mocha_object_type_map:
			return mocha_true;
		case mocha_object_type_vector:
			return mocha_true;
		case mocha_object_type_list:
			return mocha_true;
		case mocha_object_type_nil:
			return mocha_true;
		case mocha_object_type_blob:
			return mocha_true;
		case mocha_object_type_function:
			return mocha_true;
		case mocha_object_type_internal_function:
			return mocha_true;
		case mocha_object_type_character:
			return mocha_true;
		default:
			return mocha_false;
	}
}

const mocha_context* mocha_object_context(const mocha_object* a)
{
	if (a->type == mocha_object_type_context) {
		return &a->data.context;
	}

	return 0;
}

mocha_boolean mocha_object_is_function(const mocha_object* a)
{
	return (a->type == mocha_object_type_function) || (a->type == mocha_object_type_internal_function);
}

mocha_boolean mocha_object_is_invokable(const mocha_object* a)
{
	return (a->object_type && a->object_type->invoke != 0) || mocha_object_is_function(a); //(a->type == mocha_object_type_function);
}

mocha_boolean mocha_object_is_list(const mocha_object* a)
{
	return (a->type == mocha_object_type_list);
}

mocha_boolean mocha_object_is_map(const mocha_object* a)
{
	return (a->type == mocha_object_type_map);
}

mocha_boolean mocha_object_is_vector(const mocha_object* a)
{
	return (a->type == mocha_object_type_vector);
}

mocha_boolean mocha_object_is_symbol(const mocha_object* a)
{
	return (a->type == mocha_object_type_symbol);
}

mocha_boolean mocha_object_is_integer(const mocha_object* a)
{
	return (a->type == mocha_object_type_integer);
}

mocha_boolean mocha_object_is_string(const mocha_object* a)
{
	return (a->type == mocha_object_type_string);
}

mocha_boolean mocha_object_is_sequence(const mocha_object* a)
{
	mocha_object_type t = a->type;
	return (t == mocha_object_type_nil || t == mocha_object_type_list || t == mocha_object_type_map || t == mocha_object_type_vector || t == mocha_object_type_string);
}

mocha_boolean mocha_object_is_blob(const mocha_object* a)
{
	return (a->type == mocha_object_type_blob);
}

mocha_boolean mocha_object_is_valid(const mocha_object* a)
{
	mocha_object_type t = a->type;
	return (t >= mocha_object_type_nil && t <= mocha_object_type_context);
}

const mocha_sequence* mocha_object_sequence(const mocha_object* a)
{
	switch (a->type) {
		case mocha_object_type_map:
			return &a->data.map.seq;
		case mocha_object_type_list:
			return &a->data.list.seq;
		case mocha_object_type_vector:
			return &a->data.vector.seq;
		case mocha_object_type_string:
			return &a->data.string.seq;
		case mocha_object_type_nil:
			mocha_nil_init((mocha_nil*) &a->data.nil);
			return &a->data.nil.seq;
		default:
			MOCHA_ERROR("Object is not a sequence: %s", mocha_print_object_debug_str(a));
			return 0;
	}
}
