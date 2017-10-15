#ifndef mocha_reducer_internal_h
#define mocha_reducer_internal_h

#include <mocha/type.h>

struct mocha_object;
struct mocha_context;
struct mocha_list;
struct mocha_values;

typedef const struct mocha_object* (*mocha_c_fn_reducer_init)(struct mocha_values* values);
typedef const struct mocha_object* (*mocha_c_fn_reducer_work)(struct mocha_values* values, const struct mocha_object* a, const struct mocha_object* b);
typedef const struct mocha_object* (*mocha_c_fn_reducer_work_single)(struct mocha_values* values, const struct mocha_object* a);
typedef const struct mocha_object* (*mocha_c_fn_reducer_work_check)(struct mocha_values* values, const struct mocha_object* a, const struct mocha_object* b, mocha_boolean* should_continue);

typedef const struct mocha_object* (*mocha_c_fn_reducer_check)(struct mocha_values* values, const struct mocha_object* a, mocha_boolean* should_continue);

const struct mocha_object* mocha_reducer_reduce_internal(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_init init, mocha_c_fn_reducer_work work, const char* debug);
const struct mocha_object* mocha_reducer_reduce_internal_no_init(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work work, const char* debug);

const struct mocha_object* mocha_reducer_reduce_internal_single(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work_single single, mocha_c_fn_reducer_work work, const char* debug);

const struct mocha_object* mocha_reducer_reduce_internal_check(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work_check work, const char* debug);
const struct mocha_object* mocha_reducer_reduce_internal_check_with_init(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_init init, mocha_c_fn_reducer_check work, const char* debug);

#endif
