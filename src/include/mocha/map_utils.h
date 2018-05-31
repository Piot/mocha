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
#ifndef mocha_map_utils_h
#define mocha_map_utils_h

#include <mocha/types.h>
#include <stddef.h>
#include <tyran/tyran_types.h>

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
const struct mocha_map* mocha_map_lookup_c_string_map_allow_nil(struct mocha_values* values, const struct mocha_map* map, const char* s);
const struct mocha_vector* mocha_map_lookup_c_string_vector(struct mocha_values* values, const struct mocha_map* map, const char* s);
const struct mocha_sequence* mocha_map_lookup_c_string_sequence(struct mocha_values* values, const struct mocha_map* map, const char* s);
const struct mocha_object* mocha_map_assoc(const struct mocha_map* map, struct mocha_values* values, const struct mocha_object** adds, size_t add_count);

#endif
