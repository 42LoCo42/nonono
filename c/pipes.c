#define _XOPEN_SOURCE 500
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>

#define BENDINESS 25

typedef enum {
	UP,
	LEFT,
	DOWN,
	RIGHT,
	FINAL,
} direction;

typedef struct {
	int x;
	int y;
	direction d;
	int color;
} mypipe;

const char* chars[] = {
	// Characters for pipe drawing.
	// Order of table:
	// columns: old direction
	// rows:    new direction

	//  UP   LEFT DOWN RIGHT
		"┃", "┗", "!", "┛", // UP
		"┓", "━", "┛", "!", // LEFT
		"!", "┏", "┃", "┓", // DOWN
		"┏", "!", "┗", "━", // RIGHT
};

int width, height;

int ranCap(int cap) {
	// Returns a random integer between 0 (inclusive) and cap (exclusive)
	return random() % cap;
}

int ranBtw(int min, int max) {
	// Returns a random integer between min and max (both inclusive)
	return ranCap(max - min + 1) + min;
}

void getTermSize(int* width, int* height) {
	// Writes the terminal size (columns, rows) to (width, height)
	struct winsize size = {};
	if(ioctl(STDIN_FILENO, TIOCGWINSZ, &size) < 0) {
		perror("Could not get terminal size");
		exit(1);
	}

	*width = size.ws_col;
	*height = size.ws_row;
}

void turnRand(direction* d) {
	// Randomly rotates the given direction by 0°, 90° or -90°
	int turn = ranCap(BENDINESS);
	if(turn == 0) turn = FINAL - 1;
	else if(turn > 1) turn = 0;
	*d = (*d + turn) % FINAL;
}

int dirDelta(direction d, unsigned int check, unsigned int mod) {
	// Returns a position change of either 1 or -1 in the modulus mod.
	// check is used to determine if the direction has changed the position.
	return
		d % 2 == check ? 0
		: d > 1 ? 1 : mod - 1;
}

mypipe startPipe() {
	// Creates & returns a random pipe
	mypipe p = {
		.x = ranBtw(1, width),
		.y = ranBtw(1, height),
		.d = ranCap(4),
		.color = ranBtw(31, 37),
	};
	return p;
}

void extendPipe(mypipe* p) {
	// Draws the next character of the given pipe

	// move cursor to current position & print color
	printf("[%d;%dH[1;%dm",
		p->y + 1, p->x + 1, p->color);

	// turn towards a new direction
	direction old = p->d;
	turnRand(&p->d);

	// print pipe character and movement
	printf("%s", chars[p->d << 2 | old]);

	// update pipe position
	p->x = (p->x + dirDelta(p->d, 0, width)) % width;
	p->y = (p->y + dirDelta(p->d, 1, height)) % height;
}

void cleanup() {
	// clear screen, go to upper left corner, show cursor
	printf("[2J[H[?25h");
	exit(1);
}

int main() {
	// init PRNG
	struct timeval now = {};
	gettimeofday(&now, NULL);
	srandom(now.tv_sec * now.tv_usec);

	// get terminal size
	getTermSize(&width, &height);

	// clear screen, hide cursor, register cleanup
	printf("[2J[?25l");
	signal(SIGINT, cleanup);

	// create all pipes
	const size_t count = 30;
	mypipe pipes[count];
	for(size_t i = 0; i < count; ++i) {
		pipes[i] = startPipe();
	}

	// extend pipes indefinitely
	for(int i = 0;; ++i) {
		for(size_t i = 0; i < count; ++i) {
			extendPipe(&pipes[i]);
		}

		// clear screen after a number of cycles
		if(i >= 500) {
			i = 0;
			printf("[2J");
		}

		fflush(stdout);
		usleep(25000);
	}
}
