#include <tyran/tyran_clib.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/resolve_map.h>
#include <mocha/runtime.h>
#include <mocha/values.h>
#include <stdlib.h>

typedef struct resolve_sequence_info {
	mocha_list target;
	mocha_object_type target_object_type;
	void* root_user_data;
	set_result_callback callback;
	mocha_values* values;
} resolve_sequence_info;

typedef struct resolve_sequence_info_item {
	resolve_sequence_info* parent;
	int index;
} resolve_sequence_info_item;

static void sequence_completed(resolve_sequence_info* root)
{
	const mocha_object* seq = mocha_values_create_sequence(root->values, root->target_object_type, root->target.objects, root->target.count);
	root->callback(root->root_user_data, seq);
	tyran_free(root);
}

static mocha_boolean all_sequence_items_done(const resolve_sequence_info* info)
{
	for (size_t i = 0; i < info->target.count; ++i) {
		if (info->target.objects[i] == 0) {
			return mocha_false;
		}
	}

	return mocha_true;
}

static void sequence_item_ready(void* user_data, const struct mocha_object* object)
{
	resolve_sequence_info_item* self = (resolve_sequence_info_item*) user_data;
	resolve_sequence_info* root = self->parent;

	root->target.objects[self->index] = object;
	tyran_free(self);
	if (all_sequence_items_done(root)) {
		sequence_completed(root);
	}
}

void resolve_sequence_callback(const mocha_context* context, const mocha_sequence* seq, mocha_object_type target_type, void* user_data, set_result_callback callback_info)
{
	if (context == 0) {
		MOCHA_LOG("Sequence resolve context is null!");
	}
	resolve_sequence_info* info = tyran_malloc(sizeof(resolve_sequence_info));

	info->values = context->values;
	info->target_object_type = target_type;
	info->callback = callback_info;
	info->root_user_data = user_data;

	size_t seq_count = mocha_sequence_count(seq);

	mocha_list_init_prepare(&info->target, &context->values->object_references, seq_count);
	if (seq_count == 0) {
		sequence_completed(info);
		return;
	}
	for (size_t i = 0; i < seq_count; ++i) {
		const mocha_object* sequence_object = mocha_sequence_get(seq, context->values, i); // map->objects[i];

		resolve_sequence_info_item* item_info = tyran_malloc(sizeof(resolve_sequence_info_item));
		item_info->parent = info;
		item_info->index = i;

		resolve_closure_ex(context, sequence_object, item_info, sequence_item_ready);
	}
}
