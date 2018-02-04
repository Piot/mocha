#include <mocha/hashed_strings.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/print.h>
#include <mocha/sequence.h>
#include <mocha/serialize_in.h>
#include <mocha/values.h>

static void lookup_init(mocha_serialize_in_keyword_lookup* self)
{
	self->entries_count = 0;
}

static const struct mocha_object* lookup_find(const mocha_serialize_in_keyword_lookup* self, uint8_t index)
{
	return self->entries[index].keyword_object;
}

static void lookup_add(mocha_serialize_in_keyword_lookup* self, const struct mocha_object* keyword_object)
{
	self->entries[self->entries_count++].keyword_object = keyword_object;
}

static const mocha_object* serialize_object(mocha_serialize_in* self, mocha_octet_in_stream* stream);

static const mocha_object* read_sequence(mocha_serialize_in* self, mocha_octet_in_stream* stream, mocha_object_type create_type)
{
	uint8_t count = mocha_octet_in_stream_read_uint8(stream);
	const struct mocha_object* objects[256];
	for (size_t i = 0; i < count; ++i) {
		objects[i] = serialize_object(self, stream);
	}

	const struct mocha_object* seq_object = mocha_values_create_sequence(self->values, create_type, objects, count);

	return seq_object;
}

static const mocha_object* serialize_object(mocha_serialize_in* self, mocha_octet_in_stream* stream)
{
	const mocha_object* o = 0;

	mocha_object_type type_to_create = (mocha_object_type) mocha_octet_in_stream_read_uint8(stream);
	switch (type_to_create) {
		case mocha_object_type_nil:
			o = mocha_values_create_nil(self->values);
			break;
		case mocha_object_type_list:
		case mocha_object_type_map:
		case mocha_object_type_vector:
			o = read_sequence(self, stream, type_to_create);
			break;
		case mocha_object_type_integer: {
			uint16_t int_value = mocha_octet_in_stream_read_uint16(stream);
			o = mocha_values_create_integer(self->values, int_value);
		} break;
		case mocha_object_type_string: {
			uint8_t string_length = mocha_octet_in_stream_read_uint8(stream);
			char temp_buf[256];
			mocha_octet_in_stream_read_string(stream, temp_buf, string_length);
			o = mocha_values_create_string_from_cstr_ex(self->values, temp_buf, string_length);
		} break;
		case mocha_object_type_character: {
			uint8_t ch_value = mocha_octet_in_stream_read_uint8(stream);
			o = mocha_values_create_character(self->values, (mocha_char) ch_value);
		} break;
		case mocha_object_type_keyword: {
			uint8_t index = mocha_octet_in_stream_read_uint8(stream);
			if (index & 0x80) {
				uint8_t string_length = index & 0x7f;
				char temp_buf[127];
				mocha_octet_in_stream_read_string(stream, temp_buf, string_length);
				const struct mocha_object* keyword_object = mocha_values_create_keyword(self->values, temp_buf);
				lookup_add(&self->lookup, keyword_object);
				o = keyword_object;
			} else {
				const struct mocha_object* keyword_object = lookup_find(&self->lookup, index);
				o = keyword_object;
			}
		} break;
		case mocha_object_type_true:
			o = mocha_values_create_boolean(self->values, TYRAN_TRUE);
			break;
		default:
			MOCHA_ERROR("Not supported %d", o->type);
			return 0;
	}

	return o;
}

void mocha_serialize_in_init(mocha_serialize_in* self, const struct mocha_hashed_strings* hashed_strings, struct mocha_values* values)
{
	lookup_init(&self->lookup);
	self->values = values;
	self->hashed_strings = hashed_strings;
}

const struct mocha_object* mocha_serialize_in_buf(mocha_serialize_in* self, const uint8_t* buf, size_t length)
{
	mocha_octet_in_stream_init(&self->in_stream, buf, length);
	const struct mocha_object* state_object = serialize_object(self, &self->in_stream);

	return state_object;
}
