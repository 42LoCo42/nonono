#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define check(name, cond) if(cond) {perror(name); _exit(1);}

int _start() {
	int fd;
	check("open", (fd = open("/proc/sys/vm/drop_caches", O_WRONLY)) < 0);
	check("write", write(fd, "3", 1) != 1);
	check("close", close(fd) < 0);
	check("open", (fd = open("/proc/sys/vm/compact_memory", O_WRONLY)) < 0);
	check("write", write(fd, "1", 1) != 1);
	check("close", close(fd) < 0);
	_exit(0);
}
