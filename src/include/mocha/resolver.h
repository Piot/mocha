#ifndef mocha_resolver_h
#define mocha_resolver_h

#include <mocha/resolve_callback.h>

struct mocha_closure;
struct mocha_context;
struct mocha_function;
struct mocha_list;
struct mocha_object;

void resolve_closure_ex(const struct mocha_context* context, const struct mocha_object* object, void* user_data, set_result_callback callback);
void resolve_closure(const struct mocha_closure* self, resolve_callback callback);

#endif
