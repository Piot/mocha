#ifndef mocha_def_function_h
#define mocha_def_function_h

#include <mocha/context.h>
#include <mocha/type.h>

#define MOCHA_DEF_FUNCTION_HELPER(name, eval_arguments)                                                                                                                                                                                                        \
	static mocha_type name##_def;                                                                                                                                                                                                                              \
	name##_def.invoke = name##_func;                                                                                                                                                                                                                           \
	name##_def.eval_all_arguments = eval_arguments;

#define MOCHA_DEF_FUNCTION(name, eval_arguments)                                                                                                                                                                                                               \
	MOCHA_DEF_FUNCTION_HELPER(name, eval_arguments)                                                                                                                                                                                                            \
	mocha_context_add_function(context, values, #name, &name##_def); 

#define MOCHA_DEF_FUNCTION_EX(name, exported_name, eval_arguments)                                                                                                                                                                                            \
	MOCHA_DEF_FUNCTION_HELPER(name, eval_arguments)                                                                                                                                                                                                            \
	mocha_context_add_function(context, values, exported_name, &name##_def);

#endif
