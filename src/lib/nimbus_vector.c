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
#include <imprint/memory.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/values.h>
#include <mocha/vector.h>
#include <tiny-libc/tiny_libc.h>

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
	size_t index = (size_t) mocha_object_integer(key, "vector_get_obj");
	return get_fn(_self, values, index);
}

static void get_objects_fn(const mocha_sequence* _self, const mocha_object*** objects, size_t* count)
{
	const mocha_vector* self = (const mocha_vector*) _self;
	*objects = self->objects;
	*count = self->count;
}

void mocha_vector_init(mocha_vector* self, imprint_memory* object_array_memory, const struct mocha_object* args[], size_t count)
{
	self->objects = (const mocha_object**) TYRAN_MEMORY_ALLOC_TYPE_COUNT(object_array_memory, mocha_object*, count);
	self->count = count;
	self->seq.count = count_fn;
	self->seq.get = get_fn;
	self->seq.get_object = get_object_fn;
	self->seq.get_objects = get_objects_fn;
	tc_memcpy_type_n(self->objects, args, count);
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
