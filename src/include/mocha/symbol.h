#ifndef mocha_symbol_h
#define mocha_symbol_h

#include "fnv.h"

typedef struct mocha_symbol
{
	mocha_string_hash hash;
} mocha_symbol;

void mocha_symbol_init(mocha_symbol* self, mocha_string_hash hash);

#endif
