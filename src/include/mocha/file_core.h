#ifndef mocha_file_core_h
#define mocha_file_core_h
#include <stdint.h>

struct file_loader;

typedef void (*file_loader_callback)(void* user_data, const uint8_t* octets, int32_t result);
void file_loader_load(struct file_loader* self, const char* url, void* user_data, file_loader_callback callback);

#endif
