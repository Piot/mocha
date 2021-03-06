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
#ifndef mocha_parser_h
#define mocha_parser_h

#include <stddef.h>

#include <mocha/char_buffer.h>
#include <mocha/context.h>
#include <mocha/error.h>
#include <mocha/string.h>
#include <mocha/values.h>

typedef struct mocha_parser {
	mocha_char_buffer buffer;
	mocha_context* context;
	mocha_error error;
	mocha_values* values;
} mocha_parser;

void mocha_parser_init(mocha_parser* self, mocha_values* values, mocha_context* context, const mocha_char* input, size_t input_length);

const struct mocha_object* mocha_parser_parse(mocha_parser* self, mocha_error* error);

#endif
