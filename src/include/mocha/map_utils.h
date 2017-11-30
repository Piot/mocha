#ifndef mocha_map_utils_h
#define mocha_map_utils_h

#include <mocha/types.h>
#include <tyran/tyran_types.h>
#include <stddef.h>

struct mocha_object;
struct mocha_map;
struct mocha_values;
struct mocha_blob;

const struct mocha_object* mocha_map_lookup_c_string(struct mocha_values* values, const struct mocha_map* map, const char* s);
int mocha_map_lookup_c_string_int(struct mocha_values* values, const struct mocha_map* map, const char* s);
int mocha_map_lookup_c_string_int_found(struct mocha_values* values, const struct mocha_map* map, const char* s, tyran_boolean* found);

mocha_boolean mocha_map_lookup_c_string_boolean(struct mocha_values* values, const struct mocha_map* map, const char* s);
mocha_boolean mocha_map_lookup_c_string_boolean_with_default(struct mocha_values* values, const struct mocha_map* map, const char* s, mocha_boolean default_value);
const struct mocha_blob* mocha_map_lookup_c_string_blob(struct mocha_values* values, const struct mocha_map* map, const char* s);
const struct mocha_map* mocha_map_lookup_c_string_map(struct mocha_values* values, const struct mocha_map* map, const char* s);
const struct mocha_vector* mocha_map_lookup_c_string_vector(struct mocha_values* values, const struct mocha_map* map, const char* s);
const struct mocha_sequence* mocha_map_lookup_c_string_sequence(struct mocha_values* values, const struct mocha_map* map, const char* s);
const struct mocha_object* mocha_map_assoc(const struct mocha_map* map, struct mocha_values* values, const struct mocha_object** adds, size_t add_count);

#endif
