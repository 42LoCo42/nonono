#define _GNU_SOURCE

#include <dlfcn.h>
#include <stddef.h>
#include <sys/socket.h>

static int (*original_bind)(int sockfd, const struct sockaddr* addr, socklen_t addrlen) = NULL;

static void __attribute__((constructor)) libInit();

static void libInit() {
	original_bind = dlsym(RTLD_NEXT, "bind");
}

int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
	int yes = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	return ret < 0 ? ret : (*original_bind)(sockfd, addr, addrlen);
}
