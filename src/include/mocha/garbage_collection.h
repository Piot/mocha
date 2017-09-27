#ifndef mocha_garbage_collect_h
#define mocha_garbage_collect_h

struct mocha_runtime;
struct mocha_object;

void mocha_garbage_collect(mocha_runtime* self, const mocha_object** roots, size_t root_count);

#endif
