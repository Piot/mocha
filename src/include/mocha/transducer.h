#ifndef mocha_transducer_h
#define mocha_transducer_h

#include <mocha/function.h>
#include <mocha/type.h>

struct mocha_object;

typedef struct mocha_transducer {
	const struct mocha_function* fn;
	mocha_c_fn c_fn;
	const char* debug_name;
} mocha_transducer;

void mocha_transducer_init_internal(mocha_transducer* self, mocha_c_fn fn, const char* debug_name);
void mocha_transducer_init_script(mocha_transducer* self, const mocha_function* fn);

#endif
