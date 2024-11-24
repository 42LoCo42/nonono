#include <stdio.h>
#include <unistd.h>

int main() {
	setbuf(stdout, NULL);
	for(int i = 0; i < 10; ++i) {
		printf("yes\n");
		usleep(1e6);
	}
}
