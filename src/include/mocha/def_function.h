/*

MIT License

Copyright (c) 2013 Peter Bjorklund

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
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
