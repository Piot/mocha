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
#ifndef mocha_map_h
#define mocha_map_h

#include <mocha/sequence.h>
#include <mocha/types.h>

struct mocha_object;
struct imprint_memory;

typedef struct mocha_map {
	mocha_sequence seq;
	const struct mocha_object** objects;
	size_t count;
} mocha_map;

void mocha_map_init(mocha_map* self, struct imprint_memory* object_array_memory, const struct mocha_object* args[], size_t count);

void mocha_map_deinit(mocha_map* self);

size_t mocha_map_init_prepare(mocha_map* self, struct imprint_memory* object_array_memory, size_t count);

const struct mocha_object* mocha_map_lookup(const mocha_map* self, const struct mocha_object* key);

mocha_boolean mocha_map_equal(const mocha_map* self, const mocha_map* other);

#endif
