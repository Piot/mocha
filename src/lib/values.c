#include <limits.h>
#include <mocha/execute_step.h>
#include <mocha/hashed_strings.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/print.h>
#include <mocha/runner.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <tyran/tyran_memory_pool.h>

#include <stdlib.h>
#include <tyran/tyran_clib.h>

static mocha_object* mocha_values_create_object(mocha_values* self, mocha_object_type object_type)
{
	mocha_object* o = TYRAN_MEMORY_CALLOC_TYPE(&self->objects, mocha_object);
	if (o == 0) {
		MOCHA_LOG("OUT OF MEMORY!");
		return 0;
	}

	// MOCHA_LOG("object pool %d", self->objects->count);
	o->type = object_type;
	o->values = self;
	o->object_type = 0;
	return o;
}

const mocha_object* mocha_values_create_boolean(mocha_values* self, mocha_boolean value)
{
	if (value) {
		return &self->true_object;
	} else {
		return &self->nil_object;
	}
}

const mocha_object* mocha_values_create_blob(mocha_values* self, const uint8_t* octets, size_t count)
{
	mocha_object* o = mocha_values_create_object(self, mocha_object_type_blob);

	mocha_blob_init(&o->data.blob, &self->blob_memory, octets, count);
	return o;
}

const mocha_object* mocha_values_create_internal_function(mocha_values* self, const struct mocha_type* type, const char* name)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_internal_function);

	value->object_type = type;
	value->debug_string = name;

	return value;
}

const mocha_object* mocha_values_create_internal_function_ex(mocha_values* self, const struct mocha_type* type, const char* name, void* user_data)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_internal_function);

	value->object_type = type;
	value->debug_string = name;
	value->user_data = user_data;

	return value;
}

const mocha_object* mocha_values_create_map(mocha_values* self, const mocha_object** args, size_t count)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_map);

	mocha_map_init(&value->data.map, &self->object_references, args, count);
	value->object_type = &self->map_def;

	return value;
}

const mocha_object* mocha_values_create_vector(mocha_values* self, const mocha_object** args, size_t count)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_vector);

	mocha_vector_init(&value->data.vector, &self->object_references, args, count);

	return value;
}

const mocha_object* mocha_values_create_list(mocha_values* self, const mocha_object** args, size_t count)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_list);

	mocha_list_init(&value->data.list, &self->object_references, args, count);

	return value;
}

const mocha_object* mocha_values_create_integer(mocha_values* self, int16_t v)
{
	if (v > SHRT_MAX || v < SHRT_MIN) {
		MOCHA_LOG("couldn't create integer %d", v);
		return 0;
	}

	mocha_object* value = mocha_values_create_object(self, mocha_object_type_integer);
	value->data.integer = v;

	return value;
}

const mocha_object* mocha_values_create_string_from_cstr(mocha_values* self, const char* s)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_string);

	mocha_string_init_from_c(&value->data.string, &self->string_content_memory, s);

	return value;
}

const mocha_object* mocha_values_create_string_from_cstr_ex(mocha_values* self, const char* s, size_t count)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_string);

	mocha_string_init_from_c_ex(&value->data.string, &self->string_content_memory, s, count);

	return value;
}

const struct mocha_object* mocha_values_create_string(mocha_values* self, const mocha_char* s, size_t count)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_string);

	mocha_string_init(&value->data.string, &self->string_content_memory, s, count);

	return value;
}

const mocha_object* mocha_values_create_function(mocha_values* self, const struct mocha_context* context, const mocha_object* name, const mocha_object* arguments, const mocha_object* body)
{
	static mocha_type fn_type;

	fn_type.eval_all_arguments = mocha_true;
	fn_type.invoke = 0;

	if (!mocha_object_is_vector(arguments)) {
		MOCHA_LOG("Need to be an argument vector for function");
		MOCHA_LOG("Illegal argument vector:%s", mocha_print_object_debug_str(arguments));
	} else {
		const mocha_vector* vec = mocha_object_vector(arguments);
		for (size_t i = 0; i < vec->count; ++i) {
			if (!mocha_object_is_symbol(vec->objects[i])) {
				MOCHA_LOG("Vector has illegal stuff in it!");
			}
		}
	}

	const mocha_symbol* symbol_name = mocha_object_symbol(name);
	const char* c_func_name = mocha_hashed_strings_lookup(self->hashed_strings, symbol_name->name_hash);

	mocha_context* context_with_own_name = mocha_context_create(context, c_func_name);
	if (context_with_own_name == 0) {
		MOCHA_LOG("Function context is null");
		return 0;
	}

	mocha_object* r = mocha_values_create_object(self, mocha_object_type_function);
	r->object_type = &fn_type;
	r->data.function.arguments = arguments;
	r->data.function.code = body;
	r->data.function.context = context_with_own_name;
	r->data.function.debug_name = c_func_name;

	// MOCHA_LOG("function %p context %p", (void*) r, (void*) context_with_own_name);

	mocha_context_add(context_with_own_name, name, r);
	// mocha_context_print_debug("function context", context_with_own_name);

	return r;
}

void mocha_values_clear(mocha_values* self)
{
	tyran_memory_clear(&self->objects);
	tyran_memory_clear(&self->object_references);
	tyran_memory_clear(&self->blob_memory);
	tyran_memory_clear(&self->string_content_memory);
}

const mocha_object* mocha_values_create_macro(mocha_values* self, const struct mocha_context* context, const mocha_object* name, const mocha_object* arguments, const mocha_object* body)
{
	static mocha_type macro_type;

	macro_type.eval_all_arguments = mocha_false;
	macro_type.invoke = 0;

	mocha_object* r = mocha_values_create_object(self, mocha_object_type_function);
	r->object_type = &macro_type;
	r->data.function.arguments = arguments;
	r->data.function.code = body;
	r->data.function.is_special = mocha_true;
	mocha_context* context_with_own_name = mocha_context_create(context, "create_macro");
	mocha_context_add(context_with_own_name, name, r);
	// mocha_context_print_debug("function context", context_with_own_name);
	r->data.function.context = context_with_own_name;
	return r;
}

const struct mocha_object* mocha_values_create_nil(mocha_values* self)
{
	return &self->nil_object;
}

MOCHA_FUNCTION(keyword_type_func)
{
	const mocha_object* argument = mocha_runner_eval(context, arguments->objects[1]);

	if (argument->type == mocha_object_type_map) {
		const mocha_object* value = mocha_map_lookup(&argument->data.map, arguments->objects[0]);

		if (value) {
			return value;
		} else {
			if (arguments->count == 3) {
				return arguments->objects[2];
			}
		}
	}

	const mocha_object* nil = mocha_values_create_nil(context->values);
	return nil;
}

MOCHA_FUNCTION(map_type_func)
{
	const mocha_object* map_self = mocha_runner_eval(context, arguments->objects[0]);
	const mocha_object* argument = mocha_runner_eval(context, arguments->objects[1]);

	if (argument->type == mocha_object_type_keyword) {
		const mocha_object* value = mocha_map_lookup(&map_self->data.map, argument);

		if (value) {
			return value;
		}
	}

	const mocha_object* nil = mocha_values_create_nil(context->values);
	return nil;
}

void mocha_values_init(mocha_values* self, mocha_hashed_strings* hashed_strings, struct tyran_memory* parent_memory, mocha_values_config config, const char* debugstring)
{
	tyran_memory* local_memory = &self->memory;
	if (!hashed_strings) {
		MOCHA_ERROR("Hashed strings");
	}
	self->hashed_strings = hashed_strings;
	size_t object_reference_size = sizeof(mocha_object*) * config.object_reference_count;
	size_t objects_size = sizeof(mocha_object) * config.objects_count;
	size_t total_memory_size = objects_size + object_reference_size + config.string_memory_size + config.blob_memory_size;
	tyran_memory_construct(local_memory, parent_memory, total_memory_size, debugstring);
	self->debug = debugstring;

	char* temp_objects_string = malloc(strlen(debugstring) + strlen("xobjects") + 2);
	temp_objects_string[0] = 0;
	strcat(temp_objects_string, debugstring);
	strcat(temp_objects_string, "/xobjects");

	tyran_memory_construct(&self->objects, local_memory, objects_size, temp_objects_string);
	char* temp = malloc(strlen(debugstring) + strlen("xobject-references") + 2);
	temp[0] = 0;
	strcat(temp, debugstring);
	strcat(temp, "/xobject-references");
	tyran_memory_construct(&self->object_references, local_memory, object_reference_size, temp);
	self->keyword_def.invoke = keyword_type_func;
	self->keyword_def.eval_all_arguments = mocha_true;

	tyran_memory_construct(&self->string_content_memory, local_memory, config.string_memory_size, "string-content");
	tyran_memory_construct(&self->blob_memory, local_memory, config.blob_memory_size, "blob-content");

	self->map_def.invoke = map_type_func;
	self->map_def.eval_all_arguments = mocha_true;

	self->nil_object.type = mocha_object_type_nil;
	self->nil_object.values = self;
	self->true_object.type = mocha_object_type_true;
	self->true_object.values = self;
}

const struct mocha_object* mocha_values_create_keyword(mocha_values* self, const char* key_c_string)
{
	mocha_string_hash hash = mocha_hashed_strings_hash_string(self->hashed_strings, key_c_string);
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_keyword);
	mocha_keyword_init(&value->data.keyword, hash);
	value->object_type = &self->keyword_def;

	return value;
}

const struct mocha_object* mocha_values_create_keyword_from_hash(mocha_values* self, mocha_string_hash hash)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_keyword);
	mocha_keyword_init(&value->data.keyword, hash);
	value->object_type = &self->keyword_def;

	return value;
}

const mocha_object* mocha_values_create_symbol(mocha_values* self, const char* key_string)
{
	mocha_string_hash total_hash = mocha_hashed_strings_hash_string(self->hashed_strings, key_string);
	const char* pointer_to_slash = tyran_str_chr(key_string + 1, '/');
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_symbol);
	if (pointer_to_slash) {
		char temp_namespace[64];
		size_t namespace_length = pointer_to_slash - key_string;
		tyran_strncpy(temp_namespace, 64, key_string, namespace_length);
		temp_namespace[namespace_length] = 0;
		mocha_string_hash namespace_hash = mocha_hashed_strings_hash_string(self->hashed_strings, temp_namespace);
		mocha_string_hash name_hash = mocha_hashed_strings_hash_string(self->hashed_strings, pointer_to_slash + 1);
		mocha_symbol_init_with_namespace(&value->data.symbol, namespace_hash, name_hash, total_hash);
	} else {
		mocha_symbol_init(&value->data.symbol, total_hash);
	}

	return value;
}

const struct mocha_object* mocha_values_create_symbol_from_hash(mocha_values* self, mocha_string_hash hash)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_symbol);
	mocha_symbol_init(&value->data.symbol, hash);

	return value;
}

const struct mocha_object* mocha_values_create_character(mocha_values* self, mocha_char ch)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_character);

	value->data.character = ch;

	return value;
}

const struct mocha_object* mocha_values_create_eval(mocha_values* self, const mocha_context* context, const mocha_object* object)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_eval);

	mocha_closure_init(&value->data.closure, context, object);
	return value;
}

const struct mocha_object* mocha_values_create_recur(mocha_values* self, const mocha_object* arguments)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_recur);
	mocha_recur* r = &value->data.recur;
	r->arguments = arguments;

	return value;
}

const struct mocha_object* mocha_values_create_context(mocha_values* self, const struct mocha_context* parent, const char* debugstring)
{
	if (parent == 0) {
		MOCHA_LOG("CAN NOT DO IT!");
	}
	if (parent->runtime == 0) {
		MOCHA_ERROR("Must have a parent runtime! to create context");
	}
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_context);
	mocha_context* context = &value->data.context;
	context->runtime = parent->runtime;

	context->name = debugstring;
	//	if (self->runtime->values == self) {
	//		MOCHA_LOG("Creating context %s values:%p object %p context:%p", debugstring, (void*) self, (void*) value, (void*) context);
	//	}
	mocha_context_init(context, self, value, parent);

	return value;
}

const struct mocha_object* mocha_values_create_sequence(mocha_values* self, mocha_object_type type, const mocha_object* args[], size_t count)
{
	switch (type) {
		case mocha_object_type_map:
			return mocha_values_create_map(self, args, count);
		case mocha_object_type_list:
			return mocha_values_create_list(self, args, count);
		case mocha_object_type_vector:
			return mocha_values_create_vector(self, args, count);
		default:
			MOCHA_LOG("Illegal create sequence! %d", type);
			return 0;
	}
}

static void print_memory(const tyran_memory* memory)
{
	tyran_memory_print_debug(memory);
}

void mocha_values_print_debug(const mocha_values* self, const char* debugstring)
{
	MOCHA_LOG("----- %p values '%s' (%s)", (void*) self, self->debug, debugstring);
	print_memory(&self->objects);
	print_memory(&self->object_references);
	print_memory(&self->blob_memory);
	print_memory(&self->string_content_memory);
	MOCHA_LOG("------------------------");
}
