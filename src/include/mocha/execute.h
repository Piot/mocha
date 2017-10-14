#ifndef mocha_execute_h
#define mocha_execute_h

struct mocha_object;
struct mocha_context;
struct mocha_list;
struct mocha_function;

const struct mocha_object* execute(const struct mocha_context* context, const struct mocha_object* object_fn, const struct mocha_list* arguments);
const struct mocha_object* execute_1(const struct mocha_context* context, const struct mocha_object* object_fn, const struct mocha_object* a);
const struct mocha_object* execute_2(const struct mocha_context* context, const struct mocha_object* object_fn, const struct mocha_object* a, const struct mocha_object* b);

const struct mocha_object* script_execute(const struct mocha_context* context, const struct mocha_function* script_fn, const struct mocha_list* arguments_list);
const struct mocha_context* mocha_context_create_invoke_context(const struct mocha_context* context, const struct mocha_function* fn, const struct mocha_list* arguments_list);

#endif
