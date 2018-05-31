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
#include <mocha/char_buffer.h>
#include <mocha/char_query.h>
#include <mocha/log.h>

#include <stdlib.h>
#include <tiny-libc/tiny_libc.h>

void mocha_char_buffer_init(mocha_char_buffer* self, const mocha_char* input, size_t input_length)
{
	self->input_buffer = tc_malloc(sizeof(mocha_char) * input_length + 1);
	memcpy(self->input_buffer, input, sizeof(mocha_char) * input_length);
	self->input_buffer[input_length] = 0;

	self->input = self->input_buffer;
	self->input_end = self->input + input_length;
}

static mocha_char skip_to_eol(mocha_char_buffer* self)
{
	int ch;

	do {
		ch = mocha_char_buffer_read_char(self);
	} while (ch != 0 && !mocha_char_is_eol(ch));

	return ch;
}

mocha_char mocha_char_buffer_read_char(mocha_char_buffer* self)
{
	if (self->input > self->input_end) {
		MOCHA_LOG("ERROR: You read too far!");
		return -1;
	}

	mocha_char ch = *self->input++;

	if (ch == ';') {
		ch = skip_to_eol(self);
	}

	return ch;
}

void mocha_char_buffer_unread_char(mocha_char_buffer* self, mocha_char c)
{
	if (self->input == self->input_buffer) {
		MOCHA_LOG("ERROR: You unread too far!");
	}
	self->input--;

	if (c != *self->input) {
		MOCHA_LOG("ERROR: You unread illegal char!");
	}
}

mocha_char mocha_char_buffer_skip_space(mocha_char_buffer* self)
{
	int ch;

	do {
		ch = mocha_char_buffer_read_char(self);
	} while (ch != 0 && mocha_char_is_space(ch));

	return ch;
}
