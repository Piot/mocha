#ifndef mocha_print_h
#define mocha_print_h

struct mocha_object;
struct mocha_list;
struct mocha_map;
struct mocha_values;

void mocha_print_object_debug(const struct mocha_object* o);

void mocha_print_object_debug_no_quotes(const struct mocha_object* o);

const char* mocha_print_object_debug_str(const struct mocha_object* o);
const char* mocha_print_object_debug_str_pure(const struct mocha_object* o);

const char* mocha_print_list_debug_str(struct mocha_values* values, const struct mocha_list* l);
const char* mocha_print_map_debug_str(struct mocha_values* values, const struct mocha_map* l);

#endif
