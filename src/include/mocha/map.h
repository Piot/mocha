#ifndef mocha_map_h
#define mocha_map_h

#include <mocha/sequence.h>
#include <mocha/types.h>

struct mocha_object;
struct tyran_memory;

typedef struct mocha_map {
	mocha_sequence seq;
	const struct mocha_object** objects;
	size_t count;
} mocha_map;

void mocha_map_init(mocha_map* self, struct tyran_memory* object_array_memory, const struct mocha_object* args[], size_t count);

void mocha_map_deinit(mocha_map* self);

size_t mocha_map_init_prepare(mocha_map* self, struct tyran_memory* object_array_memory, size_t count);

const struct mocha_object* mocha_map_lookup(const mocha_map* self, const struct mocha_object* key);

mocha_boolean mocha_map_equal(const mocha_map* self, const mocha_map* other);

#endif
