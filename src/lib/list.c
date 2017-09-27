#include <mocha/list.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/values.h>

#include <stdlib.h>
#include <tyran/tyran_clib.h>

size_t mocha_list_init_prepare(mocha_list* self, tyran_memory* object_array_memory, size_t count)
{
	self->objects = (const mocha_object**) TYRAN_MEMORY_CALLOC_TYPE_COUNT(object_array_memory, mocha_object*, count);
	if (count > 0 && self->objects == 0) {
		MOCHA_LOG("BREAKIT");
	}
	self->count = count;

	return sizeof(mocha_object*)* count;
}

static size_t count_fn(const mocha_sequence* _self)
{
	const mocha_list* self = (const mocha_list*) _self;
	return self->count;
}

static const mocha_object* get_fn(const mocha_sequence* _self, mocha_values* values, size_t index)
{
	const mocha_list* self = (const mocha_list*) _self;
	if (index >= self->count) {
		return mocha_values_create_nil(values);
	}
	return self->objects[index];
}

static const mocha_object* get_object_fn(const mocha_sequence* _self, mocha_values* values, const mocha_object* key)
{
	size_t index = mocha_object_integer(key, "get_object_index");
	return get_fn(_self, values, index);
}

static void get_objects_fn(const mocha_sequence* _self, const mocha_object*** objects, size_t* count)
{
	const mocha_list* self = (const mocha_list*) _self;
	*objects = self->objects;
	*count = self->count;
}

void mocha_list_init(mocha_list* self, tyran_memory* object_array_memory, const mocha_object** const args, size_t count)
{
	size_t octet_count = mocha_list_init_prepare(self, object_array_memory, count);
	self->seq.get = get_fn;
	self->seq.get_object = get_object_fn;
	self->seq.get_objects = get_objects_fn;
	self->seq.count = count_fn;

	memcpy(self->objects, args, octet_count);
}

void mocha_list_deinit(mocha_list* self)
{
	tyran_free(self->objects);
	self->objects = 0;
	self->count = 0;
}

mocha_boolean mocha_list_equal(const mocha_list* self, const mocha_list* other)
{
	if (self->count != other->count) {
		return mocha_false;
	}

	for (size_t i = 0; i < self->count; ++i) {
		if (!mocha_object_equal(self->objects[i], other->objects[i])) {
			return mocha_false;
		}
	}

	return mocha_true;
}
