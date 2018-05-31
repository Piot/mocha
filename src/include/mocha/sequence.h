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
