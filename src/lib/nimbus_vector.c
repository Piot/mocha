#include <tyran/tyran_clib.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <tyran/tyran_memory.h>
#include <mocha/vector.h>
#include <mocha/values.h>

#include <stdlib.h>

static size_t count_fn(const mocha_sequence* _self)
{
	const mocha_vector* self = (const mocha_vector*) _self;
	//	MOCHA_LOG("vec:count");
	return self->count;
}

static const mocha_object* get_fn(const mocha_sequence* _self, struct mocha_values* values, size_t index)
{
	const mocha_vector* self = (const mocha_vector*) _self;
	// MOCHA_LOG("vec:get %d %d", index, self->count);
	if (index >= self->count) {
		return mocha_values_create_nil(values);
	}
	return self->objects[index];
}

static const mocha_object* get_object_fn(const mocha_sequence* _self, struct mocha_values* values, const mocha_object* key)
{
	size_t index = mocha_object_integer(key, "vector_get_obj");
	return get_fn(_self, values, index);
}

static void get_objects_fn(const mocha_sequence* _self, const mocha_object*** objects, size_t* count)
{
	const mocha_vector* self = (const mocha_vector*) _self;
	*objects = self->objects;
	*count = self->count;
}

void mocha_vector_init(mocha_vector* self, tyran_memory* object_array_memory, const struct mocha_object* args[], size_t count)
{
	self->objects = (const mocha_object**) TYRAN_MEMORY_ALLOC_TYPE_COUNT(object_array_memory, mocha_object*, count);
	self->count = count;
	self->seq.count = count_fn;
	self->seq.get = get_fn;
	self->seq.get_object = get_object_fn;
	self->seq.get_objects = get_objects_fn;
	tyran_memcpy_type_n(self->objects, args, count);
}

mocha_boolean mocha_vector_equal(const mocha_vector* self, const mocha_vector* other)
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
