#ifndef mocha_copy_object_h
#define mocha_copy_object_h

struct mocha_object;
struct mocha_values;

const struct mocha_object* mocha_values_deep_copy(struct mocha_values* self, const struct mocha_object* object_to_copy);

#endif
