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
#ifndef mocha_string_h
#define mocha_string_h

#include <mocha/sequence.h>
#include <mocha/types.h>

typedef int mocha_char;
#include <string.h>
#define mocha_strchr strchr
struct imprint_memory;

typedef struct mocha_string {
	mocha_sequence seq;
	mocha_char* string;
	size_t count;

} mocha_string;

const char* mocha_string_to_c(const mocha_string* s);

const char* mocha_string_to_c_buf(const mocha_string* s, char* temp, size_t maxbuf);

mocha_boolean mocha_string_equal_str(const mocha_string* self, const char* cstr);

void mocha_string_init(mocha_string* self, struct imprint_memory* string_memory, const mocha_char* s, size_t count);

void mocha_string_init_from_c(mocha_string* self, struct imprint_memory* string_memory, const char* s);

void mocha_string_init_from_c_ex(mocha_string* self, struct imprint_memory* string_memory, const char* s, size_t count);

mocha_boolean mocha_string_equal(const mocha_string* a, const mocha_string* b);

mocha_boolean mocha_string_less(const mocha_string* a, const mocha_string* b);

size_t mocha_string_to_utf8(mocha_char c, char* b);

#endif
