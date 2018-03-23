#include <mocha/log.h>
#include <mocha/sequence.h>

size_t mocha_sequence_count(const mocha_sequence* self)
{
	//	MOCHA_LOG("seq:count %p", (void*) self);
	if (self->count == 0) {
		MOCHA_LOG("POINTER IS NULL!");
		return 0;
	}
	return self->count(self);
}

const struct mocha_object* mocha_sequence_get(const mocha_sequence* self, struct mocha_values* values, size_t index)
{
	// MOCHA_LOG("seq:get %p %d", (void*) self, index);
	if (self->get == 0) {
		MOCHA_LOG("POINTER IS NULL!");
		return 0;
	}
	return self->get(self, values, index);
}

const struct mocha_object* mocha_sequence_get_object(const mocha_sequence* self, struct mocha_values* values, const struct mocha_object* key)
{
	if (self->get == 0) {
		MOCHA_LOG("POINTER IS NULL!");
		return 0;
	}
	return self->get_object(self, values, key);
}

void mocha_sequence_get_objects(const mocha_sequence* self, const struct mocha_object*** objects, size_t* count)
{
	if (self->get == 0) {
		MOCHA_LOG("POINTER IS NULL!");
		return;
	}
	self->get_objects(self, objects, count);
}
