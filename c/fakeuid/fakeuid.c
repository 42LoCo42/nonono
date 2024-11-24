#include <unistd.h>

#define ROOT 0

uid_t getuid(void) {
	return ROOT;
}

uid_t geteuid(void) {
	return ROOT;
}

gid_t getgid(void) {
	return ROOT;
}

gid_t getegid(void) {
	return ROOT;
}
