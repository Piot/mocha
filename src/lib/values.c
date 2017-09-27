#include <mocha/hashed_strings.h>
#include <mocha/log.h>
#include <mocha/object.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/type.h>
#include <tyran/tyran_memory_pool.h>
#include <mocha/utils.h>
#include <mocha/values.h>

#include <tyran/tyran_clib.h>
#include <stdlib.h>

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

const mocha_object* mocha_values_create_integer(mocha_values* self, int v)
{
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

	mocha_context* context_with_own_name = mocha_context_create(context, "create function");
	if (context_with_own_name == 0) {
		MOCHA_LOG("Function context is null");
		return 0;
	}

	mocha_object* r = mocha_values_create_object(self, mocha_object_type_function);
	r->object_type = &fn_type;
	r->data.function.arguments = arguments;
	r->data.function.code = body;
	r->data.function.context = context_with_own_name;

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

static const mocha_object* copy_object(mocha_values* self, const mocha_object* o);

static void copy_array(mocha_values* self, const mocha_object** target, size_t target_count, const mocha_object** source, size_t source_count)
{
	if (source_count > target_count) {
		MOCHA_LOG("Error: Buffer is too small!");
		return;
	}
	// MOCHA_LOG("*** Copy array count %d", source_count);
	for (size_t i = 0; i < source_count; ++i) {
		// MOCHA_LOG("Copy array %p index %d", (void*)source, i);
		target[i] = copy_object(self, source[i]);
		// MOCHA_LOG("Target:%d type:%d %s", i, target[i]->type, mocha_print_object_debug_str(target[i]));
		// MOCHA_LOG("Source:%d type:%d %s", i, source[i]->type, mocha_print_object_debug_str(source[i]));
	}
	// MOCHA_LOG("*** Copy array done! %d", source_count);
}

static const mocha_object* copy_object(mocha_values* self, const mocha_object* o)
{
#define TEMP_BUF_SIZE 256
	const mocha_object* temp_buf[TEMP_BUF_SIZE];

	if (o == 0) {
		return 0;
	}
	// MOCHA_LOG("Copy %s", mocha_print_object_debug_str(o));
	switch (o->type) {
		case mocha_object_type_nil:
			return mocha_values_create_nil(self);
		case mocha_object_type_list: {
			const mocha_list* list = mocha_object_list(o);
			copy_array(self, temp_buf, TEMP_BUF_SIZE, list->objects, list->count);
			const mocha_object* after_list_object = mocha_values_create_list(self, temp_buf, list->count);
			return after_list_object;
		}
		case mocha_object_type_map: {
			const mocha_map* map = mocha_object_map(o);
			copy_array(self, temp_buf, TEMP_BUF_SIZE, map->objects, map->count);
			const mocha_object* after_map_object = mocha_values_create_map(self, temp_buf, map->count);
			return after_map_object;
		}
		case mocha_object_type_vector: {
			const mocha_vector* vec = mocha_object_vector(o);
			copy_array(self, temp_buf, TEMP_BUF_SIZE, vec->objects, vec->count);
			return mocha_values_create_vector(self, temp_buf, vec->count);
		}
		case mocha_object_type_integer: {
			int v = mocha_object_integer(o, "clone");
			return mocha_values_create_integer(self, v);
		}
		case mocha_object_type_string: {
			const mocha_string* str = mocha_object_string(o);
			return mocha_values_create_string(self, str->string, str->count);
		}
		case mocha_object_type_character: {
			mocha_char chr = mocha_object_character(o);
			return mocha_values_create_character(self, chr);
		}
		case mocha_object_type_keyword: {
			const mocha_keyword* keyword = mocha_object_keyword(o, "copy_object_keyword");
			return mocha_values_create_keyword_from_hash(self, keyword->hash);
		}
		case mocha_object_type_true: {
			return mocha_values_create_boolean(self, mocha_true);
		}
		case mocha_object_type_symbol: {
			MOCHA_LOG("Error can not copy symbols!");
			return 0;
		}
		case mocha_object_type_function: {
			MOCHA_LOG("Error can not copy functions!");
			return 0;
		}
		case mocha_object_type_internal_function:
			MOCHA_LOG("Error can not copy internal functions!");
			return 0;
		case mocha_object_type_blob: {
			MOCHA_LOG("Error can not copy blobs!");
			return 0;
		}
		case mocha_object_type_closure: {
			MOCHA_LOG("Error can not copy closures!");
			return 0;
		}
		case mocha_object_type_context: {
			MOCHA_LOG("Error can not copy contexts!");
			return 0;
		}
	}

	MOCHA_LOG("ERROR! DID NOT COPY!");
	return 0;
}

static const mocha_object* copy_object_and_children(mocha_values* self, const mocha_object* o)
{
	return copy_object(self, o);
}

const mocha_object* mocha_values_deep_copy(mocha_values* self, const mocha_object* object_to_copy)
{
	// MOCHA_LOG("deep copy: '%s'", mocha_print_object_debug_str(object_to_copy));
	const mocha_object* clone = copy_object_and_children(self, object_to_copy);
	// MOCHA_LOG("deep copy clone: '%s'", mocha_print_object_debug_str(clone));

	return clone;
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
	const mocha_object* argument = arguments->objects[1];

	if (argument->type == mocha_object_type_map) {
		const mocha_object* value = mocha_map_lookup(&argument->data.map, arguments->objects[0]);

		if (value) {
			MOCHA_RESULT_VALUE(result_callback, value);
			return;
		} else {
			if (arguments->count == 3) {
				MOCHA_RESULT_VALUE(result_callback, arguments->objects[2]);
				return;
			}
		}
	}

	const mocha_object* nil = mocha_values_create_nil(context->values);
	MOCHA_RESULT_VALUE(result_callback, nil);
}

MOCHA_FUNCTION(map_type_func)
{
	const mocha_object* argument = arguments->objects[1];
	const mocha_object* map_self = arguments->objects[0];

	if (argument->type == mocha_object_type_keyword) {
		const mocha_object* value = mocha_map_lookup(&map_self->data.map, argument);

		if (value) {
			MOCHA_RESULT_VALUE(result_callback, value);
			return;
		}
	}

	const mocha_object* nil = mocha_values_create_nil(context->values);
	MOCHA_RESULT_VALUE(result_callback, nil);
}

void mocha_values_init(mocha_values* self, mocha_hashed_strings* hashed_strings, struct tyran_memory* parent_memory, struct mocha_runtime* runtime, mocha_values_config config, const char* debugstring)
{
	tyran_memory* local_memory = &self->memory;
	self->hashed_strings = hashed_strings;
	size_t object_reference_size = sizeof(mocha_object*) * config.object_reference_count;
	size_t objects_size = sizeof(mocha_object) * config.objects_count;
	size_t total_memory_size = objects_size + object_reference_size + config.string_memory_size + config.blob_memory_size;
	tyran_memory_construct(local_memory, parent_memory, total_memory_size, debugstring);
	self->runtime = runtime;
	self->debug = debugstring;
	tyran_memory_construct(&self->objects, local_memory, objects_size, "objects");
	tyran_memory_construct(&self->object_references, local_memory, object_reference_size, "object-references");
	self->keyword_def.invoke = keyword_type_func;
	self->keyword_def.eval_all_arguments = mocha_true;

	tyran_memory_construct(&self->string_content_memory, local_memory, config.string_memory_size, "string-content");
	tyran_memory_construct(&self->blob_memory, local_memory, config.blob_memory_size, "blob-content");

	self->map_def.invoke = map_type_func;
	self->map_def.eval_all_arguments = mocha_true;

	self->nil_object.type = mocha_object_type_nil;
	self->true_object.type = mocha_object_type_true;
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

const mocha_object* mocha_values_create_symbol(mocha_values* self, const char* key_c_string)
{
	mocha_string_hash hash = mocha_hashed_strings_hash_string(self->hashed_strings, key_c_string);
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_symbol);
	mocha_symbol_init(&value->data.symbol, hash);

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

const struct mocha_object* mocha_values_create_closure(mocha_values* self, const mocha_context* context, const mocha_object* object)
{
	if (context == 0) {
		MOCHA_LOG("Create closure is null!");
	}
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_closure);

	mocha_closure_init(&value->data.closure, context, object);
	return value;
}

const struct mocha_object* mocha_values_create_context(mocha_values* self, const struct mocha_context* parent, const char* debugstring)
{
	if (parent == 0) {
		MOCHA_LOG("CAN NOT DO IT!");
	}
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_context);
	mocha_context* context = &value->data.context;
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
