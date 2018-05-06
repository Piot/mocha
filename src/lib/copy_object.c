#include <mocha/execute_step.h>
#include <mocha/hashed_strings.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <tyran/tyran_memory_pool.h>

#include <stdlib.h>
#include <tiny_libc/tiny_libc.h>

static const mocha_object* copy_object(mocha_values* self, const mocha_object* o);

static void copy_array(mocha_values* self, const mocha_object** target, size_t target_count, const mocha_object** source, size_t source_count)
{
	if (source_count > target_count) {
		MOCHA_ERROR("Error: Buffer is too small!");
		return;
	}
	// MOCHA_LOG("*** Copy array count %d", source_count);
	for (size_t i = 0; i < source_count; ++i) {
		// MOCHA_LOG("Copy array %p index %d", (void*)source, i);
		target[i] = copy_object(self, source[i]);
		// MOCHA_LOG("Target:%d type:%d %s", i, target[i]->type, mocha_print_object_debug_str(target[i]));
		// MOCHA_LOG("Source:%d type:%d %s", i, source[i]->type, mocha_print_object_debug_str(source[i]));
	}
	// MOCHA_LOG("*** Copy array done! %d", source_count);
}

static const mocha_object* copy_object(mocha_values* self, const mocha_object* o)
{
#define TEMP_BUF_SIZE 256
	const mocha_object* temp_buf[TEMP_BUF_SIZE];

	if (o == 0) {
		return 0;
	}
	// MOCHA_LOG("Copy %s", mocha_print_object_debug_str(o));
	switch (o->type) {
		case mocha_object_type_nil:
			return mocha_values_create_nil(self);
		case mocha_object_type_list: {
			const mocha_list* list = mocha_object_list(o);
			copy_array(self, temp_buf, TEMP_BUF_SIZE, list->objects, list->count);
			const mocha_object* after_list_object = mocha_values_create_list(self, temp_buf, list->count);
			return after_list_object;
		}
		case mocha_object_type_map: {
			const mocha_map* map = mocha_object_map(o);
			copy_array(self, temp_buf, TEMP_BUF_SIZE, map->objects, map->count);
			const mocha_object* after_map_object = mocha_values_create_map(self, temp_buf, map->count);
			return after_map_object;
		}
		case mocha_object_type_vector: {
			const mocha_vector* vec = mocha_object_vector(o);
			copy_array(self, temp_buf, TEMP_BUF_SIZE, vec->objects, vec->count);
			return mocha_values_create_vector(self, temp_buf, vec->count);
		}
		case mocha_object_type_integer: {
			int v = mocha_object_integer(o, "clone");
			return mocha_values_create_integer(self, v);
		}
		case mocha_object_type_string: {
			const mocha_string* str = mocha_object_string(o);
			return mocha_values_create_string(self, str->string, str->count);
		}
		case mocha_object_type_character: {
			mocha_char chr = mocha_object_character(o);
			return mocha_values_create_character(self, chr);
		}
		case mocha_object_type_keyword: {
			const mocha_keyword* keyword = mocha_object_keyword(o, "copy_object_keyword");
			return mocha_values_create_keyword_from_hash(self, keyword->hash);
		}
		case mocha_object_type_true: {
			return mocha_values_create_boolean(self, mocha_true);
		}
		case mocha_object_type_symbol: {
			MOCHA_LOG("Error can not copy symbols!");
			return 0;
		}
		case mocha_object_type_function: {
			MOCHA_LOG("Error can not copy functions!");
			return 0;
		}
		case mocha_object_type_internal_function:
			MOCHA_LOG("Error can not copy internal functions!");
			return 0;
		case mocha_object_type_blob: {
			MOCHA_LOG("Error can not copy blobs!");
			return 0;
		}
		default:
			MOCHA_LOG("Error can not copy!");
			return 0;
	}

	MOCHA_LOG("ERROR! DID NOT COPY!");
	return 0;
}

static const mocha_object* copy_object_and_children(mocha_values* self, const mocha_object* o)
{
	return copy_object(self, o);
}

const mocha_object* mocha_values_deep_copy(mocha_values* self, const mocha_object* object_to_copy)
{
	// MOCHA_LOG("deep copy: '%s'", mocha_print_object_debug_str(object_to_copy));
	const mocha_object* clone = copy_object_and_children(self, object_to_copy);
	// MOCHA_LOG("deep copy clone: '%s'", mocha_print_object_debug_str(clone));

	return clone;
}
