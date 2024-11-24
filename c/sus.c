#include <fcntl.h>
#include <unistd.h>

void _start() {
	int f = open("/sys/power/state", O_WRONLY);
	write(f, "mem", 3);
	close(f);
	_exit(0);
}
