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
#include <mocha/object.h>
#include <mocha/sequence.h>
#include <mocha/serialize_out.h>

static void serialize_object(mocha_serialize_out* self, mocha_octet_out_stream* stream, const struct mocha_object* o);

static void write_sequence(mocha_serialize_out* self, mocha_octet_out_stream* stream, const mocha_sequence* seq)
{
	const mocha_object** objects;
	size_t count;
	mocha_sequence_get_objects(seq, &objects, &count);
	if (count > 255) {
		MOCHA_ERROR("Sequence is too long %zu. Max is 255", count);
		return;
	}
	mocha_octet_out_stream_write_uint8(stream, count);
	for (size_t i = 0; i < count; ++i) {
		serialize_object(self, stream, objects[i]);
	}
}

static void lookup_init(keyword_lookup* self)
{
	self->entries_count = 0;
}

static uint8_t lookup_find(const keyword_lookup* self, mocha_string_hash hash)
{
	for (uint8_t i = 0; i < self->entries_count; ++i) {
		if (self->entries[i].hash == hash) {
			return i;
		}
	}

	return 0xff;
}

static void lookup_add(keyword_lookup* self, mocha_string_hash hash)
{
	self->entries[self->entries_count++].hash = hash;
}

static void serialize_object(mocha_serialize_out* self, mocha_octet_out_stream* stream, const struct mocha_object* o)
{
	mocha_octet_out_stream_write_uint8(stream, o->type);
	switch (o->type) {
		case mocha_object_type_nil:
			// DO NOTHING
			break;
		case mocha_object_type_list:
		case mocha_object_type_map:
		case mocha_object_type_vector:
			write_sequence(self, stream, mocha_object_sequence(o));
			break;
		case mocha_object_type_integer:
			mocha_octet_out_stream_write_uint16(stream, o->data.integer);
			break;
		case mocha_object_type_string:
			if (o->data.string.count > 255) {
				CLOG_ERROR("Illegal length:%zu", o->data.string.count);
				return;
			}
			mocha_octet_out_stream_write_uint8(stream, o->data.string.count);
			mocha_octet_out_stream_write_string(stream, mocha_string_to_c(&o->data.string));
			break;
		case mocha_object_type_character:
			mocha_octet_out_stream_write_uint8(stream, o->data.character);
			// DO NOTHING
			break;
		case mocha_object_type_keyword: {
			mocha_string_hash hash = o->data.keyword.hash;
			uint8_t existing_index = lookup_find(&self->lookup, hash);
			if (existing_index == 0xff) {
				lookup_add(&self->lookup, hash);
				const char* keyword_string = mocha_hashed_strings_lookup(self->hashed_strings, hash);
				size_t keyword_string_length = tc_strlen(keyword_string);
				if (keyword_string_length > 127) {
					CLOG_ERROR("Illegal keyword length %zu", keyword_string_length);
					return;
				}
				uint8_t keyword_string_length_u8 = keyword_string_length;
				mocha_octet_out_stream_write_uint8(stream, 0x80 | (keyword_string_length_u8 & 0x7f));
				mocha_octet_out_stream_write_string(stream, keyword_string);
			} else {
				mocha_octet_out_stream_write_uint8(stream, existing_index & 0x7f);
			}
		}
		case mocha_object_type_true:
			// DO NOTHING
			break;
		default:
			MOCHA_ERROR("Not supported %c", o->type);
			return;
	}
}

void mocha_serialize_out_init(mocha_serialize_out* self, const mocha_hashed_strings* hashed_strings)
{
	self->hashed_strings = hashed_strings;
}

size_t mocha_serialize_out_buf(mocha_serialize_out* self, uint8_t* buf, size_t max_count, const struct mocha_object* state_object)
{
	lookup_init(&self->lookup);
	mocha_octet_out_stream_init(&self->out_stream, buf, max_count);
	serialize_object(self, &self->out_stream, state_object);

	size_t count = mocha_octet_out_stream_flush(&self->out_stream);
	return count;
}
