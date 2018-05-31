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
#ifndef mocha_execute_h
#define mocha_execute_h

struct mocha_object;
struct mocha_context;
struct mocha_list;
struct mocha_function;

const struct mocha_object* execute(const struct mocha_context* context, const struct mocha_object* object_fn, const struct mocha_list* arguments);
const struct mocha_object* execute_0(const struct mocha_context* context, const struct mocha_object* object_fn);
const struct mocha_object* execute_1(const struct mocha_context* context, const struct mocha_object* object_fn, const struct mocha_object* a);
const struct mocha_object* execute_2(const struct mocha_context* context, const struct mocha_object* object_fn, const struct mocha_object* a, const struct mocha_object* b);
const struct mocha_object* execute_3(const struct mocha_context* context, const struct mocha_object* object_fn, const struct mocha_object* a, const struct mocha_object* b, const struct mocha_object* c);

const struct mocha_context* mocha_context_create_invoke_context(const struct mocha_context* context, const struct mocha_function* fn, const struct mocha_list* arguments_list);

const struct mocha_object* mocha_script_execute_2(const struct mocha_context* context, const char* function_name, const struct mocha_object* a, const struct mocha_object* b);
const struct mocha_object* mocha_script_execute_1(const struct mocha_context* context, const char* function_name, const struct mocha_object* a);
const struct mocha_object* script_function_object_from_c_string(const struct mocha_context* context, const char* function_name);

#endif
