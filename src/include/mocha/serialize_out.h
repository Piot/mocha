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
#ifndef mocha_serialize_h
#define mocha_serialize_h

#include "octet_out_stream.h"
#include <mocha/fnv.h>
#include <stddef.h>
#include <stdint.h>

struct mocha_object;
struct mocha_hashed_strings;

typedef struct keyword_lookup_entry {
	mocha_string_hash hash;
	uint8_t index;
} keyword_lookup_entry;

typedef struct keyword_lookup {
	keyword_lookup_entry entries[128];
	size_t entries_count;
} keyword_lookup;

typedef struct mocha_serialize_out {
	keyword_lookup lookup;
	mocha_octet_out_stream out_stream;
	struct mocha_hashed_strings* hashed_strings;
} mocha_serialize_out;

void mocha_serialize_out_init(mocha_serialize_out* self, const struct mocha_hashed_strings* hashed_strings);
size_t mocha_serialize_out_buf(mocha_serialize_out* self, uint8_t* buf, size_t max_count, const struct mocha_object* o);

#endif
