#include "utils.h"

void baseCallback(void* fake_arg) {
	CallbackArg* arg = fake_arg;

	if(nng_aio_result(arg->aio) == 0) {
		nng_msg* received = nng_aio_get_msg(arg->aio);
		arg->realCallback(fake_arg, received);
		nng_msg_free(received);
	}
}
