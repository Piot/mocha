#ifndef mocha_keyword_h
#define mocha_keyword_h

#include <stdlib.h>

#include "fnv.h"

typedef struct mocha_keyword {
	mocha_string_hash hash;
} mocha_keyword;

void mocha_keyword_init(mocha_keyword* self, mocha_string_hash hash);

#endif
