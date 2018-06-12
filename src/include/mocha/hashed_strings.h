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
#ifndef mocha_hashed_strings_h
#define mocha_hashed_strings_h

#include <stdlib.h>

#include <mocha/fnv.h>

typedef struct mocha_hashed_string {
	const char* string;
	mocha_string_hash hash;
} mocha_hashed_string;

typedef struct mocha_hashed_strings {
	mocha_hashed_string* strings;
	size_t count;
	size_t max_count;
} mocha_hashed_strings;

extern mocha_hashed_strings* g_hashed_strings;

void mocha_hashed_strings_init(mocha_hashed_strings* self);
const char* mocha_hashed_strings_lookup(const mocha_hashed_strings* self, mocha_string_hash hash);
mocha_string_hash mocha_hashed_strings_hash_string(mocha_hashed_strings* self, const char* str);
void mocha_hashed_strings_debug(const mocha_hashed_strings* self);

#endif
