#ifndef mocha_context_h
#define mocha_context_h

#include <mocha/types.h>

struct mocha_object;
struct mocha_type;
struct mocha_values;
struct mocha_runtime;
struct mocha_keyword;
struct mocha_function;

typedef struct mocha_context {
	const struct mocha_object* map_object;
	const struct mocha_context* parent;
	const struct mocha_object* self_object;
	const struct mocha_function* script_fn;
	struct mocha_values* values;
	const char* name;
} mocha_context;

void mocha_context_print_debug(const char* debug_text, const mocha_context* self, mocha_boolean extended);
const char* mocha_context_print_debug_short(const mocha_context* self);

struct mocha_context* mocha_context_create(const mocha_context* self, const char* debug);

void mocha_context_import(mocha_context* target, const mocha_context* source, const struct mocha_keyword* prefix);

const struct mocha_object* mocha_context_lookup(const mocha_context* self, const struct mocha_object* o);

void mocha_context_add_or_replace(struct mocha_context* self, const struct mocha_object* key, const struct mocha_object* value);

void mocha_context_add(struct mocha_context* self, const struct mocha_object* key, const struct mocha_object* value);

void mocha_context_add_function(struct mocha_context* self, struct mocha_values* values, const char* name, const struct mocha_type* type);

void mocha_context_init(mocha_context* self, struct mocha_values* values, const struct mocha_object* self_object, const mocha_context* parent);

void mocha_context_deinit(mocha_context* self);

#endif
