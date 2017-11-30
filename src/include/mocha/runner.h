#ifndef mocha_runner_h
#define mocha_runner_h

struct mocha_execute_step;
struct mocha_object;
struct mocha_context;
struct mocha_list;

const struct mocha_object* mocha_runner_eval(const struct mocha_context* context, const struct mocha_object* object);
const struct mocha_object* mocha_runner_eval_arguments(const struct mocha_context* context, const struct mocha_list* arguments);
const struct mocha_object* mocha_runner_eval_arguments_skip_first(const struct mocha_context* context, const struct mocha_list* arguments);
const struct mocha_object* mocha_runner_eval_arguments_rest(const struct mocha_context* context, const struct mocha_list* arguments);

#endif
