#ifndef mocha_def_function_h
#define mocha_def_function_h

#include <mocha/context.h>
#include <mocha/type.h>

#define MOCHA_DEF_FUNCTION(name)                                                                                                                                                                                                               \
	mocha_context_add_function(context, values, #name, &name ## _func);

#define MOCHA_DEF_FUNCTION_EX(name, exported_name)                                                                                                                                                                                            \
	mocha_context_add_function(context, values, exported_name, &name ## _func);

#endif
