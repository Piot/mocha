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
#include <mocha/error.h>
#include <mocha/log.h>

static const char* error_string(const mocha_error* error, char* buffer)
{
	const char* s;

	switch (error->code) {
		case mocha_error_code_unexpected_end:
			s = "Unexpected end";
			break;
		case mocha_error_code_ok:
			s = "No error";
			break;
		case mocha_error_code_symbol_too_long:
			s = "Symbol is too long";
			break;
		case mocha_error_code_missing_end_of_string:
			s = "Missing end of string";
			break;
		case mocha_error_code_unexpected_character:
			s = "Unexpected character";
			break;
		case mocha_error_code_file_not_found:
			s = "File not found";
			break;
		case mocha_error_code_expected_list:
			s = "Expected a list";
			break;
		case mocha_error_code_not_invokable:
			s = "Not invokable";
			break;
		case mocha_error_code_fail:
			sprintf(buffer, "Failed: %s", error->string);
			s = buffer;
			break;
	}

	return s;
}

void mocha_error_show(mocha_error* self)
{
	char buffer[512];
	(void) error_string(self, buffer);
	// MOCHA_OUTPUT("Error %d '%s'\n", self->code, error_string(self, buffer));
}

void mocha_error_init(mocha_error* self)
{
	self->code = mocha_error_code_ok;
}
