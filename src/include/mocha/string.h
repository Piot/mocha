#ifndef mocha_string_h
#define mocha_string_h

#include <mocha/sequence.h>
#include <mocha/types.h>

typedef int mocha_char;
#include <string.h>
#define mocha_strchr strchr
struct tyran_memory;

typedef struct mocha_string {
	mocha_sequence seq;
	mocha_char* string;
	size_t count;

} mocha_string;

const char* mocha_string_to_c(const mocha_string* s);

const char* mocha_string_to_c_buf(const mocha_string* s, char* temp, size_t maxbuf);

mocha_boolean mocha_string_equal_str(const mocha_string* self, const char* cstr);

void mocha_string_init(mocha_string* self, struct tyran_memory* string_memory, const mocha_char* s, size_t count);

void mocha_string_init_from_c(mocha_string* self, struct tyran_memory* string_memory, const char* s);

void mocha_string_init_from_c_ex(mocha_string* self, struct tyran_memory* string_memory, const char* s, size_t count);

mocha_boolean mocha_string_equal(const mocha_string* a, const mocha_string* b);

mocha_boolean mocha_string_less(const mocha_string* a, const mocha_string* b);

size_t mocha_string_to_utf8(mocha_char c, char* b);

#endif
