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
#include <clog/clog.h>
#include <mocha/octet_out_stream.h>
#include <tiny-libc/tiny_libc.h>

void mocha_octet_out_stream_rewind(mocha_octet_out_stream* self)
{
	self->p = self->octets;
}

void mocha_octet_out_stream_init(mocha_octet_out_stream* self, uint8_t* buffer, size_t buf_size)
{
	self->octets = buffer;
	self->p = buffer;
	self->buffer_size = buf_size;
	self->last_p = self->p + buf_size;
	mocha_octet_out_stream_rewind(self);
}

size_t mocha_octet_out_stream_count(const mocha_octet_out_stream* self)
{
	return self->p - self->octets;
}

void mocha_octet_out_stream_write_uint8(mocha_octet_out_stream* self, uint8_t v)
{
	if (self->p >= self->last_p) {
		CLOG_ERROR("Problem");
		return;
	}

	*self->p++ = v;
}

void mocha_octet_out_stream_write_uint16(mocha_octet_out_stream* self, uint16_t v)
{
	if (self->p + 2 >= self->last_p) {
		CLOG_ERROR("Problem");
		return;
	}

	*((uint16_t*) self->p) = tc_htons(v);
	self->p += 2;
}

void mocha_octet_out_stream_write_uint32(mocha_octet_out_stream* self, uint32_t v)
{
	if (self->p + 4 >= self->last_p) {
		CLOG_ERROR("Problem");
		return;
	}

	*((uint32_t*) self->p) = tc_htonl(v);
	self->p += 4;
}

void mocha_octet_out_stream_write_octets(mocha_octet_out_stream* self, const uint8_t* octets, size_t octet_count)
{
	if (self->p + octet_count > self->last_p) {
		CLOG_ERROR("Wrote too far");
		return;
	}
	tc_memcpy_octets(self->p, octets, octet_count);
	self->p += octet_count;
}

void mocha_octet_out_stream_write_string(mocha_octet_out_stream* self, const char* x)
{
	mocha_octet_out_stream_write_octets(self, x, tc_strlen(x));
}

size_t mocha_octet_out_stream_flush(mocha_octet_out_stream* self)
{
	return self->p - self->octets;
}
