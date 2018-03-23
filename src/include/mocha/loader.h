#ifndef mocha_loader_h
#define mocha_loader_h

#include <stdint.h>

struct pnacl_loader;

typedef struct mocha_loader {
	struct pnacl_loader* loader;
} mocha_loader;

/*
   typedef void (*mocha_loader_load_fn_callback)(const void* user_data, uint8_t* octets, int32_t result);
   typedef void (*mocha_loader_load_fn)(const void* user_data, const char* resource, mocha_loader_load_fn_callback callback);
 */

#endif
