#ifndef mocha_type_h
#define mocha_type_h

#include <mocha/types.h>

struct mocha_list;
struct mocha_runtime;
struct mocha_call_stack_item;
struct mocha_object;
struct mocha_context;

typedef const struct mocha_object* (*mocha_c_fn)(const struct mocha_context* context, const struct mocha_list* arguments);
#define MOCHA_FUNCTION(NAME) static const mocha_object* NAME(const struct mocha_context* context, const struct mocha_list* arguments)

typedef struct mocha_type
{
	mocha_c_fn invoke;
	mocha_boolean eval_all_arguments;
} mocha_type;

#endif
