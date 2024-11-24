#include "fcntl.h"
#include "unistd.h"
#define _GNU_SOURCE

#include <dirent.h>
#include <err.h>
#include <linux/sched.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>

#define die(...) err(1, __VA_ARGS__)

void ls(DIR* dir) {
	struct dirent* entry = NULL;
	while((entry = readdir(dir)) != NULL) {
		printf("[%s] ", entry->d_name);
	}
	puts("");
	closedir(dir);
}

void cat(const char* path) {
	int fd = open(path, O_RDONLY);
	if(fd < 0) die("open %s", path);

	char buf[8192] = {0};
	for(;;) {
		ssize_t ret = read(fd, buf, sizeof(buf));
		if(ret < 0) die("read %s", path);
		if(ret == 0) break;

		printf("%.*s", (int) ret, buf);
	}

	puts("##############################");
	close(fd);
}

void mkdirp(const char* path) {
	char* tmp = strdup(path);

	for(char* it = tmp;;) {
		it = strchr(it, '/');
		if(it == NULL) break;

		*it = 0;
		mkdir(tmp, 0755);
		*it = '/';
		it++;
	}

	mkdir(tmp, 0755);

	free(tmp);
}

void echo(const char* text, const char* file) {
	int fd = open(file, O_WRONLY);
	if(fd < 0) die("open %s", file);
	if(write(fd, text, strlen(text)) < 0) die("write %s to %s", text, file);
	close(fd);
}

int manual_mount(const char* fstype) {
	int fscontext = fsopen(fstype, 0);
	if(fscontext < 0) die("fsopen %s", fstype);

	if(fsconfig(fscontext, FSCONFIG_CMD_CREATE, NULL, NULL, 0) < 0)
		die("fsconfig create");

	int fs = fsmount(fscontext, 0, 0);
	if(fs < 0) die("fsmount");

	close(fscontext);
	return fs;
}

void child() {
	echo("1000 1000 1", "/proc/self/uid_map");
	echo("deny", "/proc/self/setgroups");
	echo("100 100 1", "/proc/self/gid_map");

	int tmpfs = manual_mount("tmpfs");
	if(move_mount(tmpfs, "", AT_FDCWD, "/", MOVE_MOUNT_F_EMPTY_PATH) < 0)
		die("move_mount");
	ls(opendir("/"));
	// if(fchdir(tmpfs) < 0) die("fchdir");

	// TODO mkdir & mount everything
	// if(mkdir("proc", 0555) < 0) die("mkdir proc");
	// if(mount("proc", "proc", "proc", 0, NULL) < 0) die("mount proc");

	if(chroot(".") < 0) die("chroot");

	/* if(syscall(SYS_pivot_root, "mnt", "mnt") < 0) die("pivot_root"); */
	/* if(chdir("/") < 0) die("chdir /"); */

	// if(umount2("/", MNT_DETACH) < 0) die("umount /");

	/* cat("/proc/mounts"); */
	/* ls(opendir("/")); */
	/* ls(opendir("/proc")); */

	/* printf("%d %d\n", getuid(), getgid()); */
}

int main(void) {
	struct clone_args cl_args = {
		.flags       = CLONE_NEWUSER | CLONE_NEWNS | CLONE_NEWPID,
		.exit_signal = SIGCHLD,
	};
	pid_t pid = syscall(SYS_clone3, &cl_args, sizeof(cl_args));
	switch(pid) {
	case -1:
		die("clone");
	case 0:
		child();
		break;
	default:
		if(wait(NULL) < 0) die("wait");
	}
}
