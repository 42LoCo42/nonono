#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

int main() {
	struct timespec delay = {
		.tv_sec = 0,
		.tv_nsec = 1e6,
	};
	long ticks = 0;
	char buf;

	// make stdin nonblocking
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

	for(;;) {
		if(read(STDIN_FILENO, &buf, sizeof(char)) == 0) {
			printf("%ld ms\n", ticks);
		} else {
			ticks++;
			nanosleep(&delay, NULL);
		}
	}
}
