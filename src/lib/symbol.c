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
#include <mocha/symbol.h>
#include <mocha/types.h>

void mocha_symbol_init(mocha_symbol* self, mocha_string_hash hash)
{
	self->name_hash = hash;
	self->namespace_hash = 0;
	self->total_hash = hash;
	self->has_namespace = mocha_false;
}

void mocha_symbol_init_with_namespace(mocha_symbol* self, mocha_string_hash namespace_hash, mocha_string_hash name_hash, mocha_string_hash total_hash)
{
	self->namespace_hash = namespace_hash;
	self->name_hash = name_hash;
	self->total_hash = total_hash;
	self->has_namespace = mocha_true;
}
