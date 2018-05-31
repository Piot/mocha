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
#include <mocha/log.h>
#include <mocha/nil.h>
#include <mocha/object.h>
#include <mocha/values.h>

#include <stdlib.h>

static size_t count_fn(const mocha_sequence* _self)
{
	return 0;
}

static const mocha_object* get_fn(const mocha_sequence* _self, struct mocha_values* values, size_t index)
{
	return mocha_values_create_nil(values);
}

static const mocha_object* get_object_fn(const mocha_sequence* _self, struct mocha_values* values, const mocha_object* key)
{
	return mocha_values_create_nil(values);
}

static void get_objects_fn(const mocha_sequence* _self, const mocha_object*** objects, size_t* count)
{
	*objects = 0;
	*count = 0;
}

void mocha_nil_init(mocha_nil* self)
{
	self->seq.count = count_fn;
	self->seq.get = get_fn;
	self->seq.get_object = get_object_fn;
	self->seq.get_objects = get_objects_fn;
}
