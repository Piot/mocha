#ifndef mocha_serialize_in_h
#define mocha_serialize_in_h

#include "octet_in_stream.h"
#include <mocha/fnv.h>
#include <stddef.h>
#include <stdint.h>

struct mocha_object;
struct mocha_hashed_strings;
struct mocha_values;

typedef struct mocha_serialize_in_keyword_lookup_entry {
	const struct mocha_object* keyword_object;
} mocha_serialize_in_keyword_lookup_entry;

typedef struct mocha_serialize_in_keyword_lookup {
	mocha_serialize_in_keyword_lookup_entry entries[128];
	size_t entries_count;
} mocha_serialize_in_keyword_lookup;

typedef struct mocha_serialize_in {
	mocha_serialize_in_keyword_lookup lookup;
	mocha_octet_in_stream in_stream;
	struct mocha_hashed_strings* hashed_strings;
	struct mocha_values* values;
} mocha_serialize_in;

void mocha_serialize_in_init(mocha_serialize_in* self, const struct mocha_hashed_strings* hashed_strings, struct mocha_values* values);
const struct mocha_object* mocha_serialize_in_buf(mocha_serialize_in* self, const uint8_t* buf, size_t max_count);

#endif
