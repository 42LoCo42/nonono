#include <ixp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils.h"

void _check(char* file, int line, int cond, char* msg) {
	if(cond) {
		fprintf(stderr, "%s:%d: %s: %s\n", file, line - 1, msg, ixp_errbuf());
		exit(1);
	}
}

IxpMsg strToIxpMsg(char* msg) {
	uint len = strlen(msg);
	IxpMsg ret = {
		.data = msg,
		.pos  = msg,
		.end  = msg + len,
		.size = len,
		.mode = 0
	};
	return ret;
}

void client(IxpConn* conn) {
	printf("handler for %d\n", conn->fd);
	char buf[16] = {0};
	ssize_t ret = read(conn->fd, buf, sizeof(buf));
	check(ret < 0, "Could not receive");
	if(ret == 0) {
		fprintf(stderr, "closed %d", conn->fd);
		ixp_hangup(conn);
		return;
	}
	printf("%.*s", (int) ret, buf);
}

void server(IxpConn* conn) {
	int fd = accept(conn->fd, NULL, 0);
	check(fd < 0, "Could not accept client");
	printf("accepted %d\n", fd);
	ixp_listen(conn->srv, fd, NULL, client, NULL);
}

