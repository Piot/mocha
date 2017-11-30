#ifndef mocha_core_memoize_h
#define mocha_core_memoize_h

struct mocha_context;
struct mocha_values;

void mocha_core_memoize_define_context(struct mocha_context* context, struct mocha_values* values);

#endif
