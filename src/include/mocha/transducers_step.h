#ifndef mocha_transducers_step_h
#define mocha_transducers_step_h

struct mocha_values;
struct mocha_list;
struct tyran_memory;

#include <mocha/transduce.h>

#include <mocha/type.h>

// static const mocha_object *mocha_transducers_step_create(mocha_values *values, const mocha_list *arguments, mocha_c_fn init, mocha_c_fn work, const char *debug)
const struct mocha_object* mocha_transducers_step_create_internal_list(struct mocha_values* values, struct tyran_memory* memory, const struct mocha_list* arguments, mocha_transducer_stepper stepper, void* user_data, const char* debug);

#endif
