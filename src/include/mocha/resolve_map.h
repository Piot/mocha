#ifndef mocha_resolve_map_h
#define mocha_resolve_map_h

struct mocha_map;
struct mocha_context;
struct mocha_sequence;

#include <mocha/object_type.h>
#include <mocha/resolver.h>

//typedef void (*set_sequence_callback)(void* user_data, const struct mocha_object* sequence_object);

void resolve_sequence_callback(const struct mocha_context* context, const struct mocha_sequence* sequence, mocha_object_type target_type, void* user_data, set_result_callback callback_info);

#endif
