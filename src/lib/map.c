#include <mocha/log.h>
#include <mocha/map.h>
#include <mocha/object.h>
#include <mocha/print.h>
#include <tiny_libc/tiny_libc.h>
#include <tyran/tyran_memory.h>

#include <mocha/values.h>
#include <stdlib.h>

size_t mocha_map_init_prepare(mocha_map* self, tyran_memory* object_array_memory, size_t count)
{
	self->objects = (const mocha_object**) TYRAN_MEMORY_CALLOC_TYPE_COUNT(object_array_memory, mocha_object*, count);
	self->count = count;

	return 0;
}

static size_t count_fn(const mocha_sequence* _self)
{
	const mocha_map* self = (const mocha_map*) _self;
	return self->count;
}

static const mocha_object* get_fn(const mocha_sequence* _self, mocha_values* values, size_t index)
{
	const mocha_map* self = (const mocha_map*) _self;
	if (index >= self->count) {
		return mocha_values_create_nil(values);
	}
	return self->objects[index];
}

static const mocha_object* get_object_fn(const mocha_sequence* _self, mocha_values* values, const mocha_object* key)
{
	const mocha_map* self = (const mocha_map*) _self;
	// MOCHA_LOG("key %s", mocha_print_object_debug_str(key));
	const mocha_object* value = mocha_map_lookup(self, key);
	if (value == 0) {
		value = mocha_values_create_nil(values);
	}
	// MOCHA_LOG("Found value %s", mocha_print_object_debug_str(value));
	return value;
}

static void get_objects_fn(const mocha_sequence* _self, const mocha_object*** objects, size_t* count)
{
	const mocha_map* self = (const mocha_map*) _self;
	*objects = self->objects;
	*count = self->count;
}

void mocha_map_init(mocha_map* self, tyran_memory* object_array_memory, const mocha_object** args, size_t count)
{
	if ((count % 2) != 0) {
		MOCHA_LOG("ERROR: Must have even numbers in map!");
		self->count = 0;
		return;
	}

	self->seq.count = count_fn;
	self->seq.get = get_fn;
	self->seq.get_object = get_object_fn;
	self->seq.get_objects = get_objects_fn;

	self->objects = (const mocha_object**) TYRAN_MEMORY_ALLOC_TYPE_COUNT(object_array_memory, mocha_object*, count);
	self->count = count;
	tc_memcpy_type_n(self->objects, args, count);
}

void mocha_map_deinit(mocha_map* self)
{
	(void) self;
}

const struct mocha_object* mocha_map_lookup(const mocha_map* self, const struct mocha_object* key)
{
	if (self == 0) {
		MOCHA_ERROR("Can not lookup in null map");
	}
	// MOCHA_LOG("Searching for %s", mocha_print_object_debug_str(key));
	for (size_t i = 0; i < self->count; i += 2) {
		// MOCHA_LOG("Comparing to %s", mocha_print_object_debug_str(self->objects[i]));
		if (mocha_object_equal(self->objects[i], key)) {
			const mocha_object* found_object = self->objects[i + 1];
			if (!mocha_object_is_valid(found_object)) {
				MOCHA_ERROR("Object map is corrupt %p", self);
				return 0;
			}
			return found_object;
		}
	}
	// MOCHA_LOG("Couldn't find key in map '%s'", mocha_print_object_debug_str(key));
	return 0;
}

mocha_boolean mocha_map_equal(const mocha_map* self, const mocha_map* other)
{
	if (self->count != other->count) {
		return mocha_false;
	}

	for (size_t i = 0; i < other->count; i += 2) {
		const mocha_object* key = self->objects[i];
		const mocha_object* other_value = mocha_map_lookup(other, key);

		if (!other_value) {
			return mocha_false;
		}
		const mocha_object* value = self->objects[i + 1];

		if (!mocha_object_equal(value, other_value)) {
			return mocha_false;
		}
	}

	return mocha_true;
}
