#include <err.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv) {
	if(argc < 2) errx(1, "Usage: %s <program> [arguments...]", argv[0]);

	if(prctl(PR_SET_CHILD_SUBREAPER, 1, 0, 0, 0) < 0) err(1, "Could not activate subreaper");

	switch(fork()) {
		case 0:
			execvp(argv[1], argv + 1);
			err(1, "Could not exec child");
		case -1:
			err(1, "Could not fork");
	}

	for(;;) {
		if(wait(0) < 0) err(1, "wait");
	}
}
