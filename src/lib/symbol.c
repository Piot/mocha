#include <mocha/symbol.h>

void mocha_symbol_init(mocha_symbol* self, mocha_string_hash hash)
{
	self->hash = hash;
}
