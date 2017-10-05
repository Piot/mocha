#include <mocha/log.h>
#include <mocha/nil.h>
#include <mocha/object.h>
#include <mocha/values.h>

#include <stdlib.h>

static size_t count_fn(const mocha_sequence* _self)
{
	return 0;
}

static const mocha_object* get_fn(const mocha_sequence* _self, struct mocha_values* values, size_t index)
{
	return mocha_values_create_nil(values);
}

static const mocha_object* get_object_fn(const mocha_sequence* _self, struct mocha_values* values, const mocha_object* key)
{
	return mocha_values_create_nil(values);
}

static void get_objects_fn(const mocha_sequence* _self, const mocha_object*** objects, size_t* count)
{
	*objects = 0;
	*count = 0;
}

void mocha_nil_init(mocha_nil* self)
{
	self->seq.count = count_fn;
	self->seq.get = get_fn;
	self->seq.get_object = get_object_fn;
	self->seq.get_objects = get_objects_fn;
}
