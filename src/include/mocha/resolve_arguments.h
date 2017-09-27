#ifndef mocha_resolve_arguments_h
#define mocha_resolve_arguments_h

struct mocha_list;
struct mocha_context;

typedef void (*set_arguments_callback)(void* user_data, const struct mocha_list* list);

void resolve_arguments_callback(const struct mocha_context* context, const struct mocha_list* arguments, void* user_data, set_arguments_callback callback_info);

#endif
