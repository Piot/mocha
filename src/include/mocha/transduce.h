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
#ifndef mocha_transduce_h
#define mocha_transduce_h

#include <mocha/types.h>

struct mocha_context;
struct mocha_object;
struct mocha_list;

typedef void (*mocha_transducer_adder_add_fn)(void* user_data, const struct mocha_object* result_item);
typedef const struct mocha_object* (*mocha_transducer_adder_done_fn)(void* user_data);

typedef struct mocha_transducer_adder {
	mocha_transducer_adder_add_fn add_fn;
	mocha_transducer_adder_done_fn done_fn;
	void* user_data;
} mocha_transducer_adder;

void mocha_transducer_adder_call(mocha_transducer_adder* self, const struct mocha_object* item);
typedef const struct mocha_object* (*mocha_transducer_stepper)(const struct mocha_object* predicate_value, const struct mocha_object* item, mocha_boolean* should_add_it, mocha_boolean* should_continue);

const struct mocha_object* mocha_transduce(const struct mocha_context* context, const struct mocha_object* fn, void* user_data, mocha_transducer_adder adder, const struct mocha_object* state, const struct mocha_object* item);
const struct mocha_object* mocha_transduce_internal(const struct mocha_context* context, mocha_transducer_stepper stepper, const struct mocha_list* list, tyran_boolean include_index);

#endif
