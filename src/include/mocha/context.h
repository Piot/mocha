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
#ifndef mocha_context_h
#define mocha_context_h

#include <mocha/types.h>

struct mocha_object;
struct mocha_type;
struct mocha_values;
struct mocha_runtime;
struct mocha_keyword;
struct mocha_function;

typedef struct mocha_context {
	const struct mocha_object* map_object;
	const struct mocha_context* parent;
	const struct mocha_object* self_object;
	const struct mocha_function* script_fn;
	struct mocha_values* values;
	struct mocha_runtime* runtime;
	const char* name;
} mocha_context;

void mocha_context_print_debug(const char* debug_text, const mocha_context* self, mocha_boolean extended);

const char* mocha_context_print_debug_short(const mocha_context* self);

struct mocha_context* mocha_context_create(const mocha_context* self, const char* debug);

void mocha_context_import(mocha_context* target, const mocha_context* source, const struct mocha_keyword* prefix);

const struct mocha_object* mocha_context_lookup(const mocha_context* self, const struct mocha_object* o);

void mocha_context_add_or_replace(struct mocha_context* self, const struct mocha_object* key, const struct mocha_object* value);

void mocha_context_add(struct mocha_context* self, const struct mocha_object* key, const struct mocha_object* value);

void mocha_context_add_function(struct mocha_context* self, struct mocha_values* values, const char* name, const struct mocha_type* type);

void mocha_context_init(mocha_context* self, struct mocha_values* values, const struct mocha_object* self_object, const mocha_context* parent);

void mocha_context_deinit(mocha_context* self);

#endif
