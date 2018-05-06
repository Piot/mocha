#include <clog/clog.h>
#include <mocha/octet_in_stream.h>

void mocha_octet_in_stream_init(mocha_octet_in_stream* self, const uint8_t* octets, size_t octet_count)
{
	self->p = self->octets = octets;
	self->last_p = self->p + octet_count;
}

uint8_t mocha_octet_in_stream_read_uint8(mocha_octet_in_stream* self)
{
	if (self->p > self->last_p) {
		CLOG_ERROR("Problem");
		return 0;
	}

	return *self->p++;
}

uint16_t mocha_octet_in_stream_read_uint16(mocha_octet_in_stream* self)
{
	if (self->p + 2 > self->last_p) {
		CLOG_ERROR("Problem");
		return 0;
	}

	uint16_t v = tc_ntohs(*(uint16_t*) self->p);
	self->p += 2;
	return v;
}

uint32_t mocha_octet_in_stream_read_uint32(mocha_octet_in_stream* self)
{
	if (self->p + 4 > self->last_p) {
		CLOG_ERROR("Problem");
		return 0;
	}

	uint32_t v = tc_ntohl(*(uint32_t*) self->p);
	self->p += 4;
	return v;
}

void mocha_octet_in_stream_read_string(mocha_octet_in_stream* self, char* buf, size_t count)
{
	for (size_t i = 0; i < count; ++i) {
		buf[i] = mocha_octet_in_stream_read_uint8(self);
	}
}
