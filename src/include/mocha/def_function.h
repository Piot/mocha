#ifndef mocha_def_function_h
#define mocha_def_function_h

#include <mocha/context.h>
#include <mocha/type.h>

#define MOCHA_DEF_FUNCTION_HELPER(name)                                                                                                                                                                                                                        \
	static mocha_type name##_def;                                                                                                                                                                                                                              \
	name##_def.invoke = name##_func;

#define MOCHA_DEF_FUNCTION(name)                                                                                                                                                                                                                               \
	MOCHA_DEF_FUNCTION_HELPER(name)                                                                                                                                                                                                                            \
	mocha_context_add_function(context, values, #name, &name##_def);

#define MOCHA_DEF_FUNCTION_EX(name, exported_name)                                                                                                                                                                                                             \
	MOCHA_DEF_FUNCTION_HELPER(name)                                                                                                                                                                                                                            \
	mocha_context_add_function(context, values, exported_name, &name##_def);

#endif
