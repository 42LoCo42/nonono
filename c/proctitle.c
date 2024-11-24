#include <bsd/unistd.h>

int main(int argc, char** argv, char** envp) {
	setproctitle_init(argc, argv, envp);
	setproctitle("foo: %d\n", 42);
	pause();
}
