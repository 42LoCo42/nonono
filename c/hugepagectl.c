#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#define die(...) err(1, __VA_ARGS__)

#define O_ERR "Could not open file %s"
#define W_ERR "Could not write to file %s"

#define DROP_CACHES_FILE "/proc/sys/vm/drop_caches"
#define COMPACT_MEMORY_FILE "/proc/sys/vm/compact_memory"
#define HUGEPAGES_FILE "/sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages"

void safe_write(const char* path, const char* str) {
	FILE* file = fopen(path, "w");
	if(file == NULL) {
		die(O_ERR, path);
	}
	size_t len = strlen(str);
	size_t ret = fwrite(str, sizeof(char), len, file);
	if(ret < len) {
		die(W_ERR, path);
	}
	fclose(file);
}

void set_hugepages(const char* str) {
	printf("Setting nr_hugepages to %s...\n", str);
	safe_write(HUGEPAGES_FILE, str);
}

int main(int argc, char** argv) {
	if(argc < 2) {
		set_hugepages("0");
		exit(0);
	}

	// drop caches
	printf("Dropping caches...\n");
	safe_write(DROP_CACHES_FILE, "3");

	// compact memory
	printf("Compacting memory...\n");
	safe_write(COMPACT_MEMORY_FILE, "1");

	// set hugepage count
	set_hugepages(argv[1]);
}
