#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "mmap-file.h"

#define die(msg, ...) {__VA_ARGS__;} perror(msg); exit(1)
// die if exit status is less than zero (typical error)
#define dlz(code, msg, ...) if(code < 0) {die(msg, __VA_ARGS__);}

int mapFile(const char* path, mapped_file* file, map_file_mode mode) {
	// get flags
	int open_flag = O_RDONLY;
	int mmap_prot = PROT_READ;
	int mmap_flag = MAP_PRIVATE;
	if(mode != MAP_FILE_READONLY) {
		mmap_prot |= PROT_WRITE;
	}
	if(mode == MAP_FILE_WRITE_DISK) {
		open_flag = O_RDWR;
		mmap_flag  = MAP_SHARED;
	}

	// open file
	file->fd = open(path, open_flag);
	dlz(file->fd, "open");

	// get size
	struct stat statbuf;
	dlz(fstat(file->fd, &statbuf), "fstat", close(file->fd));
	file->size = statbuf.st_size;

	// check if file is empty
	// not critical, but it can't be mapped if so
	if(file->size < 1) {
		close(file->fd);
		return -1;
	}

	// map file
	file->data = mmap(NULL, file->size + 1, mmap_prot, mmap_flag, file->fd, 0);
	if(file->data == MAP_FAILED) {
		close(file->fd);
		die("mmap");
	}

	// everything went well
	return 0;
}

int syncFile(mapped_file* file, int flags) {
	return msync(file->data, file->size, flags);
}

void unmapFile(mapped_file* file) {
	dlz(munmap(file->data, file->size + 1), "munmap", close(file->fd));
	dlz(close(file->fd), "close");
}
