#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	for(int i = 0; i < 134; ++i) {
		printf("%3d %-15s %s\n", i, strerrorname_np(i), strerror(i));
	}
}
