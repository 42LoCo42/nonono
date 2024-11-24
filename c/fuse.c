#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <unistd.h>
#include <linux/fuse.h>
#include <poll.h>

#define err(x) {perror(x); goto err;}
#define errx(...) {fprintf(stderr, __VA_ARGS__); goto err;}

#define MOUNTPOINT "/home/leonsch/dev/c/fuse/mnt"

char* opcodeToName(uint32_t opcode) {
	switch(opcode) {
		case FUSE_INIT:       return "init";
		case FUSE_GETATTR:    return "getattr";
		case FUSE_ACCESS:     return "access";
		case FUSE_OPEN:       return "open";
		case FUSE_OPENDIR:    return "opendir";
		case FUSE_READ:       return "read";
		case FUSE_READDIR:    return "readdir";
		case FUSE_INTERRUPT:  return "interrupt";
		case FUSE_LOOKUP:     return "lookup";
		case FUSE_FLUSH:      return "flush";
		case FUSE_RELEASE:    return "release";
		case FUSE_RELEASEDIR: return "releasedir";
		case FUSE_STATFS:     return "statfs";
		default:              return "unknown";
	}
}

void printFuseHeader(struct fuse_in_header* header) {
	printf("FUSE header:\nOperation: %s\nUID: %lu\nFSID: %lu\nUID: %d\nGID: %d\nPID: %d\n",
		opcodeToName(header->opcode), header->unique, header->nodeid,
		header->uid, header->gid, header->pid
	);
}

int main() {
	int fusefd = open("/dev/fuse", O_RDWR);
	if(fusefd < 0) err("open fusefd");
	printf("got fusefd %d\n", fusefd);

	if(mount(
		"source",
		MOUNTPOINT,
		"fuse.testing",
		MS_NOSUID | MS_NODEV,
		"fd=3,rootmode=47777,user_id=0,group_id=0"
	) < 0) err("mount fusefd");

	printf("Ready.\n");

	struct fuse_in_header header = {};
	for(;;) {
		size_t expect = sizeof(header);
		ssize_t sbytes = read(fusefd, &header, expect);
		if(sbytes < 0) err("Error while reading header");
		if(sbytes == 0) errx("EOF while reading header\n")
		size_t bytes = sbytes;
		if(bytes != expect)
			errx("Incorrect bytes: got %lu, expect %lu\n", bytes, expect);
		printFuseHeader(&header);
	}

err:
	printf("Shutting down\n");
	umount2(MOUNTPOINT, MNT_DETACH);
	close(fusefd);
}
