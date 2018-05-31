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
#ifndef mocha_transducers_step_h
#define mocha_transducers_step_h

struct mocha_values;
struct mocha_list;
struct imprint_memory;

#include <mocha/transduce.h>

#include <mocha/type.h>

// static const mocha_object *mocha_transducers_step_create(mocha_values *values, const mocha_list *arguments, mocha_c_fn init, mocha_c_fn work, const char *debug)
const struct mocha_object* mocha_transducers_step_create_internal_list(struct mocha_values* values, struct imprint_memory* memory, const struct mocha_list* arguments, mocha_transducer_stepper stepper, void* user_data, const char* debug);

#endif
