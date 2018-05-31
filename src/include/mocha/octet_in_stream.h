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
#ifndef mocha_octet_in_stream_h
#define mocha_octet_in_stream_h

#include <stddef.h>
#include <stdint.h>

typedef struct mocha_octet_in_stream {
	const uint8_t* octets;
	const uint8_t* p;
	const uint8_t* last_p;
} mocha_octet_in_stream;

void mocha_octet_in_stream_init(mocha_octet_in_stream* self, const uint8_t* octets, size_t octet_count);
uint8_t mocha_octet_in_stream_read_uint8(mocha_octet_in_stream* self);
uint16_t mocha_octet_in_stream_read_uint16(mocha_octet_in_stream* self);
uint32_t mocha_octet_in_stream_read_uint32(mocha_octet_in_stream* self);
void mocha_octet_in_stream_read_string(mocha_octet_in_stream* self, char* buf, size_t count);
#endif
