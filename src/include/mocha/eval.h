#ifndef mocha_eval_h
#define mocha_eval_h

#include <mocha/resolver.h>

struct mocha_context;

void mocha_eval_string(struct mocha_context* parse_context, const char* s, void* user_data, set_result_callback callback);

#endif
