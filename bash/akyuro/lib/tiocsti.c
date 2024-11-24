#include <unistd.h>
#include <sys/ioctl.h>
#include <err.h>

int main(void) {
	char c;
	ssize_t r;
	while((r = read(0, &c, 1)) > 0) {
		if(ioctl(1, TIOCSTI, &c)) {
			err(1, "ioctl(TIOCSTI)");
		}
	}
	if(r < 0) {
		err(1, "read");
	}
	return 0;
}
