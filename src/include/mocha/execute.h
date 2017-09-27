#ifndef mocha_execute_h
#define mocha_execute_h

#include <mocha/resolve_callback.h>

struct mocha_context;
struct mocha_function;
struct mocha_list;
struct mocha_object;

void script_execute(const struct mocha_context* context, const struct mocha_function* script_fn, const struct mocha_list* arguments_list, resolve_callback resolve);
void execute(const struct mocha_context* context, const struct mocha_object* object_fn, const struct mocha_list* arguments, resolve_callback resolve);

#endif
