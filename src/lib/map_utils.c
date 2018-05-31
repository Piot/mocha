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
#include <mocha/map.h>
#include <mocha/map_utils.h>
#include <mocha/print.h>
#include <mocha/values.h>
#include <stdlib.h>
#include <tiny-libc/tiny_libc.h>

const mocha_object* mocha_map_lookup_c_string(mocha_values* values, const mocha_map* map, const char* s)
{
	const mocha_object* key = mocha_values_create_keyword(values, s);
	const mocha_object* value = mocha_map_lookup(map, key);

	return value;
}

int mocha_map_lookup_c_string_int_found(mocha_values* values, const mocha_map* map, const char* s, tyran_boolean* found)
{
	const mocha_object* object = mocha_map_lookup_c_string(values, map, s);

	if (!object || !mocha_object_is_integer(object)) {
		*found = TYRAN_FALSE;
		return -1;
	}

	*found = TYRAN_TRUE;

	return mocha_object_integer(object, s);
}

int mocha_map_lookup_c_string_int(mocha_values* values, const mocha_map* map, const char* s)
{
	tyran_boolean was_found;
	int result = mocha_map_lookup_c_string_int_found(values, map, s, &was_found);

	if (!was_found) {
		MOCHA_LOG("ERror looking up int!");
		MOCHA_LOG("Couldn't look it up %s from %s", s, mocha_print_map_debug_str(values, map));
		return -1;
	}

	return result;
}

mocha_boolean mocha_map_lookup_c_string_boolean(mocha_values* values, const mocha_map* map, const char* s)
{
	const mocha_object* object = mocha_map_lookup_c_string(values, map, s);

	if (object == 0) {
		MOCHA_LOG("ERror looking up boolean!");
		MOCHA_LOG("Couldn't look it up %s from %s", s, mocha_print_map_debug_str(values, map));
		return -1;
	}

	return mocha_object_boolean(object);
}

mocha_boolean mocha_map_lookup_c_string_boolean_with_default(struct mocha_values* values, const struct mocha_map* map, const char* s, mocha_boolean default_value)
{
	const mocha_object* object = mocha_map_lookup_c_string(values, map, s);

	if (object == 0) {
		return default_value;
	}

	return mocha_object_boolean(object);
}

const mocha_map* mocha_map_lookup_c_string_map_allow_nil(mocha_values* values, const mocha_map* map, const char* s)
{
	const mocha_object* object = mocha_map_lookup_c_string(values, map, s);
	// MOCHA_LOG("lookup %s", s);
	if (mocha_object_is_nil(object)) {
		return 0;
	}

	if (!mocha_object_is_map(object)) {
		MOCHA_ERROR("map_lookup_c_string_map: not a map! name:%s %s %s", s, mocha_print_map_debug_str(values, map), mocha_print_object_debug_str(object));
		return 0;
	}
	return mocha_object_map(object);
}

const mocha_map* mocha_map_lookup_c_string_map(mocha_values* values, const mocha_map* map, const char* s)
{
	const mocha_object* object = mocha_map_lookup_c_string(values, map, s);
	// MOCHA_LOG("lookup %s", s);
	if (!mocha_object_is_map(object)) {
		MOCHA_ERROR("map_lookup_c_string_map: not a map! name:%s %s %s", s, mocha_print_map_debug_str(values, map), mocha_print_object_debug_str(object));
		return 0;
	}
	return mocha_object_map(object);
}

const struct mocha_vector* mocha_map_lookup_c_string_vector(struct mocha_values* values, const struct mocha_map* map, const char* s)
{
	const mocha_object* object = mocha_map_lookup_c_string(values, map, s);
	// MOCHA_LOG("lookup %s", s);
	return mocha_object_vector(object);
}

const struct mocha_sequence* mocha_map_lookup_c_string_sequence(struct mocha_values* values, const struct mocha_map* map, const char* s)
{
	const mocha_object* object = mocha_map_lookup_c_string(values, map, s);
	// MOCHA_LOG("lookup %s", s);
	return mocha_object_sequence(object);
}

const struct mocha_blob* mocha_map_lookup_c_string_blob(struct mocha_values* values, const struct mocha_map* map, const char* s)
{
	const mocha_object* object = mocha_map_lookup_c_string(values, map, s);

	return mocha_object_blob(object);
}

static int map_find_index(const mocha_map* map, const mocha_object* key)
{
	for (size_t i = 0; i < map->count; i += 2) {
		if (key && mocha_object_equal(map->objects[i], key)) {
			return (int) i;
		}
	}

	return -1;
}

const struct mocha_object* mocha_map_assoc(const mocha_map* map, mocha_values* values, const mocha_object** adds, size_t add_count)
{
	size_t new_count = map->count + add_count;
	const mocha_object** result = tc_malloc(sizeof(mocha_object*) * new_count);
	tc_memcpy_type_n(result, map->objects, map->count);

	size_t end_count = map->count;

	for (size_t i = 0; i < add_count; i += 2) {
		const mocha_object* key = adds[i];
		if (!mocha_object_is_valid(key)) {
			MOCHA_ERROR("Object map is corrupt %p", map);
		}
		const mocha_object* value = adds[i + 1];
		if (!mocha_object_is_valid(value)) {
			MOCHA_ERROR("Object map is corrupt %p", map);
			return 0;
		}
		int index = map_find_index(map, key);
		if (index >= 0) {
			result[index + 1] = value;
		} else {
			// MOCHA_LOG("Couldn't find '%s' adding to end", mocha_print_object_debug_str(key));
			result[end_count] = key;
			result[end_count + 1] = value;
			end_count += 2;
		}
	}

	const mocha_object* new_map = mocha_values_create_map(values, result, end_count);
	tc_free(result);

	return new_map;
}
