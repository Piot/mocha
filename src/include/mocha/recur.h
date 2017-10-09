#ifndef mocha_recur_h
#define mocha_recur_h

#include <mocha/type.h>

struct mocha_object;
struct mocha_context;
struct mocha_list;
struct mocha_function;

const struct mocha_object* mocha_recur_create_step(const struct mocha_context* context, const struct mocha_list* arguments, const struct mocha_function* fn, const char* debug);

#endif
