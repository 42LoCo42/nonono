#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE
#include <dirent.h>
#include <ftw.h>
#include <stdio.h>
#include <string.h>

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	int rv = remove(fpath);
	if(rv) perror(fpath);
	return rv;
}

int rmrf(char *path) {
	return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

int main() {
	DIR* here = opendir(".");
	if(here == NULL) perror("opendir");
	for(;;) {
		struct dirent* ent = readdir(here);
		if(ent == NULL) break;

		if(ent->d_type == DT_DIR) {
			if(strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0) {
				continue;
			}

			rmrf(ent->d_name);
		}
	}
}
