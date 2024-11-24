#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv) {
	if(argc < 2) errx(1, "Usage: %s <cmd> [args...]", argv[0]);

	pid_t ppid = getppid();
	char* path = NULL;
	if(asprintf(&path, "/proc/%d/fd/_", ppid) < 0)
		err(1, "Could not allocate path string");

	char* fdpos = strchr(path, '_');

	*fdpos = STDIN_FILENO + '0';
	int fd0 = open(path, O_RDONLY | O_CLOEXEC);
	if(fd0 < 0) err(1, "Could not open stdin");
	if(dup2(fd0, STDIN_FILENO) < 0) err(1, "Could not dup2 stdin");

	*fdpos = STDOUT_FILENO + '0';
	int fd1 = open(path, O_WRONLY | O_CLOEXEC);
	if(fd1 < 0) err(1, "Could not open stdout");
	if(dup2(fd1, STDOUT_FILENO) < 0) err(1, "Could not dup2 stdout");

	*fdpos = STDERR_FILENO + '0';
	int fd2 = open(path, O_WRONLY | O_CLOEXEC);
	if(fd2 < 0) err(1, "Could not open stderr");
	if(dup2(fd2, STDERR_FILENO) < 0) err(1, "Could not dup2 stderr");

	free(path);

	if(execvp(argv[1], &argv[1]) < 0) err(1, "Could not execvp");
}
