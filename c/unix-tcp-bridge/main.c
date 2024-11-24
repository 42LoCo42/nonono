#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#define check(name, cond) if(cond) {perror(name); exit(1);}

void signalHandler(int) {
	int status;
	pid_t pid = wait(&status);
	printf("child %d died in a horrible accident with status %d\n", pid, WEXITSTATUS(status));
}

// read from fd1 and write to fd2
void pump(int fd1, int fd2) {
	char buf[BUFSIZ] = {};
	for(;;) {
		ssize_t len = read(fd1, &buf, BUFSIZ);
		if(len <= 0) {
			perror("pump read");
			break;
		}

		ssize_t written = 0;
		while(written < len) {
			ssize_t now = write(fd2, buf, len);
			if(now <= 0) {
				perror("pump write");
				return;
			}
			written += now;
		}
	}

	// shutdown only the used ends
	shutdown(fd1, SHUT_RD);
	shutdown(fd2, SHUT_WR);
	exit(0);
}

// create the two data pumps
void handler(int tcp, char* path) {
	int us = -1;
	check("handler socket", (us = socket(AF_UNIX, SOCK_STREAM, 0)) < 0);

	struct sockaddr_un addr = {
		.sun_family = AF_UNIX,
	};
	memcpy(&addr.sun_path, path, strlen(path) + 1);

	check("handler connect", connect(us, (const struct sockaddr*) &addr, sizeof(addr)) < 0);

	switch(fork()) {
		case -1: perror("handler fork"); exit(1); break;
		case 0:  pump(tcp, us); break;
		default:
			pump(us, tcp);
		break;
	}
	exit(0);
}

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <socket>\n", argv[0]);
		exit(1);
	}

	check("signal", signal(SIGCHLD, &signalHandler) == SIG_ERR);
	// check("prctl", prctl(PR_SET_CHILD_SUBREAPER, 1) < 0); // more death messages

	int listener = -1;
	check("main socket", (listener = socket(AF_INET, SOCK_STREAM, 0)) < 0);

	int yes = 1;
	check("main setsockopt", setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0);

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(4713),
		.sin_addr = 0,
	};
	check("main bind", bind(listener, (const struct sockaddr*) &addr, sizeof(addr)) < 0);

	check("main listen", listen(listener, 5) < 0);

	for(;;) {
		int conn = accept(listener, NULL, 0);
		if(conn < 0) continue;

		switch(fork()) {
			case -1: perror("main fork"); exit(1); break;
			case 0:  handler(conn, argv[1]); break;
			default: close(conn);
		}
	}
}
