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
#ifndef mocha_print_h
#define mocha_print_h

struct mocha_object;
struct mocha_list;
struct mocha_map;
struct mocha_values;

void mocha_print_object_debug(const struct mocha_object* o);

void mocha_print_object_debug_no_quotes(const struct mocha_object* o);

const char* mocha_print_object_debug_str(const struct mocha_object* o);
const char* mocha_print_object_debug_str_pure(const struct mocha_object* o);
size_t mocha_print_object_edn_buf(char* buf, size_t max_len, const struct mocha_object* o);

const char* mocha_print_list_debug_str(struct mocha_values* values, const struct mocha_list* l);
const char* mocha_print_map_debug_str(struct mocha_values* values, const struct mocha_map* l);

#endif
