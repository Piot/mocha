#include <mocha/log.h>
#include <mocha/map.h>
#include <mocha/map_utils.h>
#include <mocha/print.h>
#include <mocha/values.h>
#include <stdlib.h>
#include <tyran/tyran_clib.h>

const mocha_object* mocha_map_lookup_c_string(mocha_values* values, const mocha_map* map, const char* s)
{
	const mocha_object* key = mocha_values_create_keyword(values, s);
	const mocha_object* value = mocha_map_lookup(map, key);

	return value;
}

int mocha_map_lookup_c_string_int(mocha_values* values, const mocha_map* map, const char* s)
{
	const mocha_object* object = mocha_map_lookup_c_string(values, map, s);

	if (object == 0) {
		MOCHA_LOG("ERror looking up int!");
		MOCHA_LOG("Couldn't look it up %s from %s", s, mocha_print_map_debug_str(values, map));
		return -1;
	}

	return mocha_object_integer(object, s);
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

const mocha_map* mocha_map_lookup_c_string_map(mocha_values* values, const mocha_map* map, const char* s)
{
	const mocha_object* object = mocha_map_lookup_c_string(values, map, s);
	// MOCHA_LOG("lookup %s", s);
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
			return i;
		}
	}

	return -1;
}

const struct mocha_object* mocha_map_assoc(const mocha_map* map, mocha_values* values, const mocha_object** adds, size_t add_count)
{
	size_t new_count = map->count + add_count;
	const mocha_object** result = tyran_malloc(sizeof(mocha_object*) * new_count);
	tyran_memcpy_type_n(result, map->objects, map->count);

	size_t end_count = map->count;

	for (size_t i = 0; i < add_count; i += 2) {
		const mocha_object* key = adds[i];
		const mocha_object* value = adds[i + 1];
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
	tyran_free(result);
	
	return new_map;
}
