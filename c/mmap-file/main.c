#include <stdio.h>
#include <stdlib.h>

#include "mmap-file.h"

int main() {
	// map file
	map_file_mode mode = MAP_FILE_WRITE_DISK;
	mapped_file thing = {};
	if(mapFile("random_file", &thing, mode) < 0) {
		printf("File is empty!\n");
		exit(1);
	}

	// work with the file
	printf("%s", thing.data);
	printf("File size in bytes: %ld\n", thing.size);

	// try modification
	if(mode != MAP_FILE_READONLY) {
		thing.data[0] = '!';
		printf("%s", thing.data);
	}

	// try syncing
	syncFile(&thing, MS_SYNC);

	// unmap file - don't forget this or your programm will memleak
	unmapFile(&thing);
}
