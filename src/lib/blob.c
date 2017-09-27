#include <mocha/blob.h>
#include <tyran/tyran_memory.h>
#include <tyran/tyran_clib.h>


void mocha_blob_init(mocha_blob* self, struct tyran_memory* memory, const uint8_t* octets, size_t count)
{
	uint8_t* buffer = TYRAN_MEMORY_ALLOC_TYPE_COUNT(memory, uint8_t, count);

	tyran_memcpy_octets(buffer, octets, count);
	self->memory = memory;
	self->octets = buffer;
	self->count = count;
}

