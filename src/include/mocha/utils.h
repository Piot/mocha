#ifndef mocha_utils_h
#define mocha_utils_h

#include <mocha/string.h>
#include <mocha/types.h>

struct mocha_object;
struct mocha_values;
struct mocha_map;
struct mocha_vector;

const struct mocha_object* mocha_utils_map_lookup_c_string(const struct mocha_map* self, struct mocha_values* values, const char* symbol_name);

const struct mocha_object* mocha_utils_vector_index(const struct mocha_vector* self, size_t index, struct mocha_values* values);

#endif
