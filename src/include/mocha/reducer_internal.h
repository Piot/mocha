#ifndef mocha_reducer_internal_h
#define mocha_reducer_internal_h

#include <mocha/type.h>

struct mocha_object;
struct mocha_context;
struct mocha_list;

const struct mocha_object* mocha_reducer_create_step_internal(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn init, mocha_c_fn work, const char* debug);

#endif
