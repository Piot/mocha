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
#include <mocha/hashed_strings.h>
#include <mocha/log.h>
#include <tiny-libc/tiny_libc.h>

mocha_hashed_strings* g_hashed_strings;

void mocha_hashed_strings_init(mocha_hashed_strings* self)
{
	g_hashed_strings = self;
	self->count = 0;
	self->max_count = 2048;
	self->strings = malloc(sizeof(mocha_hashed_string) * self->max_count); // tc_malloc_type_count(mocha_hashed_string, self->max_count);
}

static const char* internal_lookup(const mocha_hashed_strings* self, mocha_string_hash hash)
{
	for (size_t i = 0; i < self->count; ++i) {
		const mocha_hashed_string* item = &self->strings[i];
		if (item->hash == hash) {
			return item->string;
		}
	}
	return 0;
}

const char* mocha_hashed_strings_lookup(const mocha_hashed_strings* self, mocha_string_hash hash)
{
	const char* string = internal_lookup(self, hash);
	if (!string) {
		MOCHA_LOG("ERROR! Couldn't find hash '%d'", hash);
		return 0;
	}

	return string;
}

mocha_string_hash mocha_hashed_strings_hash_string(mocha_hashed_strings* self, const char* str)
{
	mocha_string_hash hash = mocha_fnv_hash(str);

	const char* previous_string = internal_lookup(self, hash);
	if (previous_string) {
		if (!tc_str_equal(previous_string, str)) {
			MOCHA_ERROR("Error. Strings collision %s and %s", previous_string, str);
			return 0;
		}
	} else {
		if (self->count >= self->max_count) {
			MOCHA_ERROR("Out of hash memory! %zu of %zu", self->count, self->max_count);
			return 0;
		}
		mocha_hashed_string* item = &self->strings[self->count++];
		size_t new_string_length = tc_strlen(str);
		char* temp = malloc(sizeof(char) * new_string_length + 1); // tc_malloc_type_count(char, new_string_length + 1);
		tc_memcpy_type_n(temp, str, new_string_length);
		temp[new_string_length] = 0;
		item->string = temp;
		item->hash = hash;
	}
	// MOCHA_LOG("Hashing done '%s' -> %d", str, hash);
	return hash;
}

void mocha_hashed_strings_debug(const mocha_hashed_strings* self)
{
	MOCHA_OUTPUT("hashed strings");
	MOCHA_OUTPUT("...hashed strings count:%zu max:%zu ptr:%p", self->count, self->max_count, self->strings);
}
