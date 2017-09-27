#ifndef mocha_vector_h
#define mocha_vector_h

#include <mocha/types.h>
#include <mocha/vector.h>

struct mocha_object;
struct tyran_memory;

typedef struct mocha_vector {
	mocha_sequence seq;
	const struct mocha_object** objects;
	size_t count;
} mocha_vector;

void mocha_vector_init(mocha_vector* self, struct tyran_memory* object_array_memory, const struct mocha_object* args[], size_t count);

mocha_boolean mocha_vector_equal(const mocha_vector* self, const mocha_vector* other);

#endif
