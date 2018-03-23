#ifndef mocha_sequence_h
#define mocha_sequence_h

#include <stdlib.h>

struct mocha_values;
struct mocha_sequence;
struct mocha_object;

typedef size_t (*sequence_count_fn)(const struct mocha_sequence* self);
typedef const struct mocha_object* (*sequence_get_fn)(const struct mocha_sequence* self, struct mocha_values* values, size_t index);
typedef const struct mocha_object* (*sequence_get_object_fn)(const struct mocha_sequence* self, struct mocha_values* values, const struct mocha_object*);
typedef void (*sequence_get_objects_fn)(const struct mocha_sequence* self, const struct mocha_object*** objects, size_t* count);

typedef struct mocha_sequence {
	sequence_count_fn count;
	sequence_get_fn get;
	sequence_get_object_fn get_object;
	sequence_get_objects_fn get_objects;
} mocha_sequence;

size_t mocha_sequence_count(const mocha_sequence* self);
const struct mocha_object* mocha_sequence_get(const mocha_sequence* self, struct mocha_values* values, size_t index);
const struct mocha_object* mocha_sequence_get_object(const mocha_sequence* self, struct mocha_values* values, const struct mocha_object* key);
void mocha_sequence_get_objects(const mocha_sequence* self, const struct mocha_object*** objects, size_t* count);

#endif
