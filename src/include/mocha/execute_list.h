#ifndef mocha_execute_list_h
#define mocha_execute_list_h

#include <mocha/resolve_callback.h>

struct mocha_context;
struct mocha_list;

void execute_list(const struct mocha_context* context, const struct mocha_list* list, resolve_callback callback);

#endif
