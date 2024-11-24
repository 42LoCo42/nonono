#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>

#define check(cond, ...) if(cond) {err(1, __VA_ARGS__);}
#define CMD "/usr/bin/pause"

extern char** environ;

int main() {
	int fd = open(CMD, O_RDONLY);
	check(fd < 0, "open");

	uid_t uid = getuid();
	gid_t gid = getgid();

	struct passwd* pw = getpwuid(uid);
	check(chroot(pw->pw_dir) < 0, "chroot");

	check(setresuid(uid, uid, uid) < 0, "setresuid");
	check(setresgid(gid, gid, gid) < 0, "setresgid");

	char* argv[] = {CMD, NULL};
	check(fexecve(fd, argv, environ) < 0, "fexecve");
}
