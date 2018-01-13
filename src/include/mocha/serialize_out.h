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
