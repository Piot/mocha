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
#ifndef mocha_list_h
#define mocha_list_h

#include <mocha/sequence.h>
#include <mocha/types.h>

struct mocha_object;
struct imprint_memory;

typedef struct mocha_list {
	mocha_sequence seq;
	const struct mocha_object** objects;
	size_t count;
} mocha_list;

void mocha_list_init(mocha_list* self, struct imprint_memory* object_array_memory, const struct mocha_object** const args, size_t count);

void mocha_list_deinit(mocha_list* self);

size_t mocha_list_init_prepare(mocha_list* self, struct imprint_memory* object_array_memory, size_t count);

mocha_boolean mocha_list_equal(const mocha_list* self, const mocha_list* other);

#endif
