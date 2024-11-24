#ifndef UTILS_H_
#define UTILS_H_

#include <nng/nng.h>

#define nng_checked(b)							\
	if((rv = (b)) != 0) {						\
		fprintf(								\
			stderr,								\
			"error at %s:%d: %s\n",				\
			__FILE__, __LINE__,					\
			nng_strerror(rv));					\
		goto end;								\
	}

struct _CallbackArg;

typedef void (*RealCallbackFunction)(struct _CallbackArg* arg, nng_msg*);

typedef struct _CallbackArg {
	nng_aio* aio;
	nng_socket* socket;
	RealCallbackFunction realCallback;
} CallbackArg;

void baseCallback(void* fake_arg);

#endif // UTILS_H_
