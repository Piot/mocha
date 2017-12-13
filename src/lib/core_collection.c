#include <mocha/core_collection.h>
#include <mocha/def_function.h>
#include <mocha/execute.h>
#include <mocha/list.h>
#include <mocha/log.h>
#include <mocha/map_utils.h>
#include <mocha/print.h>
#include <mocha/random.h>
#include <mocha/reducer_internal.h>
#include <mocha/reducer_script.h>
#include <mocha/runtime.h>
#include <mocha/transduce.h>
#include <mocha/transducers_step.h>
#include <mocha/type.h>
#include <mocha/utils.h>
#include <mocha/values.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tyran/tyran_clib.h>

MOCHA_FUNCTION(shuffle_func)
{
	const mocha_object* seed_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[2]);

	int seed = mocha_object_integer(seed_object, "shuffle");
	const mocha_sequence* seq = mocha_object_sequence(sequence_object);

	size_t count;
	const mocha_object** source;
	mocha_sequence_get_objects(seq, &source, &count);
	if (count > 64) {
		MOCHA_LOG("err: Too many to shuffle!");
		return 0;
	}
	const mocha_object* temp[64];
	tyran_memcpy_type_n(temp, source, count);

	for (size_t i = 0; i < count; ++i) {
		int source_random_index = mocha_pseudo_random(seed, count);
		seed += 7919;
		int target_index = i;
		// MOCHA_LOG("Swapping: %d and %d", target_index, source_random_index)
		const mocha_object* o = temp[target_index];
		temp[target_index] = temp[source_random_index];
		temp[source_random_index] = o;
	}
	const mocha_object* result = mocha_values_create_vector(context->values, temp, count);
	return result;
}

MOCHA_FUNCTION(subvec_func)
{
	if (arguments->count < 3) {
		MOCHA_LOG("subvec needs 3 or more arguments");
		return 0;
	}
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* start_object = mocha_runner_eval(context, arguments->objects[2]);
	const mocha_sequence* seq = mocha_object_sequence(sequence_object);
	int start = mocha_object_integer(start_object, "subvec_start");
	int end;

	if (arguments->count > 3) {
		const mocha_object* end_object = mocha_runner_eval(context, arguments->objects[3]);
		end = mocha_object_integer(end_object, "subvec_end");
		if (end < start) {
			MOCHA_LOG("Error: subvec end wrong");
			return 0;
		}
		if (end > mocha_sequence_count(seq)) {
			MOCHA_LOG("Error: subvec long after end");
			return 0;
		}
	} else {
		end = mocha_sequence_count(seq);
	}

	int new_size = (end - start);
	if (new_size > 64) {
		MOCHA_LOG("subvec() Tooo big vector!");
		return 0;
	}

	const mocha_object* temp[64];
	for (int vector_index = start; vector_index < end; ++vector_index) {
		temp[vector_index - start] = mocha_sequence_get(seq, context->values, vector_index);
	}

	const mocha_object* r = mocha_values_create_vector(context->values, temp, new_size);

	return r;
}

MOCHA_FUNCTION(vec_func)
{
	const mocha_object* sequence = mocha_runner_eval(context, arguments->objects[1]);

	const mocha_object* r;

	switch (sequence->type) {
		case mocha_object_type_vector:
			r = sequence;
			break;
		case mocha_object_type_list:
			r = mocha_values_create_vector(context->values, sequence->data.list.objects, sequence->data.list.count);
			break;
		case mocha_object_type_map: {
			const mocha_map* map = &sequence->data.map;
			const mocha_object* objects[64];
			for (size_t i = 0; i < map->count; i += 2) {
				objects[i / 2] = mocha_values_create_vector(context->values, &map->objects[i], 2);
			}
			r = mocha_values_create_vector(context->values, objects, map->count / 2);
		} break;
		case mocha_object_type_nil:
			r = mocha_values_create_vector(context->values, 0, 0);
			break;
		default:
			r = 0;
	}

	return r;
}

MOCHA_FUNCTION(repeat_func)
{
	if (arguments->count < 3) {
		MOCHA_LOG("Error repeat() wrong number of arguments");
		return 0;
	}
	int repeat_count = mocha_object_integer(mocha_runner_eval(context, arguments->objects[1]), "repeat_n");
	const mocha_object* value_to_repeat = mocha_runner_eval(context, arguments->objects[2]);

	mocha_list repeat_list;

	mocha_list_init_prepare(&repeat_list, &context->values->object_references, repeat_count);
	for (int i = 0; i < repeat_count; ++i) {
		repeat_list.objects[i] = value_to_repeat;
	}
	const mocha_object* repeat_list_object = mocha_values_create_list(context->values, repeat_list.objects, repeat_list.count);
	return repeat_list_object;
}

MOCHA_FUNCTION(empty_func)
{
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[1]);
	mocha_boolean is_empty;
	const mocha_sequence* sequence = mocha_object_sequence(sequence_object);
	is_empty = mocha_sequence_count(sequence) == 0;
	const mocha_object* is_empty_object = mocha_values_create_boolean(context->values, is_empty);
	return is_empty_object;
}

typedef struct for_info {
	const mocha_object* symbols[8];
	const mocha_sequence* sequences[8];
	mocha_context* context;
	size_t divider[8];
	size_t count[8];
	size_t iteration_indexes[8];
	size_t sequence_count;
} for_info;

static void for_next_overwrite_context(for_info* self, size_t iteration_index)
{
	for (size_t i = 0; i < self->sequence_count; ++i) {
		size_t sequence_index = (iteration_index / self->divider[i]) % self->count[i];
		if (sequence_index != self->iteration_indexes[i]) {
			const mocha_sequence* sequence = self->sequences[i];
			const mocha_object* value = mocha_sequence_get(sequence, self->context->values, sequence_index);
			self->iteration_indexes[i] = sequence_index;
			const mocha_object* symbol_object = self->symbols[i];
			mocha_context_add(self->context, symbol_object, value);
		}
	}
}

MOCHA_FUNCTION(for_func)
{
	const mocha_object* assignments_object = arguments->objects[1];
	const mocha_object* body = arguments->objects[2];

	const mocha_vector* assignments = mocha_object_vector(assignments_object);
	if ((assignments->count % 2) != 0) {
		MOCHA_LOG("Must have assignments in pairs");
		return 0;
	}

	mocha_context* for_context = mocha_context_create(context, "for");

	size_t sequence_count = assignments->count / 2;

	size_t total_iterations = 1;
	for_info self;
	self.context = for_context;
	self.sequence_count = sequence_count;
	for (size_t i = 0; i < sequence_count; ++i) {
		const mocha_object* symbol = assignments->objects[i * 2];
		const mocha_object* seq_object = mocha_runner_eval(context, assignments->objects[i * 2 + 1]);
		const mocha_sequence* seq = mocha_object_sequence(seq_object);
		size_t count = mocha_sequence_count(seq);
		self.symbols[i] = symbol;
		self.sequences[i] = seq;
		self.count[i] = count;
		self.iteration_indexes[i] = 1;
		self.divider[i] = total_iterations;
		total_iterations *= count;
	}

	mocha_list result_list;
	mocha_list_init_prepare(&result_list, &context->values->object_references, total_iterations);
	for (size_t n = 0; n < total_iterations; ++n) {
		for_next_overwrite_context(&self, n);
		const mocha_object* item = mocha_runner_eval(for_context, body);
		result_list.objects[n] = item;
	}

	return mocha_values_create_list(context->values, result_list.objects, result_list.count);
}

MOCHA_FUNCTION(second_func)
{
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[1]);
	if (mocha_object_is_nil(sequence_object)) {
		return mocha_values_create_nil(context->values);
	}
	if (!mocha_object_is_sequence(sequence_object)) {
		MOCHA_LOG("Error, second() not a sequence!");
	}

	const mocha_sequence* seq = mocha_object_sequence(sequence_object);
	const mocha_object* result = mocha_sequence_get(seq, context->values, 1);
	return result;
}

MOCHA_FUNCTION(range_func)
{
	int start_value = 0;
	int increment = 1;
	int argument_index = 1;

	if (arguments->count > 2) {
		const mocha_object* start_value_object = mocha_runner_eval(context, arguments->objects[argument_index++]);
		start_value = mocha_object_integer(start_value_object, "range_start");
	}
	const mocha_object* range_end_value_object = arguments->objects[argument_index++];
	const mocha_object* end_value_object = mocha_runner_eval(context, range_end_value_object);
	MOCHA_LOG("END_VALUE:%s", mocha_print_object_debug_str(range_end_value_object));
	MOCHA_LOG("END_VALUE:%s", mocha_print_object_debug_str(end_value_object));
	int end_value = mocha_object_integer(end_value_object, "range_end");

	if (arguments->count > 3) {
		const mocha_object* increment_value_object = mocha_runner_eval(context, arguments->objects[argument_index++]);
		increment = mocha_object_integer(increment_value_object, "range_increment");
	}

	mocha_list range_list;

	int count = (end_value - start_value) / increment;

	mocha_list_init_prepare(&range_list, &context->values->object_references, count);
	for (int i = 0; i < count; ++i) {
		range_list.objects[i] = mocha_values_create_integer(context->values, start_value + (i * increment));
	}
	const mocha_object* list_object = mocha_values_create_list(context->values, range_list.objects, range_list.count);
	return list_object;
}

MOCHA_FUNCTION(str_func)
{
	char temp_buf[1024];
	temp_buf[0] = 0;
	for (size_t i = 1; i < arguments->count; ++i) {
		const mocha_object* a = arguments->objects[i];
		const char* x = mocha_print_object_debug_str_pure(a);
		tyran_strncat(temp_buf, x, 1024);
	}

	const mocha_object* result = mocha_values_create_string_from_cstr(context->values, temp_buf);

	return result;
}

MOCHA_FUNCTION(apply_func)
{
	const mocha_object* invokable = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* new_arguments_object = mocha_runner_eval(context, arguments->objects[2]);

	const mocha_sequence* seq = mocha_object_sequence(new_arguments_object);
	size_t seq_count = mocha_sequence_count(seq);
	size_t arg_count = seq_count + 1;

	mocha_list temp_arguments;
	mocha_list_init_prepare(&temp_arguments, &context->values->object_references, arg_count);
	temp_arguments.objects[0] = 0;
	for (size_t i = 0; i < seq_count; ++i) {
		temp_arguments.objects[i + 1] = mocha_sequence_get(seq, context->values, i);
	}
	return execute(context, invokable, &temp_arguments);
}

MOCHA_FUNCTION(count_func)
{
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[1]);
	size_t count;

	const mocha_sequence* sequence = mocha_object_sequence(sequence_object);
	if (sequence == 0) {
		MOCHA_LOG("Not a sequence!");
		return 0;
	}

	count = mocha_sequence_count(sequence);

	const mocha_object* o = mocha_values_create_integer(context->values, (int) count);

	return o;
}

MOCHA_FUNCTION(get_func)
{
	const mocha_object* object = mocha_runner_eval(context, arguments->objects[1]);

	if (!mocha_object_is_sequence(object)) {
		MOCHA_LOG("Error, not a sequence! %s", mocha_print_object_debug_str(object));
	}

	const mocha_object* lookup = mocha_runner_eval(context, arguments->objects[2]);
	const mocha_sequence* seq = mocha_object_sequence(object);
	const mocha_object* result = mocha_sequence_get_object(seq, context->values, lookup);
	if (mocha_object_is_nil(result) && arguments->count >= 4) {
		result = mocha_runner_eval(context, arguments->objects[3]);
	}
	return result;
}

MOCHA_FUNCTION(nth_func)
{
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[1]);
	if (!mocha_object_is_sequence(sequence_object)) {
		MOCHA_LOG("Error, not a sequence!");
	}
	const mocha_sequence* seq = mocha_object_sequence(sequence_object);

	const mocha_object* lookup = mocha_runner_eval(context, arguments->objects[2]);
	if (!mocha_object_is_integer(lookup)) {
		MOCHA_LOG("nth() must have integer index");
		return 0;
	}
	int index = mocha_object_integer(lookup, "nth_lookup");
	const mocha_object* result = mocha_sequence_get(seq, context->values, index);
	if (mocha_object_is_nil(result) && arguments->count >= 4) {
		result = arguments->objects[3];
	}
	return result;
}

static const mocha_object* rest_vector(mocha_values* values, const mocha_vector* self)
{
	const mocha_object* o;

	if (self->count > 0) {
		const mocha_object* result[128];
		memcpy(result, self->objects + 1, sizeof(mocha_object*) * (self->count - 1));
		size_t total_count = self->count - 1;
		o = mocha_values_create_list(values, result, total_count);
	} else {
		o = mocha_values_create_nil(values);
	}

	return o;
}

static const mocha_object* rest_list(mocha_values* values, const mocha_list* self)
{
	const mocha_object* o;

	if (self->count > 0) {
		const mocha_object* result[128];
		memcpy(result, self->objects + 1, sizeof(mocha_object*) * (self->count - 1));
		size_t total_count = self->count - 1;
		o = mocha_values_create_list(values, result, total_count);
	} else {
		o = mocha_values_create_nil(values);
	}

	return o;
}

MOCHA_FUNCTION(rest_func)
{
	const mocha_object* sequence = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* result;

	switch (sequence->type) {
		case mocha_object_type_list:
			result = rest_list(context->values, &sequence->data.list);
			break;
		case mocha_object_type_vector:
			result = rest_vector(context->values, &sequence->data.vector);
			break;
		case mocha_object_type_nil:
			result = mocha_values_create_list(context->values, 0, 0);
			break;
		case mocha_object_type_map:
			result = 0;
			break;
		default:
			result = 0;
			break;
	}

	return result;
}

MOCHA_FUNCTION(next_func)
{
	const mocha_object* list_object = rest_func(context, arguments);
	const mocha_sequence* seq = mocha_object_sequence(list_object);
	if (mocha_sequence_count(seq) == 0) {
		return mocha_values_create_nil(context->values);
	}
	return list_object;
}

MOCHA_FUNCTION(first_func)
{
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[1]);
	MOCHA_LOG("First:%s", mocha_print_object_debug_str(arguments->objects[1]));
	const mocha_sequence* sequence = mocha_object_sequence(sequence_object);
	const mocha_object* result = mocha_sequence_get(sequence, context->values, 0);

	return result;
}

static const mocha_object* cons_vector(mocha_values* values, const mocha_vector* self, const mocha_object** args)
{
	const int count = 1;
	const mocha_object* result[128];

	memcpy(result, args, sizeof(mocha_object*) * count);
	memcpy(result + count, self->objects, sizeof(mocha_object*) * self->count);
	size_t total_count = self->count + count;
	const mocha_object* o = mocha_values_create_list(values, result, total_count);

	return o;
}

static const mocha_object* cons_list(mocha_values* values, const mocha_list* self, const mocha_object** args)
{
	const mocha_object* result[128];
	const int count = 1;

	memcpy(result, args, sizeof(mocha_object*) * count);
	memcpy(result + count, self->objects, sizeof(mocha_object*) * self->count);
	size_t total_count = self->count + count;
	const mocha_object* new_list = mocha_values_create_list(values, result, total_count);

	return new_list;
}

MOCHA_FUNCTION(cons_func) // Add and return the *fastest* (list) type of sequence
{
	const mocha_object* sequence = mocha_runner_eval(context, arguments->objects[2]);
	const mocha_object* result;
	const mocha_object* item_to_prepend = mocha_runner_eval(context, arguments->objects[1]);

	switch (sequence->type) {
		case mocha_object_type_list:
			result = cons_list(context->values, &sequence->data.list, &item_to_prepend);
			break;
		case mocha_object_type_vector:
			result = cons_vector(context->values, &sequence->data.vector, &item_to_prepend);
			break;
		case mocha_object_type_nil: {
			result = mocha_values_create_list(context->values, &item_to_prepend, 1);
		} break;
		case mocha_object_type_map:
			MOCHA_LOG("BAD MAP");
			result = 0;
			break;
		default:
			result = 0;
			break;
	}

	return result;
}

MOCHA_FUNCTION(concat_func)
{
	size_t sequence_count = arguments->count - 1;
	size_t total_item_count = 0;
	for (size_t i = 0; i < sequence_count; ++i) {
		const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[i + 1]);
		const mocha_sequence* seq = mocha_object_sequence(sequence_object);
		size_t count = mocha_sequence_count(seq);
		// MOCHA_LOG("concat: Seq %d '%s' (%d)", i, mocha_print_object_debug_str(sequence_object), count);
		total_item_count += count;
	}

	// MOCHA_LOG("Concat start %d", total_item_count);

	const mocha_object** temp_buf = (const mocha_object**) TYRAN_MEMORY_CALLOC_TYPE_COUNT(&context->values->object_references, mocha_object*, total_item_count);
	size_t target_index = 0;
	for (size_t sequence_index = 0; sequence_index < sequence_count; ++sequence_index) {
		const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[sequence_index + 1]);
		const mocha_sequence* seq = mocha_object_sequence(sequence_object);
		size_t item_count = mocha_sequence_count(seq);
		for (size_t item_index = 0; item_index < item_count; ++item_index) {
			const mocha_object* item = mocha_sequence_get(seq, context->values, item_index);
			// MOCHA_LOG("item %d '%s'", item_index, mocha_print_object_debug_str(item));
			temp_buf[target_index++] = item;
		}
	}
	const mocha_object* result = mocha_values_create_list(context->values, temp_buf, total_item_count);
	// MOCHA_LOG("Concat done! '%s'", mocha_print_object_debug_str(result));
	return result;
}

static const struct mocha_object* vector_assoc(const mocha_vector* vector, mocha_values* values, const mocha_object** adds, size_t add_count)
{
	size_t new_count = vector->count + add_count;
	const mocha_object** result = tyran_malloc(sizeof(mocha_object*) * new_count);
	tyran_memcpy_type_n(result, vector->objects, vector->count);

	size_t end_count = vector->count;

	for (size_t i = 0; i < add_count; i += 2) {
		const mocha_object* key = adds[i];
		const mocha_object* value = adds[i + 1];
		int index = mocha_object_integer(key, "vector_assoc key");
		MOCHA_LOG("Index:%d", index);
		if (index >= 0 && index < vector->count) {
			result[index] = value;
		} else if (index == end_count) {
			// MOCHA_LOG("Couldn't find '%d' adding to end", index);
			result[end_count++] = value;
		}
	}

	const mocha_object* new_vector = mocha_values_create_vector(values, result, end_count);
	tyran_free(result);

	return new_vector;
}

MOCHA_FUNCTION(assoc_func)
{
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[1]);

	size_t pairs_count = arguments->count - 2;

	mocha_list temp;
	temp.objects = &arguments->objects[2];
	temp.count = pairs_count;

	const struct mocha_object* args = mocha_runner_eval_list(context, &temp);
	const mocha_list* evaled_args = mocha_object_list(args);
	// printf("evaled:%s", mocha_print_object_debug_str(args));
	const mocha_object** new_key_value_pairs = evaled_args->objects;
	const mocha_object* new_seq_object;

	if (mocha_object_is_vector(sequence_object)) {
		const mocha_vector* vector = mocha_object_vector(sequence_object);
		new_seq_object = vector_assoc(vector, context->values, new_key_value_pairs, pairs_count);
	} else if (mocha_object_is_map(sequence_object)) {
		const mocha_map* map = mocha_object_map(sequence_object);
		new_seq_object = mocha_map_assoc(map, context->values, new_key_value_pairs, pairs_count);
	} else {
		MOCHA_LOG("Unknown sequence:%s", mocha_print_object_debug_str(sequence_object));
	}
	return new_seq_object;
}

MOCHA_FUNCTION(dissoc_func)
{
	const mocha_object* map_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* key = 0;
	const mocha_map* map = &map_object->data.map;

	if (map->count == 0 || arguments->count < 3) {
		return map_object;
	}
	key = arguments->objects[2];
	const mocha_object* result[128];

	size_t total_count = map->count;
	size_t overwrite_index = (map->count - 1) * 2;
	for (size_t i = 0; i < map->count; i += 2) {
		if (mocha_object_equal(map->objects[i], key)) {
			overwrite_index = i;
			total_count = map->count - 2;
			break;
		}
	}

	memcpy(result, map->objects, sizeof(mocha_object*) * overwrite_index);
	memcpy(&result[overwrite_index], &map->objects[overwrite_index + 2], sizeof(mocha_object*) * ((map->count - 1) * 2 - overwrite_index));

	const mocha_object* new_map = mocha_values_create_map(context->values, result, total_count);

	return new_map;
}

static const mocha_object* conj_map(mocha_values* values, const mocha_map* self, const mocha_map* arg)
{
	const mocha_object* result[128];

	memcpy(result, self->objects, sizeof(mocha_object*) * self->count);
	memcpy(result + self->count, arg->objects, sizeof(mocha_object*) * arg->count);
	size_t total_count = self->count + arg->count;
	const mocha_object* new_map = mocha_values_create_map(values, result, total_count);

	return new_map;
}

static const mocha_object* conj_vector(mocha_values* values, const mocha_vector* self, const mocha_object** args, size_t count)
{
	const mocha_object* result[128];

	memcpy(result, self->objects, sizeof(mocha_object*) * self->count);
	memcpy(result + self->count, args, sizeof(mocha_object*) * count);
	size_t total_count = self->count + count;
	const mocha_object* o = mocha_values_create_vector(values, result, total_count);

	return o;
}

static const mocha_object* conj_list(mocha_values* values, const mocha_list* self, const mocha_object** args, size_t count)
{
	const mocha_object* result[128];

	for (size_t i = 0; i < count; ++i) {
		result[(count - i) - 1] = args[i];
	}

	size_t total_count = count;

	if (self) {
		memcpy(result + count, self->objects, sizeof(mocha_object*) * self->count);
		total_count += self->count;
	}
	const mocha_object* new_list = mocha_values_create_list(values, result, total_count);

	return new_list;
}

MOCHA_FUNCTION(conj_func) // Add and return the *same* type of sequence-object
{
	const mocha_object* sequence = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_object* result;

	const mocha_object* adds[64];
	size_t items_to_add = arguments->count - 2;
	for (size_t i = 0; i < items_to_add; ++i) {
		const mocha_object* item_to_add = mocha_runner_eval(context, arguments->objects[i + 2]);
		adds[i] = item_to_add;
	}
	switch (sequence->type) {
		case mocha_object_type_list:
			result = conj_list(context->values, &sequence->data.list, adds, items_to_add);
			break;
		case mocha_object_type_vector:
			result = conj_vector(context->values, &sequence->data.vector, adds, items_to_add);
			break;
		case mocha_object_type_nil:
			result = conj_list(context->values, 0, adds, items_to_add);
			break;
		case mocha_object_type_map:
			result = conj_map(context->values, &sequence->data.map, &arguments->objects[2]->data.map);
			break;
		default:
			result = 0;
			break;
	}

	return result;
}

const mocha_object* do_some(mocha_values* values, const struct mocha_object* predicate_value, const struct mocha_object* item, mocha_boolean* should_continue)
{
	mocha_boolean truth = mocha_object_truthy(predicate_value);
	*should_continue = !truth;
	return predicate_value;
}

MOCHA_FUNCTION(some_func)
{
	return mocha_reducer_reduce_internal_single_fn(context, arguments, do_some, "some");
}

const mocha_object* do_reduce_init(mocha_values* values)
{
	return mocha_values_create_integer(values, 0);
}

MOCHA_FUNCTION(reduce_func)
{
	return mocha_reducer_reduce_script(context, arguments, "reduce");
}

const mocha_object* do_every(mocha_values* values, const struct mocha_object* predicate_value, const struct mocha_object* item, mocha_boolean* should_continue)
{
	mocha_boolean truth = mocha_object_truthy(predicate_value);
	*should_continue = truth;
	return mocha_values_create_boolean(values, truth);
}

MOCHA_FUNCTION(every_func)
{
	return mocha_reducer_reduce_internal_single_fn(context, arguments, do_every, "every");
}

const mocha_object* do_filter(const struct mocha_object* predicate_value, const struct mocha_object* item, mocha_boolean* should_add_it, mocha_boolean* should_continue)
{
	*should_add_it = mocha_object_truthy(predicate_value);
	*should_continue = mocha_true;
	return item;
}

MOCHA_FUNCTION(filter_func)
{
	return mocha_transduce_internal(context, do_filter, arguments, TYRAN_FALSE);
}

const mocha_object* do_map(const struct mocha_object* predicate_value, const struct mocha_object* item, mocha_boolean* should_add_it, mocha_boolean* should_continue)
{
	*should_add_it = mocha_true;
	*should_continue = mocha_true;
	return predicate_value;
}

MOCHA_FUNCTION(map_func)
{
	return mocha_transduce_internal(context, do_map, arguments, TYRAN_FALSE);
}

MOCHA_FUNCTION(map_indexed_func)
{
	return mocha_transduce_internal(context, do_map, arguments, TYRAN_TRUE);
}

const mocha_object* do_keep(const struct mocha_object* predicate_value, const struct mocha_object* item, mocha_boolean* should_add_it, mocha_boolean* should_continue)
{
	*should_add_it = !mocha_object_is_nil(predicate_value);
	*should_continue = mocha_true;
	return predicate_value;
}

MOCHA_FUNCTION(keep_func)
{
	return mocha_transduce_internal(context, do_keep, arguments, TYRAN_FALSE);
}

const mocha_object* do_remove(const struct mocha_object* predicate_value, const struct mocha_object* item, mocha_boolean* should_add_it, mocha_boolean* should_continue)
{
	*should_add_it = !mocha_object_truthy(predicate_value);
	*should_continue = mocha_true;
	return item;
}

MOCHA_FUNCTION(remove_func)
{
	return mocha_transduce_internal(context, do_remove, arguments, TYRAN_FALSE);
}

MOCHA_FUNCTION(reverse_func)
{
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_sequence* sequence = mocha_object_sequence(sequence_object);

	size_t count = mocha_sequence_count(sequence);

	mocha_list repeat_list;
	mocha_list_init_prepare(&repeat_list, &context->values->object_references, count);

	for (size_t i = 0; i < count; ++i) {
		repeat_list.objects[count - i - 1] = mocha_sequence_get(sequence, context->values, i);
	}

	const mocha_object* repeat_list_object = mocha_values_create_list(context->values, repeat_list.objects, repeat_list.count);
	return repeat_list_object;
}

MOCHA_FUNCTION(flatten_func)
{
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_sequence* sequence = mocha_object_sequence(sequence_object);

	size_t count = mocha_sequence_count(sequence);
	size_t total_count = 0;
	for (size_t i = 0; i < count; ++i) {
		const mocha_object* sub_object = mocha_sequence_get(sequence, context->values, i);
		const mocha_object* sub_sequence_object = mocha_runner_eval(context, sub_object);
		const mocha_sequence* sub_sequence = mocha_object_sequence(sub_sequence_object);
		total_count += mocha_sequence_count(sub_sequence);
	}

	mocha_list repeat_list;
	mocha_list_init_prepare(&repeat_list, &context->values->object_references, total_count);

	size_t index = 0;
	for (size_t i = 0; i < count; ++i) {
		const mocha_object* sub_object = mocha_sequence_get(sequence, context->values, i);
		const mocha_object* sub_sequence_object = mocha_runner_eval(context, sub_object);
		const mocha_sequence* sub_sequence = mocha_object_sequence(sub_sequence_object);
		size_t sub_sequence_count = mocha_sequence_count(sub_sequence);
		for (size_t j = 0; j < sub_sequence_count; ++j) {
			repeat_list.objects[index++] = mocha_sequence_get(sub_sequence, context->values, j);
		}
	}

	const mocha_object* repeat_list_object = mocha_values_create_list(context->values, repeat_list.objects, repeat_list.count);
	return repeat_list_object;
}

MOCHA_FUNCTION(partition_func)
{
	const mocha_object* segment_size_object = mocha_runner_eval(context, arguments->objects[1]);
	size_t segment_size = mocha_object_integer(segment_size_object, "partition-n");
	const mocha_object* sequence_object = mocha_runner_eval(context, arguments->objects[2]);
	const mocha_sequence* sequence = mocha_object_sequence(sequence_object);

	mocha_list repeat_list;
	mocha_list_init_prepare(&repeat_list, &context->values->object_references, segment_size);

	size_t sequence_count = mocha_sequence_count(sequence);
	size_t segment_count = sequence_count / segment_size;
	mocha_list segment_list;
	mocha_list_init_prepare(&segment_list, &context->values->object_references, segment_count);

	for (size_t segment = 0; segment < segment_count; ++segment) {
		size_t start_index = segment * segment_size;
		for (size_t i = 0; i < segment_size; ++i) {
			size_t index = start_index + i;
			repeat_list.objects[i] = mocha_sequence_get(sequence, context->values, index);
		}
		const mocha_object* segment_list_object = mocha_values_create_list(context->values, repeat_list.objects, repeat_list.count);
		segment_list.objects[segment] = segment_list_object;
	}

	const mocha_object* repeat_list_object = mocha_values_create_list(context->values, segment_list.objects, segment_list.count);
	return repeat_list_object;
}

typedef const mocha_object* (*sequence_create_collection)(mocha_values* values, const struct mocha_object* o[], size_t count);

static const mocha_object* convert_sequence(const mocha_context* context, const mocha_sequence* seq, sequence_create_collection fn)
{
	const mocha_object** objects;
	size_t count;
	mocha_sequence_get_objects(seq, &objects, &count);

	const mocha_object* result = fn(context->values, objects, count);

	return result;
}

const mocha_object* force_to_list(const mocha_context* context, const mocha_sequence* seq)
{
	const mocha_object* list_object = convert_sequence(context, seq, mocha_values_create_list);
	return list_object;
}

MOCHA_FUNCTION(seq_func)
{
	const mocha_object* seq_object = mocha_runner_eval(context, arguments->objects[1]);
	const mocha_sequence* seq = mocha_object_sequence(seq_object);
	if (mocha_sequence_count(seq) == 0) {
		return mocha_values_create_nil(context->values);
	}
	return force_to_list(context, seq);
}

void mocha_core_collection_define_context(mocha_context* context, mocha_values* values)
{
	MOCHA_DEF_FUNCTION(conj);
	MOCHA_DEF_FUNCTION(filter);
	MOCHA_DEF_FUNCTION(map);
	MOCHA_DEF_FUNCTION_EX(map_indexed, "map-indexed");
	MOCHA_DEF_FUNCTION(keep);
	MOCHA_DEF_FUNCTION(remove);
	MOCHA_DEF_FUNCTION(reduce);
	MOCHA_DEF_FUNCTION(seq);
	MOCHA_DEF_FUNCTION(assoc);
	MOCHA_DEF_FUNCTION(dissoc);
	MOCHA_DEF_FUNCTION(concat);
	MOCHA_DEF_FUNCTION(cons);
	MOCHA_DEF_FUNCTION(first);
	MOCHA_DEF_FUNCTION(rest);
	MOCHA_DEF_FUNCTION(next);
	MOCHA_DEF_FUNCTION(get);
	MOCHA_DEF_FUNCTION(count);
	MOCHA_DEF_FUNCTION(nth);
	MOCHA_DEF_FUNCTION(str);
	MOCHA_DEF_FUNCTION(apply);
	MOCHA_DEF_FUNCTION(range);
	MOCHA_DEF_FUNCTION(second);
	MOCHA_DEF_FUNCTION(for);
	MOCHA_DEF_FUNCTION_EX(empty, "empty?");
	MOCHA_DEF_FUNCTION(repeat);
	MOCHA_DEF_FUNCTION(vec);
	MOCHA_DEF_FUNCTION(subvec);
	MOCHA_DEF_FUNCTION(some);
	MOCHA_DEF_FUNCTION(shuffle);
	MOCHA_DEF_FUNCTION(reverse);
	MOCHA_DEF_FUNCTION(flatten);
	MOCHA_DEF_FUNCTION(partition);
	MOCHA_DEF_FUNCTION_EX(every, "every?");
}
