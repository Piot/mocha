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
#include <mocha/char_query.h>
#include <mocha/context.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/parse_float.h>
#include <mocha/parser.h>
#include <mocha/runtime.h>
#include <mocha/string.h>
#include <mocha/symbol.h>
#include <stdlib.h>
#include <tiny-libc/tiny_libc.h>

static const mocha_object* parse_object(mocha_parser* self, mocha_error* error);

static size_t parse_array(mocha_parser* self, mocha_char end_char, mocha_error* error, const mocha_object* array[], size_t array_max_count)
{
	size_t count = 0;

	while (count < array_max_count) {
		mocha_char ch = mocha_char_buffer_skip_space(&self->buffer);
		if (ch == end_char) {
			return count;
		} else if (!ch) {
			MOCHA_LOG("ERROR. End while waiting for '%c' in array", end_char);
			MOCHA_ERR(mocha_error_code_unexpected_end);
		}
		mocha_char_buffer_unread_char(&self->buffer, ch);
		const mocha_object* o = parse_object(self, error);

		if (!o || error->code != 0) {
			MOCHA_LOG("Here is the problem '%c'", ch);
			return count;
		}
		array[count++] = o;
	}

	error->code = 1;
	return count;
}

static const mocha_object* parse_map(mocha_parser* self, mocha_error* error)
{
	const mocha_object* args[512];
	size_t count = parse_array(self, '}', error, args, 512);
	const mocha_object* o = mocha_values_create_map(self->values, args, count);

	return o;
}

static const mocha_object* parse_vector(mocha_parser* self, mocha_error* error)
{
	const mocha_object* args[512];
	size_t count = parse_array(self, ']', error, args, 512);
	const mocha_object* o = mocha_values_create_vector(self->values, args, count);

	return o;
}

static const mocha_object* parse_list(mocha_parser* self, mocha_error* error)
{
	const mocha_object* args[512];
	size_t count = parse_array(self, ')', error, args, 512);

	if (error->code != 0) {
		return 0;
	}

	const mocha_object* o = mocha_values_create_list(self->values, args, count);
	return o;
}

static size_t parse_until_space(mocha_parser* self, mocha_char* char_buffer, size_t max_symbol_length, mocha_error* error)
{
	mocha_string word_buffer;

	word_buffer.string = char_buffer;
	word_buffer.count = 0;

	while (1) {
		mocha_char ch = mocha_char_buffer_read_char(&self->buffer);
		if (mocha_char_is_space(ch) || !ch) {
			break;
		}

		if (word_buffer.count > 0 && mocha_char_is_separator(ch)) {
			mocha_char_buffer_unread_char(&self->buffer, ch);
			break;
		}

		if (word_buffer.count == max_symbol_length) {
			MOCHA_ERR(mocha_error_code_symbol_too_long);
		}
		word_buffer.string[word_buffer.count++] = ch;
	}

	return word_buffer.count;
}

static size_t parse_word(mocha_parser* self, mocha_char* char_buffer, size_t max_symbol_length, mocha_error* error)
{
	mocha_string word_buffer;

	word_buffer.string = char_buffer;
	word_buffer.count = 0;

	while (1) {
		mocha_char ch = mocha_char_buffer_read_char(&self->buffer);

		if (mocha_char_is_space(ch) || mocha_char_is_separator(ch) || !ch) {
			if (mocha_char_is_separator(ch)) {
				mocha_char_buffer_unread_char(&self->buffer, ch);
			}
			break;
		}

		if (word_buffer.count == max_symbol_length) {
			MOCHA_ERR(mocha_error_code_symbol_too_long);
		}
		word_buffer.string[word_buffer.count++] = ch;
	}

	return word_buffer.count;
}

static const mocha_object* parse_number(mocha_parser* self, mocha_error* error)
{
	const int max_symbol_length = 64;
	mocha_char* char_buffer = tc_malloc(sizeof(mocha_char) * max_symbol_length);
	size_t length = parse_word(self, char_buffer, max_symbol_length, error);

	if (error->code != mocha_error_code_ok) {
		MOCHA_ERROR("couldn't parse integer");
		return 0;
	}

	mocha_string word_buffer;
	word_buffer.string = char_buffer;
	word_buffer.count = length;

	const char* s = mocha_string_to_c(&word_buffer);
	const mocha_object* o;

	int result;

	if ((word_buffer.count >= 3) && (s[0] == '2' && s[1] == 'r')) {
		result = (int) strtoul(&s[2], 0, 2);
	} else if ((word_buffer.count >= 4) && (s[0] == '1' && s[1] == '6' && s[2] == 'r')) {
		result = (int) strtoul(&s[3], 0, 16);
	} else if ((word_buffer.count >= 4) && (s[0] == '0' && s[1] == 'x')) {
		result = (int) strtoul(&s[2], 0, 16);
	} else {
		result = (int) atol(s);
	}

	o = mocha_values_create_integer(self->values, result);

	tc_free(char_buffer);
	return o;
}

typedef struct character_name_lookup_item {
	const char* name;
	char value;
} character_name_lookup_item;

static char character_name_to_char(const char* s)
{
	const character_name_lookup_item items[] = {
		{"newline", 10}, {"space", 32}, {"tab", 9}, {"backspace", 8},
	};
	for (size_t i = 0; i < sizeof(items) / sizeof(items[0]); ++i) {
		const character_name_lookup_item* item = &items[i];
		if (tc_str_equal(s, item->name)) {
			return item->value;
		}
	}

	return -1;
}

static const mocha_object* parse_character(mocha_parser* self, mocha_error* error)
{
	const int max_symbol_length = 64;
	mocha_char* char_buffer = tc_malloc(sizeof(mocha_char) * max_symbol_length);
	size_t length = parse_until_space(self, char_buffer, max_symbol_length, error);

	if (error->code != mocha_error_code_ok) {
		tc_free(char_buffer);
		return 0;
	}

	mocha_char ch;

	mocha_string temp_string;
	temp_string.count = length;
	temp_string.string = char_buffer;

	if (length == 1) {
		ch = char_buffer[0];
	} else {
		if (char_buffer[0] == 'u') {
			const char* c_str = mocha_string_to_c(&temp_string);
			long int value = strtol(&c_str[1], 0, 16);
			ch = value;
		} else {
			ch = character_name_to_char(mocha_string_to_c(&temp_string));
		}
	}

	const mocha_object* o = mocha_values_create_character(self->values, ch);

	tc_free(char_buffer);

	return o;
}

static char translate_escape_char(const char escape_char)
{
	switch (escape_char) {
		case 't':
			return '\t';
		case 'b':
			return '\b';
		case '"':
			return '\"';
		case '\'':
			return '\'';
		case 'n':
			return '\n';
		case '\\':
			return '\\';
		default:
			return ' ';
	}
}

static const mocha_object* parse_string(mocha_parser* self, mocha_error* error)
{
	mocha_char temp[1024];
	size_t count = 0;
	mocha_char ch;

	while ((ch = mocha_char_buffer_read_char(&self->buffer)) != '"') {
		if (!ch) {
			MOCHA_ERR(mocha_error_code_missing_end_of_string);
		}
		if (ch == '\\') {
			mocha_char escape_char = mocha_char_buffer_read_char(&self->buffer);
			if (!escape_char) {
				MOCHA_ERR(mocha_error_code_missing_end_of_string);
			}
			ch = (mocha_char) translate_escape_char((char) escape_char);
		}
		temp[count++] = ch;
	}
	temp[count] = 0;
	const mocha_object* o = mocha_values_create_string(self->values, temp, count);

	return o;
}

static mocha_boolean valid_symbol_string(const mocha_string* string)
{
	const char* s = mocha_string_to_c(string);
	for (size_t i = 0; i < string->count; ++i) {
		char c = s[i];
		if (!((c >= 'a' && c <= 'z') || mocha_strchr("/<>=-*+", c) || ((i > 0) && (c == '?' || c == '2')))) {
			return mocha_false;
		}
	}

	return mocha_true;
}

static mocha_boolean valid_keyword_string(const mocha_string* string)
{
	const char* s = mocha_string_to_c(string);
	for (size_t i = 0; i < string->count; ++i) {
		char c = s[i];
		if (!((c >= 'a' && c <= 'z') || ((i > 0) && mocha_strchr(".-/?", c)))) {
			return mocha_false;
		}
	}

	return mocha_true;
}

static const mocha_object* create_symbol(mocha_context* context, const mocha_string* string)
{
	if (!valid_symbol_string(string)) {
		MOCHA_LOG("Illegal symbol '%s'", mocha_string_to_c(string));
		return 0;
	}
	mocha_values* values = context->values;
	const mocha_object* o = mocha_values_create_symbol(values, mocha_string_to_c(string));

	return o;
}

static const mocha_object* parse_symbol(mocha_parser* self, mocha_error* error)
{
	const mocha_object* o;

	const int max_symbol_length = 64;
	mocha_char* char_buffer = tc_malloc(sizeof(mocha_char) * max_symbol_length);

	size_t length = parse_word(self, char_buffer, max_symbol_length, error);

	if (error->code != mocha_error_code_ok) {
		tc_free(char_buffer);
		return 0;
	}

	mocha_string word_buffer;
	word_buffer.string = char_buffer;
	word_buffer.count = length;

	if (mocha_string_equal_str(&word_buffer, "true")) {
		o = mocha_values_create_boolean(self->values, mocha_true);
	} else if (mocha_string_equal_str(&word_buffer, "nil")) {
		o = mocha_values_create_nil(self->values);
	} else {
		o = create_symbol(self->context, &word_buffer);
	}

	tc_free(char_buffer);
	return o;
}

static const mocha_object* parse_keyword(mocha_parser* self, mocha_error* error)
{
	const int max_symbol_length = 64;
	mocha_char* char_buffer = tc_malloc(sizeof(mocha_char) * max_symbol_length);
	size_t length = parse_word(self, char_buffer, max_symbol_length, error);

	if (error->code != mocha_error_code_ok) {
		tc_free(char_buffer);
		return 0;
	}

	mocha_string temp_string;
	temp_string.count = length;
	temp_string.string = char_buffer;
	if (!valid_keyword_string(&temp_string)) {
		MOCHA_ERROR("Illegal keyword '%s'", mocha_string_to_c(&temp_string));
		return 0;
	}

	const mocha_object* o = mocha_values_create_keyword(self->values, mocha_string_to_c(&temp_string));
	// self->context = mocha_context_add(self->context, o, o);
	tc_free(char_buffer);
	return o;
}

static const mocha_object* parse_unquote(mocha_parser* self, mocha_error* error)
{
	const mocha_object* do_not_eval = parse_object(self, error);

	mocha_string temp_string;

	mocha_string_init_from_c(&temp_string, &self->values->string_content_memory, "unquote");
	const mocha_object* quote_symbol = create_symbol(self->context, &temp_string);

	const mocha_object* args[2];

	args[0] = quote_symbol;
	args[1] = do_not_eval;

	const mocha_object* l = mocha_values_create_list(self->values, args, 2);

	return l;
}

static const mocha_object* parse_tick(mocha_parser* self, mocha_error* error)
{
	const mocha_object* do_not_eval = parse_object(self, error);

	mocha_string temp_string;

	mocha_string_init_from_c(&temp_string, &self->values->string_content_memory, "quote");
	const mocha_object* quote_symbol = create_symbol(self->context, &temp_string);

	const mocha_object* args[2];

	args[0] = quote_symbol;
	args[1] = do_not_eval;

	const mocha_object* l = mocha_values_create_list(self->values, args, 2);

	return l;
}

static const mocha_object* parse_object(mocha_parser* self, mocha_error* error)
{
	const mocha_object* o = 0;

	mocha_char first_char = mocha_char_buffer_skip_space(&self->buffer);
	// MOCHA_LOG("First char:%c", first_char);
	switch (first_char) {
		case 0:
			MOCHA_LOG("END!");
			o = 0;
			break;
		case ':':
			o = parse_keyword(self, error);
			break;
		case '{':
			o = parse_map(self, error);
			break;
		case '[':
			o = parse_vector(self, error);
			break;
		case '(':
			o = parse_list(self, error);
			break;
		case '\'':
		case '`':
			o = parse_tick(self, error);
			break;
		case '~':
			o = parse_unquote(self, error);
			break;
		case '\"':
			o = parse_string(self, error);
			break;
		case '\\':
			o = parse_character(self, error);
			break;
		default:

			if (first_char == '-' || first_char == '+') {
				mocha_char ch = mocha_char_buffer_read_char(&self->buffer);
				mocha_char_buffer_unread_char(&self->buffer, ch);
				mocha_char_buffer_unread_char(&self->buffer, first_char);

				if (mocha_char_is_numerical(ch)) {
					o = parse_number(self, error);
				} else {
					o = parse_symbol(self, error);
				}
			} else if (mocha_char_is_numerical(first_char)) {
				mocha_char_buffer_unread_char(&self->buffer, first_char);
				o = parse_number(self, error);
			} else if (mocha_char_is_alpha(first_char)) {
				mocha_char_buffer_unread_char(&self->buffer, first_char);
				o = parse_symbol(self, error);
			} else {
				MOCHA_LOG("'%d'", first_char);
				MOCHA_ERR(mocha_error_code_unexpected_character);
			}
	}

	return o;
}

void mocha_parser_init(mocha_parser* self, mocha_values* values, mocha_context* context, const mocha_char* input, size_t input_length)
{
	mocha_char_buffer_init(&self->buffer, input, input_length);
	self->values = values;
	self->context = context;
}

const mocha_object* mocha_parser_parse(mocha_parser* self, mocha_error* error)
{
	const mocha_object* args[10 * 1024];
	size_t count = parse_array(self, 0, error, args, 10 * 1024);

	if (error->code != 0) {
		MOCHA_LOG("Parse error");
		return 0;
	}

	const mocha_object* o = mocha_values_create_list(self->values, args, count);

	return o;
}
