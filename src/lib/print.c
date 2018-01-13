#include <mocha/hashed_strings.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/print.h>
#include <mocha/string.h>
#include <mocha/values.h>
#include <tyran/tyran_memory.h>

#include <stdio.h>
#include <stdlib.h>

extern mocha_hashed_strings* g_hashed_strings;

typedef struct string_stream {
	char* p;
	char* buffer;
	size_t buffer_size;
} string_stream;

static void string_stream_rewind(string_stream* self)
{
	self->p = self->buffer;
}

static void string_stream_init_ex(string_stream* self, char* buffer, size_t buf_size)
{
	self->buffer = buffer;
	self->buffer_size = buf_size;
	string_stream_rewind(self);
}

static size_t string_stream_count(const string_stream* self)
{
	return self->p - self->buffer;
}

static void string_stream_init(tyran_memory* memory, string_stream* self, size_t buf_size)
{
	self->buffer_size = buf_size;
	self->buffer = TYRAN_MEMORY_ALLOC_TYPE_COUNT(memory, char, buf_size);
	if (self->buffer == 0) {
		puts("COULDN*T ALLLOCATE");
	}
	string_stream_rewind(self);
}

static void string_stream_output(string_stream* self, const char* buf)
{
	size_t length = strlen(buf);
	size_t offset = (size_t)(self->p - self->buffer);
	// MOCHA_LOG("[%d %s]", offset, buf);
	if (offset + length >= self->buffer_size) {
		MOCHA_ERROR("Not enough memory for output stream!");
		return;
	}
	memcpy(self->p, buf, sizeof(char) * length);
	self->p += length;
}

static void string_stream_close(string_stream* self)
{
	*self->p = 0;
}

static void string_stream_destroy(string_stream* self)
{
	(void) self;
	// TYRAN_MEMORY_FREE(self->memory, self->buffer);
}

static void print_object_debug(string_stream* f, const mocha_object* o, mocha_boolean show_quotes, mocha_boolean compress, int depth);

static void print_array_debug(string_stream* f, const mocha_object* objects[], size_t count, mocha_boolean compress, int depth)
{
	const size_t threshold = 32;
	for (size_t i = 0; i < (count > threshold ? threshold : count); ++i) {
		const mocha_object* o = objects[i];
		print_object_debug(f, o, mocha_true, TYRAN_FALSE, depth);

		if (i != count - 1) {
			string_stream_output(f, " ");
		}
	}
}

static void print_map_array_debug(string_stream* f, const mocha_object* objects[], size_t count, mocha_boolean compress, int depth)
{
	const size_t threshold = 32;
	for (size_t i = 0; i < (count > threshold ? threshold : count); ++i) {
		const mocha_object* o = objects[i];
		print_object_debug(f, o, mocha_true, TYRAN_FALSE, depth);

		if (i != count - 1) {
			if (!compress && ((i % 2) != 0)) {
				string_stream_output(f, ", ");
			} else {
				string_stream_output(f, " ");
			}
		}
	}
}

static const char* hash_to_string(mocha_string_hash hash)
{
	return mocha_hashed_strings_lookup(g_hashed_strings, hash);
}

static mocha_boolean is_printable_character(mocha_char ch)
{
	return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (mocha_strchr("*+-><=./?", ch) != 0);
}

typedef struct character_name_lookup_item {
	const char* name;
	char value;
} character_name_lookup_item;

static const char* char_to_character_name(char ch)
{
	const character_name_lookup_item items[] = {
		{"newline", 10}, {"space", 32}, {"tab", 9}, {"backspace", 8},
	};
	for (size_t i = 0; i < sizeof(items) / sizeof(items[0]); ++i) {
		const character_name_lookup_item* item = &items[i];
		if (item->value == ch) {
			return item->name;
		}
	}

	return 0;
}

void print_object_debug(string_stream* f, const mocha_object* o, mocha_boolean show_quotes, mocha_boolean compress, int depth)
{
	char buf[512];

	if (depth > 4) {
		return;
	}

	if (o == 0) {
		string_stream_output(f, "NULL illegal!");
		return;
	}

	// snprintf(buf, 256, "type %d ", o->type);
	// string_stream_output(f, buf);

	switch (o->type) {
		case mocha_object_type_nil:
			string_stream_output(f, "nil");
			break;
		case mocha_object_type_blob:
			snprintf(buf, 256, "blob %zu", o->data.blob.count);
			string_stream_output(f, buf);
			break;
		case mocha_object_type_list:
			string_stream_output(f, "(");
			print_array_debug(f, o->data.list.objects, o->data.list.count, compress, depth + 1);
			string_stream_output(f, ")");
			break;
		case mocha_object_type_vector:
			string_stream_output(f, "[");
			print_array_debug(f, o->data.vector.objects, o->data.vector.count, compress, depth + 1);
			string_stream_output(f, "]");
			break;
		case mocha_object_type_map:
			string_stream_output(f, "{");
			print_map_array_debug(f, o->data.map.objects, o->data.map.count, compress, depth + 1);
			string_stream_output(f, "}");
			break;
		case mocha_object_type_integer:
			snprintf(buf, 256, "%d", o->data.integer);
			string_stream_output(f, buf);
			break;
		case mocha_object_type_string:
			if (show_quotes) {
				snprintf(buf, 256, "\"%s\"", mocha_string_to_c(&o->data.string));
			} else {
				snprintf(buf, 256, "%s", mocha_string_to_c(&o->data.string));
			}
			string_stream_output(f, buf);
			break;
		case mocha_object_type_keyword:
			snprintf(buf, 256, ":%s", hash_to_string(o->data.keyword.hash));
			string_stream_output(f, buf);
			break;
		case mocha_object_type_true:
			string_stream_output(f, "true");
			break;
		case mocha_object_type_symbol:
			if (o->data.symbol.has_namespace) {
				snprintf(buf, 256, "%s_/_%s", hash_to_string(o->data.symbol.namespace_hash), hash_to_string(o->data.symbol.name_hash));
			} else {
				snprintf(buf, 256, "%s", hash_to_string(o->data.symbol.name_hash));
			}
			string_stream_output(f, buf);
			break;
		case mocha_object_type_function:
			snprintf(buf, 256, "fn: '%s'", o->data.function.debug_name);
			string_stream_output(f, buf);
			break;
		case mocha_object_type_internal_function:
			// string_stream_output(f, "internalfn");
			snprintf(buf, 256, "cfn: '%s'", o->debug_string);
			string_stream_output(f, buf);
			break;
		case mocha_object_type_context:
			snprintf(buf, 256, "context: '%s'", o->debug_string);
			string_stream_output(f, buf);
			break;

		case mocha_object_type_character: {
			mocha_char ch = o->data.character;
			if (!show_quotes) {
				char utf8_buf[8];
				size_t count = mocha_string_to_utf8(ch, utf8_buf);
				utf8_buf[count] = 0;
				snprintf(buf, 256, utf8_buf);
			} else {
				if (is_printable_character((char) ch)) {
					snprintf(buf, 256, "\\%c", (char) ch);
				} else {
					const char* lookup_name = char_to_character_name((char) ch);
					if (lookup_name) {
						snprintf(buf, 256, "\\%s", lookup_name);
					} else {
						snprintf(buf, 256, "\\u%x", ch);
					}
				}
			}
			string_stream_output(f, buf);
		} break;
		case mocha_object_type_recur:
			snprintf(buf, 256, "recur: '%s'", o->debug_string);
			string_stream_output(f, buf);
			break;
		case mocha_object_type_eval:
			snprintf(buf, 256, "eval: '%s'", o->debug_string);
			string_stream_output(f, buf);
			break;
	}
}

static void internal_print(const mocha_object* o, mocha_boolean show_quotes)
{
	string_stream stream;

	string_stream_init(&o->values->string_content_memory, &stream, 1 * 1024);
	print_object_debug(&stream, o, show_quotes, TYRAN_FALSE, 0);
	string_stream_close(&stream);
	MOCHA_LOG("%s", stream.buffer);
	string_stream_destroy(&stream);
}

const char* mocha_print_object_debug_str(const mocha_object* o)
{
	static string_stream stream;

	if (!stream.buffer) {
		string_stream_init(&o->values->string_content_memory, &stream, 1 * 1024);
	} else {
		string_stream_rewind(&stream);
	}
	const mocha_boolean show_quotes = mocha_true;
	print_object_debug(&stream, o, show_quotes, TYRAN_FALSE, 0);
	string_stream_close(&stream);

	return stream.buffer;
}

size_t mocha_print_object_edn_buf(char* buf, size_t max_len, const mocha_object* o)
{
	string_stream stream;

	string_stream_init_ex(&stream, buf, max_len);
	const mocha_boolean show_quotes = mocha_true;
	print_object_debug(&stream, o, show_quotes, TYRAN_TRUE, 0);
	string_stream_close(&stream);

	return string_stream_count(&stream);
}

const char* mocha_print_object_debug_str_pure(const mocha_object* o)
{
	static string_stream stream;

	if (!stream.buffer) {
		string_stream_init(&o->values->string_content_memory, &stream, 1 * 1024);
	} else {
		string_stream_rewind(&stream);
	}
	const mocha_boolean show_quotes = mocha_false;
	print_object_debug(&stream, o, show_quotes, TYRAN_FALSE, 0);
	string_stream_close(&stream);

	return stream.buffer;
}

const char* mocha_print_list_debug_str(mocha_values* values, const struct mocha_list* l)
{
	static string_stream stream;

	if (!stream.buffer) {
		string_stream_init(&values->string_content_memory, &stream, 1 * 1024);
	} else {
		string_stream_rewind(&stream);
	}

	print_array_debug(&stream, l->objects, l->count, TYRAN_FALSE, 0);
	string_stream_close(&stream);

	return stream.buffer;
}

const char* mocha_print_map_debug_str(mocha_values* values, const struct mocha_map* l)
{
	static string_stream stream;

	if (!stream.buffer) {
		string_stream_init(&values->string_content_memory, &stream, 1 * 1024);
	} else {
		string_stream_rewind(&stream);
	}

	print_map_array_debug(&stream, l->objects, l->count, TYRAN_FALSE, 0);
	string_stream_close(&stream);

	return stream.buffer;
}

void mocha_print_object_debug(const mocha_object* o)
{
	internal_print(o, mocha_true);
}

void mocha_print_object_debug_no_quotes(const mocha_object* o)
{
	internal_print(o, mocha_false);
}
