#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SEP ":::"

#define safe(cmd, ...) {if(cmd < 0) err(1, __VA_ARGS__);}

#define exec(args) { \
	if(execvp(*args, args) < 0) \
		err(1, "%s: command not found", *args); \
}

#define closeall \
	close(pipeFrom1[0]); \
	close(pipeFrom1[1]); \
	close(pipeFrom2[0]); \
	close(pipeFrom2[1]);

void printUsage(char* name) {
	errx(1, "Usage: %s <cmd> [args] %s <cmd> [args]\n", name, SEP);
}

int main(int argc, char** argv) {
	int second = -1;
	for(int i = 1; i < argc; ++i) {
		if(strcmp(argv[i], SEP) == 0) {
			argv[i] = NULL;
			second = i + 1;
			break;
		}
	}
	if(second == -1 || second == argc) printUsage(argv[0]);

	char** cmd1 = argv + 1;
	char** cmd2 = argv + second;

	int pipeFrom1[2];
	int pipeFrom2[2];

	safe(pipe(pipeFrom1), "Could not create first pipe");
	safe(pipe(pipeFrom2), "Could not create first pipe");

	switch(fork()) {
	case -1: err(1, "Could not fork");
	case  0:
		safe(dup2(pipeFrom1[1], STDOUT_FILENO), "Could not create stdout pipe");
		safe(dup2(pipeFrom2[0], STDIN_FILENO),  "Could not create stdin pipe");
		closeall;
		exec(cmd1);
		break;
	default:
		safe(dup2(pipeFrom2[1], STDOUT_FILENO), "Could not create stdout pipe");
		safe(dup2(pipeFrom1[0], STDIN_FILENO),  "Could not create stdin pipe");
		closeall;
		exec(cmd2);
	}
}
