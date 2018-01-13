#ifndef mocha_fnv_hash_h
#define mocha_fnv_hash_h

#include <stdint.h>

typedef uint32_t mocha_string_hash;

mocha_string_hash mocha_fnv_hash(const char* str);

#endif
