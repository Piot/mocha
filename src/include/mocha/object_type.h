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
#ifndef mocha_object_type_h
#define mocha_object_type_h

typedef enum mocha_object_type {
	mocha_object_type_nil,
	mocha_object_type_list,
	mocha_object_type_map,
	mocha_object_type_vector,
	mocha_object_type_integer,
	mocha_object_type_string,
	mocha_object_type_character,
	mocha_object_type_keyword,
	mocha_object_type_true,
	mocha_object_type_symbol,
	mocha_object_type_function,
	mocha_object_type_internal_function,
	mocha_object_type_blob,
	mocha_object_type_recur,
	mocha_object_type_eval,
	mocha_object_type_context
} mocha_object_type;

#endif
