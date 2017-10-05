#ifndef mocha_execute_h
#define mocha_execute_h

struct mocha_object;
struct mocha_context;
struct mocha_list;

const struct mocha_object *execute(const struct mocha_context *context, const struct mocha_object *object_fn, const struct mocha_list *arguments);

#endif
