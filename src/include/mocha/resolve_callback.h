#ifndef resolve_callback_h
#define resolve_callback_h

struct mocha_object;

typedef void (*set_result_callback)(void* user_data, const struct mocha_object* object);

typedef struct resolve_callback {
	set_result_callback callback;
	void* user_data;
} resolve_callback;

#endif
