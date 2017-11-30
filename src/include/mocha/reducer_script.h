#ifndef mocha_reducer_script_h
#define mocha_reducer_script_h

#include <mocha/type.h>

struct mocha_object;
struct mocha_context;
struct mocha_list;

const struct mocha_object* mocha_reducer_reduce_script(const struct mocha_context* context, const struct mocha_list* arguments, const char* debug);

#endif
