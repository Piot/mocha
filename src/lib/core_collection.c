#include <mocha/core_collection.h>
#include <mocha/def_function.h>
#include <mocha/execute.h>
#include <mocha/list.h>
#include <mocha/log.h>
#include <mocha/map_utils.h>
#include <mocha/print.h>
#include <mocha/random.h>
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
/*






static const mocha_object *cons_vector(mocha_values *values, const mocha_vector *self, const mocha_object **args)
{
	const int count = 1;
	const mocha_object *result[128];

	memcpy(result, args, sizeof(mocha_object *) * count);
	memcpy(result + count, self->objects, sizeof(mocha_object *) * self->count);
	size_t total_count = self->count + count;
	const mocha_object *o = mocha_values_create_list(values, result, total_count);

	return o;
}

static const mocha_object *cons_list(mocha_values *values, const mocha_list *self, const mocha_object **args)
{
	const mocha_object *result[128];
	const int count = 1;

	memcpy(result, args, sizeof(mocha_object *) * count);
	memcpy(result + count, self->objects, sizeof(mocha_object *) * self->count);
	size_t total_count = self->count + count;
	const mocha_object *new_list = mocha_values_create_list(values, result, total_count);

	return new_list;
}

MOCHA_FUNCTION(cons_func) // Add and return the *fastest* (list) type of sequence
{
	const mocha_object *sequence = arguments->objects[2];
	const mocha_object *result;

	switch (sequence->type)
	{
	case mocha_object_type_list:
		result = cons_list(context->values, &sequence->data.list, &arguments->objects[1]);
		break;
	case mocha_object_type_vector:
		result = cons_vector(context->values, &sequence->data.vector, &arguments->objects[1]);
		break;
	case mocha_object_type_nil:
	{
		result = mocha_values_create_list(context->values, &arguments->objects[1], 1);
	}
	break;
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

static const mocha_object *rest_vector(mocha_values *values, const mocha_vector *self)
{
	const mocha_object *o;

	if (self->count > 0)
	{
		const mocha_object *result[128];
		memcpy(result, self->objects + 1, sizeof(mocha_object *) * (self->count - 1));
		size_t total_count = self->count - 1;
		o = mocha_values_create_list(values, result, total_count);
	}
	else
	{
		o = mocha_values_create_nil(values);
	}

	return o;
}

static const mocha_object *rest_list(mocha_values *values, const mocha_list *self)
{
	const mocha_object *o;

	if (self->count > 0)
	{
		const mocha_object *result[128];
		memcpy(result, self->objects + 1, sizeof(mocha_object *) * (self->count - 1));
		size_t total_count = self->count - 1;
		o = mocha_values_create_list(values, result, total_count);
	}
	else
	{
		o = mocha_values_create_nil(values);
	}

	return o;
}

MOCHA_FUNCTION(rest_func)
{
	const mocha_object *sequence = arguments->objects[1];
	const mocha_object *result;

	switch (sequence->type)
	{
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

MOCHA_FUNCTION(first_func)
{
	const mocha_object *sequence_object = arguments->objects[1];

	const mocha_sequence *sequence = mocha_object_sequence(sequence_object);
	const mocha_object *result = mocha_sequence_get(sequence, context->values, 0);

	return result;
}

MOCHA_FUNCTION(second_func)
{
	const mocha_object *sequence_object = arguments->objects[1];
	if (mocha_object_is_nil(sequence_object))
	{
		return mocha_values_create_nil(context->values);
	}
	if (!mocha_object_is_sequence(sequence_object))
	{
		MOCHA_LOG("Error, second() not a sequence!");
	}

	const mocha_sequence *seq = mocha_object_sequence(sequence_object);
	const mocha_object *result = mocha_sequence_get(seq, context->values, 1);
	return result;
}


   MOCHA_FUNCTION(unquote_func)
   {
		// MOCHA_LOG("Unquoting:");
		// mocha_print_object_debug(arguments->objects[1]);
		// return mocha_runtime_eval_symbols(runtime, arguments->objects[1], &runtime->error);
   }


MOCHA_FUNCTION(count_func)
{
	const mocha_object *sequence_object = arguments->objects[1];
	size_t count;

	if (mocha_object_is_nil(sequence_object))
	{
		count = 0;
	}
	else
	{
		const mocha_sequence *sequence = mocha_object_sequence(sequence_object);
		if (sequence == 0)
		{
			MOCHA_LOG("Not a sequence!");
			return 0;
		}

		count = mocha_sequence_count(sequence);
	}

	const mocha_object *o = mocha_values_create_integer(context->values, (int)count);

	return o;
}

typedef struct for_info
{
	const mocha_sequence *sequences[8];
	const mocha_object *symbols[8];
	const mocha_object *body;
	mocha_context *context;
	size_t divider[8];
	size_t sequence_count;
	size_t iteration_index;
	size_t iteration_count;
	const mocha_object **result_objects;
} for_info;

static void for_next(for_info *self);

static void for_next_done(void *user_data, const mocha_object *result)
{
	for_info *self = (for_info *)user_data;

	size_t index = self->iteration_index;
	// MOCHA_LOG("For_next_done %d", index);
	self->result_objects[index] = result;
	self->iteration_index++;
	if (self->iteration_index == self->iteration_count)
	{
		const mocha_object *result_list = mocha_values_create_list(self->context->values, self->result_objects, self->iteration_count);
		// MOCHA_LOG("For completely done! '%s'", mocha_print_object_debug_str(result_list) );
		tyran_free(self->result_objects);
		tyran_free(self);
		return;
	}

	for_next(self);
}

static void for_next_context(for_info *self)
{
	for (size_t i = 0; i < self->sequence_count; ++i)
	{
		const mocha_object *symbol_object = self->symbols[i];
		const mocha_sequence *sequence = self->sequences[i];
		size_t sequence_index = (self->iteration_index / self->divider[i]) % mocha_sequence_count(sequence);
		const mocha_object *value = mocha_sequence_get(sequence, self->context->values, sequence_index);

		mocha_context_add(self->context, symbol_object, value);
	}
}

static void for_next(for_info *self)
{
	// MOCHA_LOG("for_next");

	for_next_context(self);
}

static void for_sequences_resolved(void *user_data, const mocha_object *sequence_list)
{
	// MOCHA_LOG("for_sequences_resolved");
	for_info *self = (for_info *)user_data;
	const mocha_sequence *sequences = mocha_object_sequence(sequence_list);
	size_t sequences_count = mocha_sequence_count(sequences);

	for (size_t i = 0; i < sequences_count; ++i)
	{
		const mocha_object *sequence_object = mocha_sequence_get(sequences, self->context->values, i);
		self->sequences[i] = mocha_object_sequence(sequence_object);
	}

	size_t iteration_count = 1;
	for (int i = (int)sequences_count - 1; i >= 0; --i)
	{
		self->divider[i] = iteration_count;
		const mocha_sequence *seq = self->sequences[i];
		iteration_count *= mocha_sequence_count(seq);
	}
	self->iteration_count = iteration_count;

	self->iteration_index = 0;
	self->result_objects = tyran_malloc(sizeof(mocha_object *) * iteration_count);
	// MOCHA_LOG("Reserving %d in target", iteration_count);
	for_next(self);
}

MOCHA_FUNCTION(for_func)
{
	const mocha_object *assignments_object = arguments->objects[1];
	const mocha_object *body = arguments->objects[2];

	const mocha_vector *assignments = mocha_object_vector(assignments_object);
	if ((assignments->count % 2) != 0)
	{
		MOCHA_LOG("Must have assignments in pairs");
		return 0;
	}

	size_t sequence_count = assignments->count / 2;

	for_info *info = tyran_malloc(sizeof(for_info));
	info->context = mocha_context_create(context, "for_func_context");
	info->body = body;
	info->sequence_count = sequence_count;

	const mocha_object *temp_objects[8];
	for (size_t i = 0; i < sequence_count; ++i)
	{
		info->symbols[i] = assignments->objects[i * 2];
		temp_objects[i] = assignments->objects[i * 2 + 1];
	}

	const mocha_object *sequences_list_object = mocha_values_create_list(context->values, temp_objects, sequence_count);
	const mocha_sequence *sequence_list_seq = mocha_object_sequence(sequences_list_object);

	resolve_sequence_callback(context, sequence_list_seq, mocha_object_type_list, info, for_sequences_resolved);

	return 0;
}

typedef struct some_info
{
	const mocha_sequence *seq;
	const mocha_object *predicate_function_object;
	size_t index;
	size_t count;
	mocha_boolean check_for_first_true;
	const mocha_context *context;
} some_info;

static void some_next(some_info *self);

static void some_done(some_info *self, const mocha_object *result)
{
	// MOCHA_LOG("some_done %d/%d", self->index, self->count);
	tyran_free(self);
}

static void some_next_test(some_info *self, const mocha_object *result)
{
	// MOCHA_LOG("some_next_test %d '%s'", self->index, mocha_print_object_debug_str(result));
	mocha_boolean is_truthy = mocha_object_truthy(result);
	if (self->check_for_first_true && is_truthy)
	{
		some_done(self, result);
	}
	else if (!self->check_for_first_true && !is_truthy)
	{
		const mocha_object *true_object = mocha_values_create_boolean(self->context->values);
		some_done(self, true_object);
	}
	else
	{
		self->index++;
		some_next(self);
	}
}

static void some_next_resolved(void *user_data, const mocha_object *result)
{
	some_info *info = (some_info *)user_data;
	some_next_test(info, result);
}

static void some_next(some_info *self)
{
	if (self->index == mocha_sequence_count(self->seq))
	{
		some_done(self, mocha_values_create_boolean(self->context->values, !self->check_for_first_true));
		return;
	}
	// MOCHA_LOG("some_next %d", self->index);
	const mocha_object *item = mocha_sequence_get(self->seq, self->context->values, self->index);

	mocha_list temp_list;
	const mocha_object *temp_objects[2];
	temp_objects[0] = 0;
	temp_objects[1] = item;
	mocha_list_init(&temp_list, &self->context->values->object_references, temp_objects, 2);

	resolve_callback execute_resolve_info;
	execute_resolve_info.user_data = self;

	execute(self->context, self->predicate_function_object, &temp_list, execute_resolve_info);
}

static void some_helper(const mocha_context *context, const mocha_list *arguments, resolve_callback result_callback, mocha_boolean check_for_first_true)
{
	const mocha_object *predicate_function_object = arguments->objects[1];
	const mocha_object *sequence_object = arguments->objects[2];

	some_info *info = tyran_malloc(sizeof(some_info));
	info->index = 0;
	info->seq = mocha_object_sequence(sequence_object);
	info->count = mocha_sequence_count(info->seq);
	info->predicate_function_object = predicate_function_object;
	info->callback_info = result_callback;
	info->check_for_first_true = check_for_first_true;
	info->context = context;

	some_next(info);
}

MOCHA_FUNCTION(some_func)
{
	// some_helper(context, arguments, result_callback);
	return 0;
}

MOCHA_FUNCTION(every_func)
{
	// some_helper(context, arguments, result_callback);
	return 0;
}

MOCHA_FUNCTION(empty_func)
{
	const mocha_object *sequence_object = arguments->objects[1];
	mocha_boolean is_empty;
	if (mocha_object_is_nil(sequence_object))
	{
		is_empty = mocha_true;
	}
	else
	{
		const mocha_sequence *sequence = mocha_object_sequence(sequence_object);
		is_empty = mocha_sequence_count(sequence) == 0;
	}
	const mocha_object *is_empty_object = mocha_values_create_boolean(context->values, is_empty);
	return  is_empty_object;
}

typedef struct map_func_info
{
	mocha_list target_list;
	const mocha_sequence *input[8];
	size_t input_count;
	resolve_callback resolve_info;
	mocha_values *values;
	const struct mocha_context *context;
	const mocha_object *invokable;
	mocha_boolean only_keep_non_nils;
	size_t written_index;
} map_func_info;

typedef struct map_func_argument_info
{
	size_t index;
	const mocha_object *temp_map_arguments[64];
	map_func_info *parent;
} map_func_argument_info;

static void execute_next(map_func_info *self, int next_index);

static void map_func_completely_done(map_func_info *self)
{
	// MOCHA_LOG("Map func ALL done. Index %d (total %d)", self->target_list.count, next_index);
	const mocha_object *result_list_object = mocha_values_create_list(self->values, self->target_list.objects, self->written_index);
	resolve_callback callback = self->resolve_info;
	tyran_free(self);
	callback.callback(callback.user_data, result_list_object);
}

static void map_func_one_argument_done(void *user_data, const mocha_object *result)
{
	map_func_argument_info *self = (map_func_argument_info *)user_data;
	map_func_info *info = self->parent;
	if (!info->only_keep_non_nils || !mocha_object_is_nil(result))
	{
		info->target_list.objects[info->written_index++] = result;
	}
	int next_index = self->index + 1;
	tyran_free(self);
	// MOCHA_LOG("Map func done %d %d", self->index + 1, info->target_list.count);
	execute_next(info, next_index);
}

void execute_next(map_func_info *self, int next_index)
{
	if (self->target_list.count == next_index)
	{
		map_func_completely_done(self);
		return;
	}
	else
	{
		// MOCHA_LOG("Index %d (total %d)", self->target_list.count, next_index);
	}
	// MOCHA_LOG("execute_next collection_count:%d index:%d", self->input_count, next_index);
	mocha_list temp_map_argument_list;
	// const mocha_object* map_arguments = mocha_values_create_list(context->values,  temp_map_list, 2);
	map_func_argument_info *arg_info = tyran_malloc(sizeof(map_func_argument_info));

	arg_info->index = next_index;
	arg_info->parent = self;
	arg_info->temp_map_arguments[0] = 0;
	for (size_t i = 0; i < self->input_count; ++i)
	{
		const mocha_sequence *sequence = self->input[i];
		const mocha_object *entry = mocha_sequence_get(sequence, self->values, next_index);
		// MOCHA_LOG("map entry %d '%s'", i, mocha_print_object_debug_str(entry));
		arg_info->temp_map_arguments[i + 1] = entry;
	}

	mocha_list_init(&temp_map_argument_list, &self->values->object_references, arg_info->temp_map_arguments, self->input_count + 1);

	resolve_callback resolve_info;
	resolve_info.callback = map_func_one_argument_done;
	resolve_info.user_data = arg_info;
	execute(self->context, self->invokable, &temp_map_argument_list, resolve_info);
}

static void map_func_resolve(const mocha_context *context, const mocha_object *invokable, const mocha_sequence *input[], size_t count, size_t maximum_count, mocha_boolean only_non_nils, resolve_callback result_callback)
{
	map_func_info *info = tyran_malloc(sizeof(map_func_info));

	info->written_index = 0;
	info->resolve_info = result_callback;
	info->values = context->values;
	info->context = context;
	for (size_t i = 0; i < count; ++i)
	{
		info->input[i] = input[i];
	}
	info->invokable = invokable;
	info->input_count = count;
	info->resolve_info = result_callback;
	info->only_keep_non_nils = only_non_nils;
	mocha_list_init_prepare(&info->target_list, &context->values->object_references, maximum_count);
	execute_next(info, 0);
}

MOCHA_FUNCTION(repeat_func)
{
	if (arguments->count < 3)
	{
		MOCHA_LOG("Error repeat() wrong number of arguments");
		return;
	}
	int repeat_count = mocha_object_integer(arguments->objects[1], "repeat_n");
	const mocha_object *value_to_repeat = arguments->objects[2];

	mocha_list repeat_list;

	mocha_list_init_prepare(&repeat_list, &context->values->object_references, repeat_count);
	for (int i = 0; i < repeat_count; ++i)
	{
		repeat_list.objects[i] = value_to_repeat;
	}
	const mocha_object *repeat_list_object = mocha_values_create_list(context->values, repeat_list.objects, repeat_list.count);
	return repeat_list_object;
}

MOCHA_FUNCTION(range_func)
{
	int start_value = 0;
	int increment = 1;
	int argument_index = 1;

	if (arguments->count > 2)
	{
		const mocha_object *start_value_object = arguments->objects[argument_index++];
		start_value = mocha_object_integer(start_value_object, "range_start");
	}
	const mocha_object *end_value_object = arguments->objects[argument_index++];
	int end_value = mocha_object_integer(end_value_object, "range_end");

	if (arguments->count > 3)
	{
		const mocha_object *increment_value_object = arguments->objects[argument_index++];
		increment = mocha_object_integer(increment_value_object, "range_increment");
	}

	mocha_list range_list;

	int count = (end_value - start_value) / increment;

	mocha_list_init_prepare(&range_list, &context->values->object_references, count);
	for (int i = 0; i < count; ++i)
	{
		range_list.objects[i] = mocha_values_create_integer(context->values, start_value + (i * increment));
	}
	const mocha_object *list_object = mocha_values_create_list(context->values, range_list.objects, range_list.count);
	return list_object;
}

static void map_iterate(const mocha_context *context, const mocha_object *invokable, const mocha_object **sequence_objects, size_t count, mocha_boolean only_non_nils, resolve_callback result_callback)
{
	const mocha_sequence *sequences[8];
	size_t minimum_count = SIZE_MAX;
	for (size_t i = 0; i < count; ++i)
	{
		const mocha_sequence *sequence = mocha_object_sequence(sequence_objects[i]);
		if (sequence == 0)
		{
			MOCHA_LOG("Illegal sequence");
			return;
		}
		// MOCHA_LOG("sequence: '%s'", mocha_print_object_debug_str(sequence_objects[i]));
		sequences[i] = sequence;
		size_t seq_count = mocha_sequence_count(sequence);
		if (seq_count < minimum_count)
		{
			minimum_count = seq_count;
		}
	}
	map_func_resolve(context, invokable, sequences, count, minimum_count, only_non_nils, result_callback);
}


MOCHA_FUNCTION(keep_func)
{
	const mocha_object *invokable = arguments->objects[1];
	map_iterate(context, invokable, &arguments->objects[2], arguments->count - 2, mocha_true, result_callback);
}



MOCHA_FUNCTION(remove_func)
{
	filter_helper(context, arguments, result_callback);
}

typedef struct reduce_info
{
	const mocha_sequence *seq;
	const mocha_context *context;
	const mocha_object *invokable_object;
	const mocha_object *last_value;
	size_t count;
	size_t item_index;
	resolve_callback callback_info;
} reduce_info;

static void reduce_iterate_next(reduce_info *self);

static void reduce_iterate_done(reduce_info *self)
{
	self->callback_info.callback(self->callback_info.user_data, self->last_value);
	tyran_free(self);
}

static void reduce_iterate_item_done(void *user_data, const mocha_object *predicate_result)
{
	reduce_info *self = (reduce_info *)user_data;
	// MOCHA_LOG("filter_iterate_item_done! %d", self->item_index);
	self->last_value = predicate_result;
	self->item_index++;
	reduce_iterate_next(self);
}

static void reduce_iterate_next(reduce_info *self)
{
	if (self->item_index == self->count)
	{
		reduce_iterate_done(self);
		return;
	}
	const mocha_object *next_item = mocha_sequence_get(self->seq, self->context->values, self->item_index);
	// MOCHA_LOG("Filter index %d obj %s", self->item_index, mocha_print_object_debug_str(self->current_item));
	resolve_callback resolve_info;
	resolve_info.callback = reduce_iterate_item_done;
	resolve_info.user_data = self;
	const mocha_object *arguments[3];
	arguments[0] = 0;
	arguments[1] = self->last_value;
	arguments[2] = next_item;
	mocha_list temp_arguments;
	mocha_list_init(&temp_arguments, &self->context->values->object_references, arguments, 3);
	execute(self->context, self->invokable_object, &temp_arguments, resolve_info);
}

MOCHA_FUNCTION(reduce_func)
{
	const mocha_object *invokable_object = arguments->objects[1];
	const mocha_object *sequence_object;
	int start_index;
	const mocha_object *start_value;

	int arg_index = 2;
	if (arguments->count > 3)
	{
		start_value = arguments->objects[arg_index++];
		start_index = 0;
	}
	else
	{
		start_index = 1;
	}

	sequence_object = arguments->objects[arg_index];

	reduce_info *info = tyran_malloc(sizeof(reduce_info));
	info->seq = mocha_object_sequence(sequence_object);
	info->context = context;
	info->invokable_object = invokable_object;
	info->count = mocha_sequence_count(info->seq);
	if ((start_index == 1) && (info->count < 1))
	{
		MOCHA_LOG("Error: reduce() needs at least one in sequence");
		return;
	}
	if (start_index > 0)
	{
		start_value = mocha_sequence_get(info->seq, context->values, 0);
	}
	info->last_value = start_value;
	info->item_index = start_index;
	info->callback_info = result_callback;
	reduce_iterate_next(info);
}

MOCHA_FUNCTION(subvec_func)
{
	if (arguments->count < 3)
	{
		MOCHA_LOG("subvec needs 3 or more arguments");
		return;
	}
	const mocha_object *sequence_object = arguments->objects[1];
	const mocha_object *start_object = arguments->objects[2];
	const mocha_sequence *seq = mocha_object_sequence(sequence_object);
	int start = mocha_object_integer(start_object, "subvec_start");
	int end;

	if (arguments->count > 3)
	{
		const mocha_object *end_object = arguments->objects[3];
		end = mocha_object_integer(end_object, "subvec_end");
		if (end < start)
		{
			MOCHA_LOG("Error: subvec end wrong");
			return;
		}
		if (end > mocha_sequence_count(seq))
		{
			MOCHA_LOG("Error: subvec long after end");
			return;
		}
	}
	else
	{
		end = mocha_sequence_count(seq);
	}

	int new_size = (end - start);
	if (new_size > 64)
	{
		MOCHA_LOG("subvec() Tooo big vector!");
		return;
	}

	const mocha_object *temp[64];
	for (int vector_index = start; vector_index < end; ++vector_index)
	{
		temp[vector_index - start] = mocha_sequence_get(seq, context->values, vector_index);
	}

	const mocha_object *r = mocha_values_create_vector(context->values, temp, new_size);

	return r;
}

MOCHA_FUNCTION(vec_func)
{
	const mocha_object *sequence = arguments->objects[1];

	const mocha_object *r;

	switch (sequence->type)
	{
	case mocha_object_type_vector:
		r = sequence;
		break;
	case mocha_object_type_list:
		r = mocha_values_create_vector(context->values, sequence->data.list.objects, sequence->data.list.count);
		break;
	case mocha_object_type_map:
	{
		const mocha_map *map = &sequence->data.map;
		const mocha_object *objects[64];
		for (size_t i = 0; i < map->count; i += 2)
		{
			objects[i / 2] = mocha_values_create_vector(context->values, &map->objects[i], 2);
		}
		r = mocha_values_create_vector(context->values, objects, map->count / 2);
	}
	break;
	case mocha_object_type_nil:
		r = mocha_values_create_vector(context->values, 0, 0);
		break;
	default:
		r = 0;
	}

	return r;
}

MOCHA_FUNCTION(get_func)
{
	const mocha_object *object = arguments->objects[1];
	if (mocha_object_is_nil(object))
	{
		return mocha_values_create_nil(context->values));
	}

	if (!mocha_object_is_sequence(object))
	{
		MOCHA_LOG("Error, not a sequence!");
	}

	const mocha_object *lookup = arguments->objects[2];
	const mocha_sequence *seq = mocha_object_sequence(object);
	const mocha_object *result = mocha_sequence_get_object(seq, context->values, lookup);
	if (mocha_object_is_nil(result) && arguments->count >= 4)
	{
		result = arguments->objects[3];
	}
	return result;
}

MOCHA_FUNCTION(nth_func)
{
	const mocha_object *sequence_object = arguments->objects[1];
	if (mocha_object_is_nil(sequence_object))
	{
		return mocha_values_create_nil(context->values));
	}
	if (!mocha_object_is_sequence(sequence_object))
	{
		MOCHA_LOG("Error, not a sequence!");
	}
	const mocha_sequence *seq = mocha_object_sequence(sequence_object);

	const mocha_object *lookup = arguments->objects[2];
	if (!mocha_object_is_integer(lookup))
	{
		MOCHA_LOG("nth() must have integer index");
		return 0;
	}
	int index = mocha_object_integer(lookup, "nth_lookup");
	const mocha_object *result = mocha_sequence_get(seq, context->values, index);
	if (mocha_object_is_nil(result) && arguments->count >= 4)
	{
		result = arguments->objects[3];
	}
	return result;
}

MOCHA_FUNCTION(str_func)
{
	char temp_buf[1024];
	temp_buf[0] = 0;
	for (size_t i = 1; i < arguments->count; ++i)
	{
		const mocha_object *a = arguments->objects[i];
		const char *x = mocha_print_object_debug_str_pure(a);
		tyran_strncat(temp_buf, x, 1024);
	}

	const mocha_object *result = mocha_values_create_string_from_cstr(context->values, temp_buf);

	return result;
}

MOCHA_FUNCTION(apply_func)
{
	const mocha_object *invokable = arguments->objects[1];
	const mocha_object *new_arguments_object = arguments->objects[2];

	const mocha_sequence *seq = mocha_object_sequence(new_arguments_object);
	size_t seq_count = mocha_sequence_count(seq);
	size_t arg_count = seq_count + 1;

	mocha_list temp_arguments;
	mocha_list_init_prepare(&temp_arguments, &context->values->object_references, arg_count);
	temp_arguments.objects[0] = 0;
	for (size_t i = 0; i < seq_count; ++i)
	{
		temp_arguments.objects[i + 1] = mocha_sequence_get(seq, context->values, i);
	}
	execute(context, invokable, &temp_arguments, result_callback);
}

MOCHA_FUNCTION(shuffle_func)
{
	const mocha_object *seed_object = arguments->objects[1];
	const mocha_object *sequence_object = arguments->objects[2];

	int seed = mocha_object_integer(seed_object, "shuffle");
	const mocha_sequence *seq = mocha_object_sequence(sequence_object);

	size_t count;
	const mocha_object **source;
	mocha_sequence_get_objects(seq, &source, &count);
	if (count > 64)
	{
		MOCHA_LOG("err: Too many to shuffle!");
		return;
	}
	const mocha_object *temp[64];
	tyran_memcpy_type_n(temp, source, count);

	for (size_t i = 0; i < count; ++i)
	{
		int source_random_index = mocha_pseudo_random(seed, count);
		seed += 7919;
		int target_index = i;
		// MOCHA_LOG("Swapping: %d and %d", target_index, source_random_index)
		const mocha_object *o = temp[target_index];
		temp[target_index] = temp[source_random_index];
		temp[source_random_index] = o;
	}
	const mocha_object *result = mocha_values_create_vector(context->values, temp, count);
	return result;
}

*/

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
		const mocha_sequence* seq = mocha_object_sequence(arguments->objects[sequence_index + 1]);
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
		int index = mocha_object_integer(key, "vector_assoc");
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

	const struct mocha_object* args = mocha_runner_eval_arguments(context, &temp);
	const mocha_list* evaled_args = mocha_object_list(args);
	// printf("evaled:%s", mocha_print_object_debug_str(args));
	const mocha_object** new_key_value_pairs = evaled_args->objects;
	const mocha_object* new_seq_object;

	if (mocha_object_is_vector(sequence_object)) {
		const mocha_vector* vector = mocha_object_vector(sequence_object);
		new_seq_object = vector_assoc(vector, context->values, new_key_value_pairs, pairs_count);
	} else {
		const mocha_map* map = mocha_object_map(sequence_object);
		new_seq_object = mocha_map_assoc(map, context->values, new_key_value_pairs, pairs_count);
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
	const mocha_object* sequence = arguments->objects[1];
	const mocha_object* result;

	switch (sequence->type) {
		case mocha_object_type_list:
			result = conj_list(context->values, &sequence->data.list, &arguments->objects[2], arguments->count - 2);
			break;
		case mocha_object_type_vector:
			result = conj_vector(context->values, &sequence->data.vector, &arguments->objects[2], arguments->count - 2);
			break;
		case mocha_object_type_nil:
			result = conj_list(context->values, 0, &arguments->objects[2], arguments->count - 2);
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

const mocha_object* do_filter(const struct mocha_object* predicate_value, const struct mocha_object* item, mocha_boolean* should_add_it, mocha_boolean* should_continue)
{
	*should_add_it = mocha_object_truthy(predicate_value);
	*should_continue = mocha_true;
	return item;
}

MOCHA_FUNCTION(filter_func)
{
	return mocha_transduce_internal(context, do_filter, arguments);
}

const mocha_object* do_map(const struct mocha_object* predicate_value, const struct mocha_object* item, mocha_boolean* should_add_it, mocha_boolean* should_continue)
{
	*should_add_it = mocha_true;
	*should_continue = mocha_true;
	return predicate_value;
}

MOCHA_FUNCTION(map_func)
{
	return mocha_transduce_internal(context, do_map, arguments);
}

const mocha_object* do_keep(const struct mocha_object* predicate_value, const struct mocha_object* item, mocha_boolean* should_add_it, mocha_boolean* should_continue)
{
	*should_add_it = !mocha_object_is_nil(predicate_value);
	*should_continue = mocha_true;
	return predicate_value;
}

MOCHA_FUNCTION(keep_func)
{
	return mocha_transduce_internal(context, do_keep, arguments);
}

const mocha_object* do_remove(const struct mocha_object* predicate_value, const struct mocha_object* item, mocha_boolean* should_add_it, mocha_boolean* should_continue)
{
	*should_add_it = !mocha_object_truthy(predicate_value);
	*should_continue = mocha_true;
	return item;
}

MOCHA_FUNCTION(remove_func)
{
	return mocha_transduce_internal(context, do_remove, arguments);
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
	if (mocha_object_is_nil(seq_object)) {
		return mocha_values_create_nil(context->values);
	}
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
	MOCHA_DEF_FUNCTION(keep);
	MOCHA_DEF_FUNCTION(remove);
	MOCHA_DEF_FUNCTION(seq);
	MOCHA_DEF_FUNCTION(assoc);
	MOCHA_DEF_FUNCTION(dissoc);
	MOCHA_DEF_FUNCTION(concat);
	/*
	MOCHA_DEF_FUNCTION(cons);
	MOCHA_DEF_FUNCTION(first);
	MOCHA_DEF_FUNCTION(rest);
	MOCHA_DEF_FUNCTION(get);
	MOCHA_DEF_FUNCTION(count);
	MOCHA_DEF_FUNCTION(vec);
	MOCHA_DEF_FUNCTION(range);
	MOCHA_DEF_FUNCTION(for);
	MOCHA_DEF_FUNCTION(remove);
	MOCHA_DEF_FUNCTION(nth);
	MOCHA_DEF_FUNCTION(second);
	MOCHA_DEF_FUNCTION(repeat);
	MOCHA_DEF_FUNCTION(subvec);
	MOCHA_DEF_FUNCTION(str);
	MOCHA_DEF_FUNCTION(shuffle);
	MOCHA_DEF_FUNCTION(apply);
	MOCHA_DEF_FUNCTION_EX(empty, "empty?");
	MOCHA_DEF_FUNCTION_EX(every, "every?");
	*/
}
