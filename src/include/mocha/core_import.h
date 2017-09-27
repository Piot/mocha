#ifndef mocha_core_import_h
#define mocha_core_import_h

struct mocha_context;
struct mocha_values;

void mocha_core_import_define_context(struct mocha_context* context, struct mocha_values* values);

#endif
