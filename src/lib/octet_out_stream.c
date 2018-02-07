#include <mocha/octet_out_stream.h>
#include <tyran/tyran_clib.h>
#include <tyran/tyran_log.h>

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
		TYRAN_ERROR("Problem");
		return;
	}

	*self->p++ = v;
}

void mocha_octet_out_stream_write_uint16(mocha_octet_out_stream* self, uint16_t v)
{
	if (self->p + 2 >= self->last_p) {
		TYRAN_ERROR("Problem");
		return;
	}

	*((uint16_t*) self->p) = tyran_htons(v);
	self->p += 2;
}

void mocha_octet_out_stream_write_uint32(mocha_octet_out_stream* self, uint32_t v)
{
	if (self->p + 4 >= self->last_p) {
		TYRAN_ERROR("Problem");
		return;
	}

	*((uint32_t*) self->p) = tyran_htonl(v);
	self->p += 4;
}

void mocha_octet_out_stream_write_octets(mocha_octet_out_stream* self, const uint8_t* octets, size_t octet_count)
{
	if (self->p + octet_count > self->last_p) {
		TYRAN_ERROR("Wrote too far");
		return;
	}
	tyran_memcpy_octets(self->p, octets, octet_count);
	self->p += octet_count;
}

void mocha_octet_out_stream_write_string(mocha_octet_out_stream* self, const char* x)
{
	mocha_octet_out_stream_write_octets(self, x, tyran_strlen(x));
}

size_t mocha_octet_out_stream_flush(mocha_octet_out_stream* self)
{
	return self->p - self->octets;
}
