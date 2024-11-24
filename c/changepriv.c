#include <err.h>
#include <netinet/ip.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define check(msg, x) if(x) {err(1, msg);}

int main(int argc, char** argv) {
	if(argc < 2) {
		errx(1, "Usage: %s username", argv[0]);
	}

	int s = 0;
	check("socket", (s = socket(AF_INET, SOCK_STREAM, 0)) < 0);

	int yes = 1;
	check("setsockopt", setsockopt(s, SOL_SOCKET,
		SO_REUSEADDR, &yes, sizeof yes) < 0);

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(80),
		.sin_addr = 0,
	};
	check("bind", bind(s, (struct sockaddr*) &addr, sizeof addr) < 0);

	struct passwd* user = NULL;
	check("getpwnam", (user = getpwnam(argv[1])) == NULL);

	printf("%s is uid %d gid %d\n",
		argv[1],
		user->pw_uid,
		user->pw_gid
	);

	check("setgid", setgid(user->pw_gid) < 0);
	check("setuid", setuid(user->pw_uid) < 0);

	check("listen", listen(s, 1) < 0);

	int c = 0;
	check("accept", (c = accept(s, NULL, NULL)) < 0);

	check("write", write(c, "foo\n", 4) != 4);

	switch(fork()) {
		case 0:
			check("execlp", execlp("pause", "pause", "a", "b", NULL) < 0);
			break;
		case -1:
			err(1, "fork");
			break;
	}

	check("wait", wait(NULL) < 0);

	close(c);
	close(s);
}
