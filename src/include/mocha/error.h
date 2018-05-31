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
#ifndef mocha_error_h
#define mocha_error_h

#define MOCHA_ERR(err_number)                                                                                                                                                                                                                                  \
	error->code = err_number;                                                                                                                                                                                                                                  \
	return 0;
#define MOCHA_ERR_STRING(err_number, err_string)                                                                                                                                                                                                               \
	error->code = err_number;                                                                                                                                                                                                                                  \
	error->string = err_string;                                                                                                                                                                                                                                \
	return 0;

typedef enum mocha_error_code {
	mocha_error_code_ok = 0,
	mocha_error_code_unexpected_end,
	mocha_error_code_symbol_too_long,
	mocha_error_code_missing_end_of_string,
	mocha_error_code_unexpected_character,
	mocha_error_code_file_not_found,
	mocha_error_code_fail,
	mocha_error_code_expected_list,
	mocha_error_code_not_invokable
} mocha_error_code;

typedef struct mocha_error {
	int line_number;
	const char* filename;
	mocha_error_code code;
	const char* string;
} mocha_error;

void mocha_error_show(mocha_error* self);

void mocha_error_init(mocha_error* self);

#endif
