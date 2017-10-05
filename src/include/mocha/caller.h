#ifndef mocha_caller_h
#define mocha_caller_h

struct mocha_context;
struct mocha_object;

void mocha_call_script_function_2(const struct mocha_context* context, const char* update_name, const struct mocha_object* argument, const struct  mocha_object* argument2, void* user_data);

void mocha_call_script_function(const struct mocha_context* context, const char* update_name, const struct mocha_object* argument, void* user_data);

#endif
