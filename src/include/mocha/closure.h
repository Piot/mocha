#ifndef mocha_closure_h
#define mocha_closure_h

struct mocha_context;
struct mocha_object;

typedef struct mocha_closure {
	const struct mocha_context* context;
	const struct mocha_object* object;
} mocha_closure;

void mocha_closure_init(mocha_closure* self, const struct mocha_context* context, const struct mocha_object* object);

#endif
