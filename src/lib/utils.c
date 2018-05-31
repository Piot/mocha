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
#include <mocha/map.h>
#include <mocha/string.h>
#include <mocha/utils.h>
#include <mocha/values.h>

const struct mocha_object* mocha_utils_map_lookup_c_string(const mocha_map* self, mocha_values* values, const char* symbol_name)
{
	const mocha_object* string_keyword = mocha_values_create_keyword(values, symbol_name);

	return mocha_map_lookup(self, string_keyword);
}

const struct mocha_object* mocha_utils_vector_index(const mocha_vector* self, size_t index, mocha_values* values)
{
	if (index < self->count) {
		return self->objects[index];
	} else {
		return mocha_values_create_nil(values);
	}
}
