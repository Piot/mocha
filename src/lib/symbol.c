#include <mocha/symbol.h>
#include <mocha/types.h>

void mocha_symbol_init(mocha_symbol* self, mocha_string_hash hash)
{
	self->name_hash = hash;
	self->namespace_hash = 0;
	self->total_hash = hash;
	self->has_namespace = mocha_false;
}

void mocha_symbol_init_with_namespace(mocha_symbol* self, mocha_string_hash namespace_hash, mocha_string_hash name_hash, mocha_string_hash total_hash)
{
	self->namespace_hash = namespace_hash;
	self->name_hash = name_hash;
	self->total_hash = total_hash;
	self->has_namespace = mocha_true;
}
