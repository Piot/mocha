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

typedef struct mocha_type {
	mocha_c_fn invoke;
	mocha_boolean eval_all_arguments;
} mocha_type;

#endif
