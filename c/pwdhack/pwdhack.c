#include <err.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define VAR_PORT "PWDHACK_PORT"
#define VAR_ROOT "PWDHACK_ROOT"

static void* child(void*) {
	// get port & root
	char* portStr = getenv(VAR_PORT);
	if(portStr == NULL) errx(1, "%s has not been defined", VAR_PORT);
	int port = atoi(portStr);

	char* rootStr = getenv(VAR_ROOT);
	if(rootStr == NULL) errx(1, "%s has not beed defined", VAR_ROOT);
	size_t rootLen = strlen(rootStr);

	// start listener
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) err(1, "Could not create socket");

	int yes = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
		err(1, "Could not set SO_REUSEADDR");

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr = 0,
	};
	if(bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0)
		err(1, "Could not bind to port %d", port);

	if(listen(sock, 1) < 0) err(1, "Could not listen");

	for(;;) {
		// accept client
		int fd = accept(sock, NULL, NULL);
		if(fd < 0) break;

		FILE* file = fdopen(fd, "r");

		// get path from client
		char* path = NULL;
		size_t ignored;
		ssize_t len = getline(&path, &ignored, file);
		if(len < 0) {
			warnx("Could not read new path");
			goto end;
		}

		// remove trailing linebreak
		if(path[len - 1] == '\n') {
			path[len - 1] = 0;
			len--;
		}

		// prepend root if path starts with /
		if(path[0] == '/') {
			path = realloc(path, rootLen + len + 1);
			memmove(path + rootLen, path, len);
			memcpy(path, rootStr, rootLen);
			len = rootLen + len;
		}

		if(chdir(path) < 0) warn("Could not chdir() to %s", path);

end:
		fclose(file);
		free(path);
	}
	return NULL;
}

static void __attribute__((constructor)) libInit();
static void libInit() {
	pthread_t tid;
	pthread_create(&tid, NULL, &child, NULL);
	pthread_detach(tid);
}
