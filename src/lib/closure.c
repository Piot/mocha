#include <mocha/closure.h>
#include <mocha/log.h>
void mocha_closure_init(mocha_closure* self, const struct mocha_context* context, const struct mocha_object* object)
{
	if (self == 0) {
		MOCHA_LOG("Closure init is null!");
	}
	self->context = context;
	self->object = object;
}
