#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void sendChar(int term, char c) {
	if(ioctl(term, TIOCSTI, &c) < 0) {
		close(term);
		err(3, "ioctl(TIOCSTI)");
	}
}

int main(int argc, char** argv) {
	if(argc < 2) {
		errx(1, "Usage: <terminal> [text]");
	}

	int term = open(argv[1], O_WRONLY);
	if(term < 0) {
		err(2, "Could not open file %s", argv[1]);
	}

	char c;
	if(argc < 3) {
		// read from stdin
		ssize_t r;
		while((r = read(0, &c, 1)) > 0) {
			sendChar(term, c);
		}

		if(r < 0) {
			close(term);
			err(4, "read");
		}
	} else {
		// read from argv[2]
		size_t i = 0;
		while((c = argv[2][i++]) != 0) {
			sendChar(term, c);
		}
	}

	close(term);
}
