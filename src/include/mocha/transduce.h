#ifndef mocha_transduce_h
#define mocha_transduce_h

struct mocha_context;
struct mocha_object;

typedef void (*mocha_transducer_adder_add_fn)(void* user_data, const struct mocha_object* result_item);
typedef const struct mocha_object* (*mocha_transducer_adder_done_fn)(void* user_data);

typedef struct mocha_transducer_adder {
	mocha_transducer_adder_add_fn add_fn;
	mocha_transducer_adder_done_fn done_fn;
	void* user_data;
} mocha_transducer_adder;

void mocha_transducer_adder_call(mocha_transducer_adder* self, const struct mocha_object* item);

typedef void (*mocha_transducer_stepper)(void* user_data, mocha_transducer_adder* adder, const struct mocha_object* invokable_predicate, const struct mocha_object* result_item);

const struct mocha_object* mocha_transduce(const struct mocha_context* context, const struct mocha_object* fn, void* user_data, mocha_transducer_adder adder, const struct mocha_object* state, const struct mocha_object* item);

#endif
