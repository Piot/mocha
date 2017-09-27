#ifndef mocha_hashed_strings_h
#define mocha_hashed_strings_h

#include <stdlib.h>

#include <mocha/fnv.h>


typedef struct mocha_hashed_string {
	const char* string;
	mocha_string_hash hash;
} mocha_hashed_string;

typedef struct mocha_hashed_strings {
	mocha_hashed_string* strings;
	size_t count;
	size_t max_count;
} mocha_hashed_strings;

extern mocha_hashed_strings* g_hashed_strings;

void mocha_hashed_strings_init(mocha_hashed_strings* self);
const char* mocha_hashed_strings_lookup(const mocha_hashed_strings* self, mocha_string_hash hash);
mocha_string_hash mocha_hashed_strings_hash_string(mocha_hashed_strings* self, const char* str);

#endif
