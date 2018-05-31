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
#include <imprint/memory.h>
#include <mocha/log.h>
#include <mocha/string.h>
#include <mocha/values.h>
#include <stdlib.h>
#include <tiny-libc/tiny_libc.h>

mocha_boolean mocha_string_equal_str(const mocha_string* self, const char* cstr)
{
	const mocha_char* p = self->string;
	const char* s = cstr;

	size_t c_len = strlen(cstr);

	if (self->count != c_len) {
		return mocha_false;
	}
	for (size_t i = 0; i < c_len; ++i) {
		if (s[i] != p[i]) {
			return mocha_false;
		}
	}

	return mocha_true;
}

mocha_boolean mocha_string_equal(const mocha_string* a, const mocha_string* b)
{
	if (a->count != b->count) {
		// MOCHA_LOG("Checking str equal %p vs %p FALSE", a, b);
		return mocha_false;
	}

	for (size_t i = 0; i < a->count; ++i) {
		if (a->string[i] != b->string[i]) {
			//	MOCHA_LOG("Checking str equal %p vs %p FALSE", a, b);
			return mocha_false;
		}
	}

	// MOCHA_LOG("Checking str equal %p vs %p TRUE", a, b);
	return mocha_true;
}

mocha_boolean mocha_string_less(const mocha_string* a, const mocha_string* b)
{
	if (a->count > b->count) {
		return mocha_false;
	}

	for (size_t i = 0; i < a->count; ++i) {
		if (a->string[i] > b->string[i]) {
			return mocha_false;
		}
	}

	return mocha_true;
}

size_t mocha_string_to_utf8(mocha_char c, char* b)
{
	size_t count = 0;
	if (c < (1 << 7)) {
		*b++ = (unsigned char) (c);
		count = 1;
	} else if (c < (1 << 11)) {
		*b++ = (unsigned char) ((c >> 6) | 0xC0);
		*b++ = (unsigned char) ((c & 0x3F) | 0x80);
		count = 2;
	} else if (c < (1 << 16)) {
		*b++ = (unsigned char) (((c >> 12)) | 0xE0);
		*b++ = (unsigned char) (((c >> 6) & 0x3F) | 0x80);
		*b++ = (unsigned char) ((c & 0x3F) | 0x80);
		count = 3;
	} else if (c < (1 << 21)) {
		*b++ = (unsigned char) (((c >> 18)) | 0xF0);
		*b++ = (unsigned char) (((c >> 12) & 0x3F) | 0x80);
		*b++ = (unsigned char) (((c >> 6) & 0x3F) | 0x80);
		*b++ = (unsigned char) ((c & 0x3F) | 0x80);
		count = 4;
	}
	return count;
}

const char* mocha_string_to_c_buf(const mocha_string* s, char* temp, size_t maxbuf)
{
	size_t len = s->count;

	if (maxbuf < len + 1) {
		MOCHA_LOG("CAN NOT FILL BUFFER");
		return 0;
	}
	size_t i = 0;
	for (; i < len;) {
		size_t increment = mocha_string_to_utf8(s->string[i], &temp[i]);
		i += increment;
	}
	temp[i] = 0;

	return temp;
}

const char* mocha_string_to_c(const mocha_string* s)
{
	static char temp[512];

	return mocha_string_to_c_buf(s, temp, 512);
}

static size_t count_fn(const mocha_sequence* _self)
{
	const mocha_string* self = (const mocha_string*) _self;
	return self->count;
}

static const struct mocha_object* get_fn(const mocha_sequence* _self, struct mocha_values* values, size_t index)
{
	const mocha_string* self = (const mocha_string*) _self;
	if (index >= self->count) {
		return mocha_values_create_nil(values);
	}
	return mocha_values_create_character(values, self->string[index]);
}

static const mocha_object* get_object_fn(const mocha_sequence* _self, mocha_values* values, const mocha_object* key)
{
	const mocha_string* self = (const mocha_string*) _self;
	size_t index = (size_t) mocha_object_integer(key, "string_get_object");
	if (index >= self->count) {
		return mocha_values_create_nil(values);
	}
	return get_fn(_self, values, index);
}

static void get_objects_fn(const mocha_sequence* _self, const mocha_object*** objects, size_t* count)
{
	(void) _self;
	// const mocha_string* self = (const mocha_string*) _self;
	MOCHA_LOG("string get_objects fn: Not supported yet!");
	*objects = 0;
	*count = 0;
}

static void string_alloc(mocha_string* self, imprint_memory* string_memory, size_t count)
{
	self->string = TYRAN_MEMORY_ALLOC_TYPE_COUNT(string_memory, mocha_char, count);
	self->count = count;
	self->seq.get = get_fn;
	self->seq.get_object = get_object_fn;
	self->seq.get_objects = get_objects_fn;
	self->seq.count = count_fn;
}

void mocha_string_init(mocha_string* self, imprint_memory* string_memory, const mocha_char* s, size_t count)
{
	string_alloc(self, string_memory, count);
	tc_memcpy_type_n(self->string, s, count);
}

void mocha_string_init_from_c_ex(mocha_string* self, imprint_memory* string_memory, const char* s, size_t count)
{
	string_alloc(self, string_memory, count);
	for (size_t i = 0; i < count; ++i) {
		self->string[i] = s[i];
	}
}

void mocha_string_init_from_c(mocha_string* self, imprint_memory* string_memory, const char* s)
{
	size_t count = strlen(s);
	mocha_string_init_from_c_ex(self, string_memory, s, count);
}
