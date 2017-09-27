#include <mocha/map.h>
#include <mocha/string.h>
#include <mocha/utils.h>
#include <mocha/values.h>

const struct mocha_object* mocha_utils_map_lookup_c_string(const mocha_map* self, mocha_values* values, const char* symbol_name)
{
	const mocha_object* string_keyword = mocha_values_create_keyword(values, symbol_name);

	return mocha_map_lookup(self, string_keyword);
}

const struct mocha_object* mocha_utils_vector_index(const mocha_vector* self, size_t index, mocha_values* values)
{
	if (index < self->count) {
		return self->objects[index];
	} else {
		return mocha_values_create_nil(values);
	}
}
