#ifndef _MMAP_FILE_H
#define _MMAP_FILE_H

#include <sys/mman.h>

typedef struct mapped_file {
	int fd;     // file descriptor
	off_t size; // number of bytes
	char* data; // null-terminated contents of file
} mapped_file;

typedef enum map_file_mode {
	MAP_FILE_READONLY,  // data can't be modified (causes SEGFAULT)
	MAP_FILE_WRITE_RAM, // modified data won't be saved to file
	MAP_FILE_WRITE_DISK // modified data will  be saved to file
} map_file_mode;

int mapFile(const char* path, mapped_file* file, map_file_mode mode);
int syncFile(mapped_file* file, int flags);
void unmapFile(mapped_file* file);

#endif // _MMAP_FILE_H
