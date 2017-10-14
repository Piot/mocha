#include <mocha/core.h>
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

#include <breath/breath_app.h>

static void init_ncurses()
{
	struct termios oldt, newt;

	/*tcgetattr gets the parameters of the current terminal
	   STDIN_FILENO will tell tcgetattr that it should write the settings
	   of stdin to oldt*/
	tcgetattr(STDIN_FILENO, &oldt);
	/*now the settings will be copied*/
	newt = oldt;

	/*ICANON normally takes care that one line at a time will be processed
	   that means it will return if it sees a "\n" or an EOF or an EOL*/
	newt.c_lflag &= ~(ICANON | ECHO);

	/*Those new settings will be set to STDIN
	   TCSANOW tells tcsetattr to change attributes immediately. */
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

static void close_ncurses()
{
	struct termios oldt, newt;

	/*tcgetattr gets the parameters of the current terminal
	   STDIN_FILENO will tell tcgetattr that it should write the settings
	   of stdin to oldt*/
	tcgetattr(STDIN_FILENO, &oldt);
	/*now the settings will be copied*/
	newt = oldt;

	/*ICANON normally takes care that one line at a time will be processed
	   that means it will return if it sees a "\n" or an EOF or an EOL*/
	newt.c_lflag |= (ICANON | ECHO);

	/*Those new settings will be set to STDIN
	   TCSANOW tells tcsetattr to change attributes immediately. */
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
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

static const mocha_object* parse_and_print(mocha_runtime* runtime, mocha_parser* parser, mocha_boolean print_only_last, mocha_error* error)
{
	const mocha_object* o = mocha_parser_parse(parser, error);

	if (o && o->type == mocha_object_type_list) {
		const mocha_list* list = &o->data.list;
		mocha_boolean printed_before = mocha_false;
		for (size_t i = 0; i < list->count; ++i) {
			const mocha_object* r = mocha_runtime_eval(runtime, list->objects[i], error);
			if (printed_before) {
				MOCHA_LOG(" ");
			}
			MOCHA_LOG("result:%s", mocha_print_object_debug_str(r));
			printed_before = mocha_true;
		}

		if (list->count > 0) {
			MOCHA_LOG("");
		}
	} else {
		MOCHA_LOG("NULL");
		o = 0;
	}

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
			mocha_string_init(h, &runtime->values->string_content_memory, input, input_length);

			mocha_parser_init(parser, runtime->values, runtime->root_context, input, input_length);
			input_length = 0;
			const mocha_object* o;
			o = parse_and_print(runtime, parser, mocha_false, error);

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
	char* temp_buffer = tyran_malloc(max_buffer_count * sizeof(char));
	mocha_char* temp_input = tyran_malloc(max_buffer_count * sizeof(mocha_char));
	int character_count = fread(temp_buffer, 1, max_buffer_count, fp);
	for (int i = 0; i < character_count; ++i) {
		temp_input[i] = temp_buffer[i];
	}
	temp_input[character_count] = 0;
	mocha_parser_init(parser, runtime->values, runtime->root_context, temp_input, character_count);
	const mocha_object* o = parse_and_print(runtime, parser, mocha_true, error);
	tyran_free(temp_input);
	tyran_free(temp_buffer);

	return o;
}

int g_breath_draw()
{
	return -1;
}

void g_breath_init(int argc, const char* argv[], int width, int height)
{
	init_ncurses();

	mocha_setup setup;
	mocha_setup_init(&setup);

	mocha_runtime* runtime = &setup.runtime;

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
	close_ncurses();
}
