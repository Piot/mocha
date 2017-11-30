#include <mocha/fnv.h>

#include <tyran/tyran_clib.h>
#include <tyran/tyran_types.h>

static const mocha_string_hash FNV_PRIME = 16777619u;
static const mocha_string_hash OFFSET_BASIS = 2166136261u;

mocha_string_hash mocha_fnv_hash(const char* str)
{
	const size_t length = tyran_strlen(str) + 1;
	mocha_string_hash hash = OFFSET_BASIS;
	for (size_t i = 0; i < length; ++i) {
		hash ^= (uint8_t) *str++;
		hash *= FNV_PRIME;
	}
	return hash;
}
