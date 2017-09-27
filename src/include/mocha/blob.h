#ifndef mocha_blob_h
#define mocha_blob_h

#include <stdint.h>

#include <mocha/string.h>

struct tyran_memory;

typedef struct mocha_blob {
	const uint8_t* octets;
	size_t count;
	struct tyran_memory* memory;
} mocha_blob;

void mocha_blob_init(mocha_blob* self, struct tyran_memory* memory, const uint8_t* octets, size_t count);

#endif
