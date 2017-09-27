#ifndef mocha_setup_h
#define mocha_setup_h

#include <mocha/runtime.h>
#include <mocha/values.h>
#include <mocha/hashed_strings.h>

typedef struct mocha_setup
{
	mocha_runtime runtime;
	mocha_hashed_strings hashed_strings;
	mocha_values values;
	tyran_memory memory;
} mocha_setup;

void mocha_setup_init(mocha_setup* self);

#endif
