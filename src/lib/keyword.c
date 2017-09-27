#include <mocha/keyword.h>

void mocha_keyword_init(mocha_keyword* self, mocha_string_hash hash)
{
	self->hash = hash;
}
