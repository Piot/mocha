#include <mocha/context.h>
#include <mocha/hashed_strings.h>
#include <mocha/log.h>
#include <mocha/map_utils.h>
#include <mocha/object.h>
#include <mocha/print.h>
#include <mocha/values.h>
#include <stdlib.h>
#include <tyran/tyran_clib.h>
#include <tyran/tyran_memory.h>

static void mocha_context_print_debug_internal(const char* debug_text, const mocha_context* self, int tab, mocha_boolean extended)
{
	if (extended && self->parent) {
		mocha_context_print_debug_internal(debug_text, self->parent, tab + 1, extended);
	}

	MOCHA_LOG("%d >  %s", tab, mocha_print_object_debug_str(self->map_object));
	// MOCHA_LOG("   level:%d, context count:%zu", tab, self->count / 2);

	//	if (!extended && self->count > 8) {
	//		return;
	//	}
}

void mocha_context_print_debug(const char* debug_text, const mocha_context* self, mocha_boolean extended)
{
	MOCHA_LOG("    --------- CONTEXT debug: '%s' %p", debug_text, (const void*) self);
	mocha_context_print_debug_internal(debug_text, self, 0, extended);
	MOCHA_LOG("    ----------------------------");
}

const char* mocha_context_name(const mocha_context* self, char temp[], int max_size)
{
	if (self->parent) {
		mocha_context_name(self->parent, temp, max_size);
		tyran_strncat(temp, "/", max_size);
	}

	tyran_strncat(temp, self->name, max_size);

	return temp;
}

const char* mocha_context_name_static(const mocha_context* self)
{
	char temp[1024];
	return mocha_context_name(self, temp, 1024);
}

const char* mocha_context_print_debug_short(const mocha_context* self)
{
	static char temp[1024];
	char name_temp[1024];
	name_temp[0] = 0;
	tyran_snprintf(temp, 1024, "context %p %s", (const void*) self, mocha_context_name(self, name_temp, 1024));
	return temp;
}

mocha_context* mocha_context_create(const mocha_context* parent, const char* debug)
{
	if (parent == 0) {
		MOCHA_LOG("PArent context can not be null! '%s'", debug);
	}
	mocha_context* context = (mocha_context*) mocha_object_context(mocha_values_create_context(parent->values, parent, debug));
	context->script_fn = parent->script_fn;
	return context;
}

void mocha_context_import(mocha_context* target, const mocha_context* source, const mocha_keyword* prefix)
{
	/*
	if (source == target) {
		MOCHA_LOG("Can not copy to itself!!!");
		return;
	}
	const mocha_map* source_map = mocha_object_map(source->map_object);

	for (size_t i = 0; i < source_map->count; i += 2) {
		const mocha_object* key = source_map->objects[i];
		const mocha_object* value = source_map->objects[i + 1];
		const mocha_symbol* key_symbol = mocha_object_symbol(key);
		const mocha_object* prefixed_key = create_prefix(target->values, prefix, key_symbol);
		mocha_context_add(target, prefixed_key, value);
	}
 */
}

void mocha_context_add(mocha_context* self, const mocha_object* key, const mocha_object* value)
{
	// MOCHA_LOG("context add %p %s key:%s", (void*) self, mocha_context_name_static(self), mocha_print_object_debug_str(key));
	// MOCHA_LOG("context add %p %s value:%s", (void*) self, mocha_context_name_static(self), mocha_print_object_debug_str(value));

	const mocha_map* map = mocha_object_map(self->map_object);
	if (map == 0) {
		MOCHA_LOG("Yep, bad map");
		return;
	}
	if (!mocha_object_is_symbol(key)) {
		if (key->data.symbol.has_namespace) {
			MOCHA_ERROR("Can not add keys with namespace for now");
			return;
		}
		MOCHA_LOG("Must be symbol!");
		return;
	}
	if (!key) {
		MOCHA_LOG("ADD: key is null");
		return;
	}

	if (!value) {
		MOCHA_LOG("ADD: value is null");
		mocha_print_object_debug(key);
		return;
	}

	const mocha_object* adds[2];
	adds[0] = key;
	adds[1] = value;

	// MOCHA_LOG("context add %p %s", (void*) self, mocha_print_object_debug_str(key));
	// MOCHA_LOG("context value %p %s", (void*) self, mocha_print_object_debug_str(value));

	const mocha_object* new_map_object = mocha_map_assoc(map, self->map_object->values, adds, 2);
	self->map_object = new_map_object;
}

void mocha_context_add_function(mocha_context* self, mocha_values* values, const char* name, const struct mocha_type* type)
{
	const mocha_object* key = mocha_values_create_symbol(values, name);
	const mocha_object* value = mocha_values_create_internal_function(values, type, name);
	mocha_context_add(self, key, value);
}

static const mocha_object* internal_context_lookup(const mocha_context* self, const mocha_object* key)
{
	if (!key) {
		MOCHA_LOG("Can not lookup with null key!");
		return 0;
	}
	// MOCHA_LOG("internal_context_lookup");

	const mocha_map* map = mocha_object_map(self->map_object);
	const mocha_object* found_object = mocha_map_lookup(map, key);
	if (!found_object && self->parent) {
		return internal_context_lookup(self->parent, key);
	}

	return found_object;
}

void mocha_context_add_or_replace(mocha_context* self, const mocha_object* key, const mocha_object* value)
{
	mocha_context_add(self, key, value);
}

const mocha_object* mocha_context_lookup(const mocha_context* self, const mocha_object* o)
{
	const mocha_object* found_value = internal_context_lookup(self, o);

	if (found_value) {
		return found_value;
	}
	MOCHA_LOG("didn't find it... context:%p key:%s", (const void*) self, mocha_print_object_debug_str(o));
	mocha_context_print_debug("context_lookup", self, 1);
	return 0;
}

void mocha_context_init(mocha_context* self, mocha_values* values, const mocha_object* self_object, const mocha_context* parent)
{
	if (parent == 0) {
		MOCHA_LOG("context init parent ERRROR!!!!!!!!!!!!!!!!");
	}
	self->self_object = self_object;
	self->parent = parent;
	self->values = values;

	if (self->values == 0) {
		MOCHA_LOG("ERRROR!!!!!!!!!!!!!!!!");
	}
	const mocha_context* next_parent = parent;
	while (next_parent) {
		if (next_parent == self) {
			MOCHA_LOG("Something has gone very wrong!!!");
			return;
		}
		next_parent = next_parent->parent;
	}
	// MOCHA_LOG("create map object %p", (void*)self);
	self->map_object = mocha_values_create_map(self->values, 0, 0);
	// MOCHA_LOG("Create map object %p %s", (void*)self,
	// mocha_print_object_debug_str(self->map_object));
}

void mocha_context_deinit(mocha_context* self)
{
	(void) self;
	// mocha_context_print_debug("deinit", self);
}
