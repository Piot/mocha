#ifndef mocha_octet_out_stream_h
#define mocha_octet_out_stream_h

#include <stddef.h>
#include <stdint.h>

typedef struct mocha_octet_out_stream {
	uint8_t* octets;
	uint8_t* p;
	const uint8_t* last_p;
	size_t buffer_size;
} mocha_octet_out_stream;

void mocha_octet_out_stream_rewind(mocha_octet_out_stream* self);
void mocha_octet_out_stream_init(mocha_octet_out_stream* self, uint8_t* buffer, size_t buf_size);
size_t mocha_octet_out_stream_count(const mocha_octet_out_stream* self);

void mocha_octet_out_stream_write_uint8(mocha_octet_out_stream* self, uint8_t v);
void mocha_octet_out_stream_write_uint16(mocha_octet_out_stream* self, uint16_t v);
void mocha_octet_out_stream_write_uint32(mocha_octet_out_stream* self, uint32_t v);
void mocha_octet_out_stream_write_octets(mocha_octet_out_stream* self, const uint8_t* octets, size_t octet_count);
size_t mocha_octet_out_stream_flush(mocha_octet_out_stream* self);
void mocha_octet_out_stream_write_string(mocha_octet_out_stream* self, const char* x);

#endif
