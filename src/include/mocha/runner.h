#ifndef mocha_runner_h
#define mocha_runner_h

struct mocha_execute_step;
struct mocha_object;
struct mocha_context;

typedef struct mocha_runner
{
	const struct mocha_execute_step *steps[20];
	int steps_count;
} mocha_runner;

void mocha_runner_init(mocha_runner* self);
const struct mocha_object *mocha_runner_eval(mocha_runner* self, const struct mocha_context *context, const struct mocha_object *object);

#endif
