#include <err.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void echoOn() {
	system("stty echo");
}

int main(int argc, char** argv) {
	if(argc < 2) errx(1, "Usage: %s /dev/input/event<N>", argv[0]);

	atexit(echoOn);
	system("stty -echo");

	int fd = open(argv[1], O_RDONLY);
	if(fd < 0) err(1, "Could not open %s", argv[1]);

	const char* valueNames[] = {
		"released",
		"pressed",
		"held down",
	};

	for(;;) {
		struct input_event ev = {};
		read(fd, &ev, sizeof(ev));

		// if((ev.type | ev.code | ev.value) == 0) continue;
		if(ev.type != 1) continue;

		char* time = ctime(&ev.time.tv_sec);
		time[strlen(time) - 1] = 0;

		printf("%s %6lu: code %3hu was %s\n",
			time,
			ev.time.tv_usec,
			ev.code,
			valueNames[ev.value]
		);
	}
}
