#ifndef mocha_type_h
#define mocha_type_h

struct mocha_list;
struct mocha_object;
struct mocha_context;

#define MOCHA_FUNCTION(NAME) static const struct mocha_object* NAME(const struct mocha_context* context, const struct mocha_list* arguments)

#endif
