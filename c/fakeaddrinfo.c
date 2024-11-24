#define _GNU_SOURCE

#include <dlfcn.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

static int (*orig_getaddrinfo)(
	const char* restrict node, const char* restrict service,
	const struct addrinfo* restrict hints, struct addrinfo** restrict res
);

static void __attribute__((constructor)) libInit();

static void libInit() {
	orig_getaddrinfo = dlsym(RTLD_NEXT, "getaddrinfo");
}

int getaddrinfo(
	const char* restrict node, const char* restrict service,
	const struct addrinfo* restrict hints, struct addrinfo** restrict res
) {
	printf("node = %s, service = %s\n", node, service);
	if(strcmp(node, "42loco42.duckdns.org") == 0) {
		return (*orig_getaddrinfo)("localhost", service, hints, res);
	} else {
		return (*orig_getaddrinfo)(node, service, hints, res);
	}
}
