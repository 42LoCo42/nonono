#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct termios term_attrs;

void disableRawMode() {
	tcsetattr(0, TCSAFLUSH, &term_attrs);
	printf("\nRaw mode disabled.\n");
}

void rawMode() {
	tcgetattr(0, &term_attrs);
	struct termios raw = term_attrs;
	raw.c_iflag &= ~(IXON | ICRNL); // no C-s and C-q; no C-m/Return translation
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); // don't echo input; no canonical mode; receive C-z; receive C-v
	// IEXTEN seems to be covered by ISIG, at least on my terminal
	tcsetattr(0, TCSAFLUSH, &raw);
	atexit(disableRawMode);
	printf("Raw mode enabled. Type anything to see raw bytes. C-d exits.\n");
}

int main() {
	rawMode();
	setvbuf(stdout, NULL, _IONBF, 0);
	char c;
	while (read(0, &c, 1) == 1 && c != 4) {
		printf("%d ", c);
	}
	return 0;
}
