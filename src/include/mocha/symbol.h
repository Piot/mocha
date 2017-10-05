#ifndef mocha_symbol_h
#define mocha_symbol_h

#include "fnv.h"
#include <mocha/types.h>

typedef struct mocha_symbol {
	mocha_string_hash name_hash;
	mocha_string_hash namespace_hash;
	mocha_string_hash total_hash;
	mocha_boolean has_namespace;
} mocha_symbol;

void mocha_symbol_init(mocha_symbol* self, mocha_string_hash name_hash);
void mocha_symbol_init_with_namespace(mocha_symbol* self, mocha_string_hash namespace_hash, mocha_string_hash name_hash, mocha_string_hash total_hash);

#endif
