#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NNG_ELIDE_DEPRECATED
#include <nng/nng.h>
#include <nng/protocol/reqrep0/rep.h>

#include "utils.h"

void recv(CallbackArg* arg, nng_msg* msg) {
	const char* body = nng_msg_body(msg);
	const size_t len = nng_msg_len(msg);
	printf("%s\n", (char*) body);
	nng_send(arg->socket, (void*) body, len, 0);
}

int main() {
	atexit(nng_fini);
	int rv;

	CallbackArg a1 = {.realCallback = recv};
	CallbackArg a2 = {.realCallback = recv};

	nng_checked(nng_rep0_open(&a1.socket));
	nng_checked(nng_rep0_open(&a2.socket));

	nng_checked(nng_listen(a1.socket, "ws://localhost:37812/multiws/s1", NULL, 0));
	nng_checked(nng_listen(a2.socket, "ws://localhost:37812/multiws/s2", NULL, 0));

	nng_checked(nng_aio_alloc(&a1.aio, baseCallback, &a1));
	nng_checked(nng_aio_alloc(&a2.aio, baseCallback, &a2));

	nng_recv_aio(a1.socket, a1.aio);
	nng_recv_aio(a2.socket, a2.aio);

	pause();
end:
	nng_aio_free(a1.aio);
	nng_aio_free(a2.aio);
	nng_checked(nng_close(a1.socket));
	nng_checked(nng_close(a2.socket));
}
