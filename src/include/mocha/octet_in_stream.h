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
