#ifndef mocha_list_h
#define mocha_list_h

#include <mocha/sequence.h>
#include <mocha/types.h>

struct mocha_object;
struct tyran_memory;

typedef struct mocha_list {
	mocha_sequence seq;
	const struct mocha_object** objects;
	size_t count;
} mocha_list;

void mocha_list_init(mocha_list* self, struct tyran_memory* object_array_memory, const struct mocha_object** const args, size_t count);

void mocha_list_deinit(mocha_list* self);

size_t mocha_list_init_prepare(mocha_list* self, struct tyran_memory* object_array_memory, size_t count);

mocha_boolean mocha_list_equal(const mocha_list* self, const mocha_list* other);

#endif
