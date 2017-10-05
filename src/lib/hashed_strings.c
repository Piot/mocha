#include <tyran/tyran_clib.h>
#include <mocha/hashed_strings.h>
#include <mocha/log.h>

mocha_hashed_strings* g_hashed_strings;

void mocha_hashed_strings_init(mocha_hashed_strings* self)
{
	g_hashed_strings = self;
	self->count = 0;
	self->max_count = 1024;
	self->strings = malloc(sizeof(mocha_hashed_string) * self->max_count); // tyran_malloc_type_count(mocha_hashed_string, self->max_count);
}

static const char* internal_lookup(const mocha_hashed_strings* self, mocha_string_hash hash)
{
	for (size_t i = 0; i < self->count; ++i) {
		const mocha_hashed_string* item = &self->strings[i];
		if (item->hash == hash) {
			return item->string;
		}
	}
	return 0;
}

const char* mocha_hashed_strings_lookup(const mocha_hashed_strings* self, mocha_string_hash hash)
{
	const char* string = internal_lookup(self, hash);
	if (!string) {
		MOCHA_LOG("ERROR! Couldn't find hash '%d'", hash);
		return 0;
	}

	return string;
}

mocha_string_hash mocha_hashed_strings_hash_string(mocha_hashed_strings* self, const char* str)
{
	mocha_string_hash hash = mocha_fnv_hash(str);

	const char* previous_string = internal_lookup(self, hash);
	if (previous_string) {
		if (!tyran_str_equal(previous_string, str)) {
			MOCHA_LOG("Error. Strings collision %s and %s", previous_string, str);
			return 0;
		}
	} else {
		if (self->count >= self->max_count) {
			MOCHA_LOG("Out of hash memory!");
			return 0;
		}
		mocha_hashed_string* item = &self->strings[self->count++];
		size_t new_string_length = tyran_strlen(str);
		char* temp = malloc(sizeof(char) * new_string_length + 1); // tyran_malloc_type_count(char, new_string_length + 1);
		tyran_memcpy_type_n(temp, str, new_string_length);
		temp[new_string_length] = 0;
		item->string = temp;
		item->hash = hash;
	}
	//MOCHA_LOG("Hashing done '%s' -> %d", str, hash);
	return hash;
}
