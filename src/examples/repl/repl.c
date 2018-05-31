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
#include <mocha/core.h>
#include <mocha/eval.h>
#include <mocha/log.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/parser.h>
#include <mocha/print.h>
#include <mocha/runner.h>
#include <mocha/runtime.h>
#include <mocha/setup.h>

#include <stdio.h>
#include <string.h>

#include <termios.h>
#include <unistd.h>

#include <stdlib.h>

#include <breathe/breathe_app.h>

struct termios oldt, newt;

static void enable_raw_mode()
{
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_iflag &= ~(INPCK | ISTRIP | IXON); // BRKINT
	newt.c_lflag &= ~(ECHONL | ICANON | ECHO | IEXTEN);
	// newt.c_oflag &= ~(OPOST);
	newt.c_cc[VMIN] = 1;
	newt.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

static void restore_term_mode()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

static mocha_string history[100];
static size_t history_index = 0;
static size_t history_count = 0;

static int read_line(mocha_char* s, int input_length, int max_length)
{
	for (int i = 0; i < input_length; ++i) {
		putc(s[i], stdout);
	}

	fputs("\x1b[K", stdout);

	while (input_length < max_length) {
		int c_char = fgetc(stdin);

		if (c_char == EOF) {
			return EOF;
		} else if (c_char == 27) {
			int key_char = fgetc(stdin);

			if (key_char == '[') {
				key_char = fgetc(stdin);

				if (key_char == 'A') {
					for (int i = 0; i < input_length; ++i) {
						putc(8, stdout);
					}
					return -2;
				}

				if (key_char == 'B') {
					for (int i = 0; i < input_length; ++i) {
						putc(8, stdout);
					}
					return -3;
				}
			}
		} else if (c_char == 127) {
			if (input_length) {
				--input_length;
				s[input_length] = 0;
				putc(8, stdout);
				putc(32, stdout);
				putc(8, stdout);
				c_char = 0;
			} else {
				c_char = 0;
			}
		} else if (c_char == 8) {
			if (input_length) {
			}
		} else if (c_char == 10) {
			break;
		} else if (c_char == 13) {
			continue;
		} else {
			s[input_length++] = c_char;
			putc(c_char, stdout);
		}
	}

	return input_length;
}

static const mocha_object* parse_and_print(mocha_runtime* runtime, const char* input, mocha_error* error)
{
	const mocha_object* o = mocha_eval_string(runtime->root_context, input);
	MOCHA_OUTPUT("%s", mocha_print_object_debug_str(o));
	return o;
}

static void repl(mocha_runtime* runtime, mocha_parser* parser, mocha_error* error)
{
	static const int max_length = 1024;
	int input_length = 0;
	mocha_char input[max_length];

	MOCHA_OUTPUT("repl=> ");
	while (1) {
		input_length = read_line(input, input_length, max_length);

		if (input_length == EOF) {
			MOCHA_LOG("EOF");
			break;
		}

		if (input_length > 0) {
			MOCHA_LOG("");
			mocha_string* h = &history[history_count++];
			history_index = history_count;
			mocha_string_init(h, &runtime->root_context->values->string_content_memory, input, input_length);

			const mocha_object* o;
			o = parse_and_print(runtime, mocha_string_to_c(h), error);
			input_length = 0;

			if (error->code != mocha_error_code_ok) {
				mocha_error_show(error);
				mocha_error_init(error);
				MOCHA_LOG("");
			}
			MOCHA_OUTPUT("repl=> ");
		} else {
			int old_history_index = history_index;

			if (input_length == -2) {
				if (history_index != 0) {
					history_index--;
				}
			} else if (input_length == -3) {
				if (history_index < history_count) {
					history_index++;
				}
			}

			{
				mocha_string* h = &history[history_index];
				memcpy(input, h->string, sizeof(mocha_char) * h->count);
				input_length = h->count;
			}
		}
	}
}

static const mocha_object* eval_file(mocha_runtime* runtime, mocha_parser* parser, const char* filename, mocha_error* error)
{
	FILE* fp = fopen(filename, "rt");

	if (!fp) {
		MOCHA_ERR_STRING(mocha_error_code_file_not_found, filename);
	}
	const int max_buffer_count = 128 * 1024;
	char* temp_buffer = tc_malloc(max_buffer_count * sizeof(char));
	mocha_char* temp_input = tc_malloc(max_buffer_count * sizeof(mocha_char));
	int character_count = fread(temp_buffer, 1, max_buffer_count, fp);
	temp_buffer[character_count] = 0;
	for (int i = 0; i < character_count; ++i) {
		temp_input[i] = temp_buffer[i];
	}
	temp_input[character_count] = 0;

	mocha_values* permanent_values = runtime->root_context->values;
	const mocha_context* root_context = runtime->root_context;
	mocha_object* parse_context_object = mocha_values_create_context(permanent_values, root_context, "scope-root");
	mocha_context* parse_context = mocha_object_context(parse_context_object);

	runtime->root_context = parse_context;
	runtime->create_context_parent = parse_context;
	const mocha_object* o = parse_and_print(runtime, temp_buffer, error);
	tc_free(temp_input);
	tc_free(temp_buffer);

	return o;
}

int g_breathe_draw()
{
	return -1;
}

void g_breathe_init(int argc, const char* argv[], int width, int height)
{
	enable_raw_mode();

	mocha_setup setup;
	mocha_setup_init(&setup, 0);

	mocha_runtime* runtime = &setup.runtime;
	runtime->cached_values = &setup.values;

	mocha_parser parser;

	if (argc > 1) {
		const char* filename = argv[1];
		eval_file(runtime, &parser, filename, &runtime->error);
	} else {
		repl(runtime, &parser, &runtime->error);
	}

	if (runtime->error.code != mocha_error_code_ok) {
		mocha_error_show(&runtime->error);
	}
	restore_term_mode();
}
