#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/runtime.h>
#include <mocha/values.h>
#include <stdlib.h>

#include <mocha/context.h>

#include <tyran/tyran_memory.h>
#include <tyran/tyran_memory_pool.h>

static int g_marked = 0;
// static int g_deleted = 0;

/*
static void mocha_garbage_collect(mocha_runtime* self, const mocha_object** roots, size_t root_count)
{
	tyran_memory_pool* objects = self->values->objects;
	tyran_memory_pool_clear_marks(objects);
	g_marked = 0;
	g_deleted = 0;
	mark_array_and_children(self, objects, roots, root_count, 0);
	delete_unused_objects(self, objects);
	MOCHA_LOG("Summary: marked %d deleted %d", g_marked, g_deleted);
}
*/

static void mark_object_and_children(mocha_runtime *self, tyran_memory_pool *pool, const mocha_object *o, int depth);

static void mark_array_and_children(mocha_runtime *self, tyran_memory_pool *pool, const mocha_object *array[], size_t count, int depth)
{
	for (size_t i = 0; i < count; ++i)
	{
		const mocha_object *o = array[i];
		mark_object_and_children(self, pool, o, depth);
	}
}

static void mark_object_and_children(mocha_runtime *self, tyran_memory_pool *pool, const mocha_object *o, int depth)
{
	if (tyran_memory_pool_is_marked(pool, (void *)o))
	{
		return;
	}
	// MOCHA_LOG("Keep %p %s", (void*)o, mocha_print_object_debug_str(o));

	tyran_memory_pool_mark(pool, (void *)o, 1);
	g_marked++;

	switch (o->type)
	{
	case mocha_object_type_list:
	{
		const mocha_list *list = mocha_object_list(o);
		mark_array_and_children(self, pool, list->objects, list->count, depth + 1);
	}
	break;
	case mocha_object_type_map:
	{
		const mocha_map *map = mocha_object_map(o);
		mark_array_and_children(self, pool, map->objects, map->count, depth + 1);
	}
	break;
	case mocha_object_type_vector:
	{
		const mocha_vector *vector = mocha_object_vector(o);
		mark_array_and_children(self, pool, vector->objects, vector->count, depth + 1);
	}
	break;
	case mocha_object_type_function:
	{
		const mocha_function *function = mocha_object_function(o);
		mark_object_and_children(self, pool, function->arguments, depth + 1);
		mark_object_and_children(self, pool, function->code, depth + 1);
		mark_object_and_children(self, pool, function->context->self_object, depth + 1);
	}
	break;
	case mocha_object_type_context:
	{
		const mocha_context *context = mocha_object_context(o);
		// MOCHA_LOG("context_object:%p  context:%p  map_object:%p", (void*) o, (void*) context, (void*)context->map_object);
		const mocha_object *map_object = context->map_object;
		//const mocha_map* map = mocha_object_map(context->map_object);
		mark_object_and_children(self, pool, map_object, depth + 1);
		if (context->parent != 0)
		{
			mark_object_and_children(self, pool, context->parent->self_object, depth + 1);
		}
	}
	break;
	default:
		break;

	}
}
/*
static void delete_unused_objects(mocha_runtime* self, tyran_memory_pool* pool)
{
	for (size_t i = 0; i < pool->max_count; ++i) {
		tyran_memory_pool_entry* e = &pool->entries[i];
		if (e->allocated && !e->marked_as_keep && e->generation <= 1) {
			void* p = tyran_memory_pool_pointer(pool, i);
			const mocha_object* o = (const mocha_object*) p;
			mocha_values_delete(self->values, o);
			g_deleted++;
		}
	}
}

*/
