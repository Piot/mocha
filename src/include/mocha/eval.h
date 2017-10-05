#ifndef mocha_eval_h
#define mocha_eval_h

struct mocha_context;
struct mocha_object;

const struct mocha_object* mocha_eval_string(struct mocha_context* parse_context, const char* s);

#endif
