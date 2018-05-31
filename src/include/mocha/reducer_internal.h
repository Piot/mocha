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
#ifndef mocha_reducer_internal_h
#define mocha_reducer_internal_h

#include <mocha/type.h>

struct mocha_object;
struct mocha_context;
struct mocha_list;
struct mocha_values;

typedef const struct mocha_object* (*mocha_c_fn_reducer_init)(struct mocha_values* values);
typedef const struct mocha_object* (*mocha_c_fn_reducer_work)(struct mocha_values* values, const struct mocha_object* a, const struct mocha_object* b);
typedef const struct mocha_object* (*mocha_c_fn_reducer_work_single)(struct mocha_values* values, const struct mocha_object* a);
typedef const struct mocha_object* (*mocha_c_fn_reducer_work_check)(struct mocha_values* values, const struct mocha_object* a, const struct mocha_object* b, mocha_boolean* should_continue);
typedef const struct mocha_object* (*mocha_c_fn_reducer_work_single_check)(struct mocha_values* values, const struct mocha_object* a, mocha_boolean* should_continue);

const struct mocha_object* mocha_reducer_reduce_internal(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_init init, mocha_c_fn_reducer_work work, const char* debug);
const struct mocha_object* mocha_reducer_reduce_internal_no_init(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work work, const char* debug);

const struct mocha_object* mocha_reducer_reduce_internal_single(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work_single single, mocha_c_fn_reducer_work work, const char* debug);
const struct mocha_object* mocha_reducer_reduce_internal_single_fn(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work_check single, const char* debug);

const struct mocha_object* mocha_reducer_reduce_internal_check(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_work_check work, const char* debug);
const struct mocha_object* mocha_reducer_reduce_internal_check_with_init(const struct mocha_context* context, const struct mocha_list* arguments, mocha_c_fn_reducer_init init, mocha_c_fn_reducer_work_single_check work, const char* debug);

#endif
