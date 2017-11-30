#ifndef mocha_nil_h
#define mocha_nil_h

#include <mocha/sequence.h>
#include <mocha/types.h>

struct mocha_object;
struct tyran_memory;

typedef struct mocha_nil {
	mocha_sequence seq;
} mocha_nil;

void mocha_nil_init(mocha_nil* self);

#endif
