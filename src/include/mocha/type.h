#ifndef mocha_type_h
#define mocha_type_h

#include <mocha/object.h>
#include <mocha/types.h>
#include <mocha/resolver.h>

struct mocha_list;
struct mocha_runtime;
struct mocha_call_stack_item;

typedef void (*mocha_c_fn)(const resolve_callback result_callback, const struct mocha_context* context, const struct mocha_list* arguments);
#define MOCHA_FUNCTION(NAME) static void NAME(resolve_callback result_callback, const struct mocha_context* context, const struct mocha_list* arguments)

typedef struct mocha_type
{
	mocha_c_fn invoke;
	mocha_boolean eval_all_arguments;
} mocha_type;

#endif
