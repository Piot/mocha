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
#ifndef mocha_char_buffer_h
#define mocha_char_buffer_h

#include <mocha/string.h>

typedef struct mocha_char_buffer_cursor {
	int line;
	int column;
} mocha_char_buffer_cursor;

typedef struct mocha_char_buffer {
	const mocha_char* input;
	const mocha_char* input_end;
	mocha_char* input_buffer;
	mocha_char_buffer_cursor cursor;
	mocha_char_buffer_cursor old_cursor;
} mocha_char_buffer;

void mocha_char_buffer_init(mocha_char_buffer* self, const mocha_char* input, size_t input_length);

mocha_char mocha_char_buffer_read_char(mocha_char_buffer* self);

void mocha_char_buffer_unread_char(mocha_char_buffer* self, mocha_char c);

mocha_char mocha_char_buffer_skip_space(mocha_char_buffer* self);

#endif
