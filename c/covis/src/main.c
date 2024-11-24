#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "termcontrol.h"
#include "dynbuf.h"

void die(const char* s) {
	perror(s);
	exit(1);
}

void exit_func() {
	tctl_reset();
}

void test_read_file() {
	int person = open("person.txt", O_RDONLY);
	if(person == -1) {
		die("Unable to open file!");
	}

	// get file size
	struct stat statbuf;
	if(fstat(person, &statbuf) < 0) {
		close(person);
		die("Unable to stat file!");
	}
	size_t file_size = statbuf.st_size;

	// read file into dynbuf
	dynbuf_make(char, person_texture);
	dynbuf_resize(person_texture, file_size);
	size_t bytes_read_total = 0;
	do {
		size_t needed_bytes = file_size - bytes_read_total;
		size_t buf_len = needed_bytes > 5 ? 5 : needed_bytes;
		char* buf = malloc(buf_len);
		if(buf == NULL) {
			close(person);
			die("Unable to allocate memory!");
		}

		ssize_t bytes_read = read(person, buf, buf_len);
		if(bytes_read < 0) {
			close(person);
			free(buf);
			die("Unable to read file!");
		}

		dynbuf_write(person_texture, bytes_read_total, buf, bytes_read);
		bytes_read_total += bytes_read;
		free(buf);
	} while(bytes_read_total < file_size);

	close(person);
	dynbuf_append(person_texture, "\0", 1);

	printf("%s", dynbuf_at(person_texture, 0));
	printf("%ld\n", strlen(dynbuf_at(person_texture, 0)));
	dynbuf_free(person_texture);
}

int test_raw_input() {
	atexit(exit_func);
	if(tctl_load() == -1) {
		die("Could't load terminal settings");
	}
	tctl_enable_raw_term();
	tctl_set_vtime(1);
	if(tctl_apply() == -1) {
		die("Couldn't modify terminal settings");
	}

	char c;
	while(1) {
		switch(read(STDIN_FILENO, &c, 1)) {
		case -1:
			if(errno != EAGAIN) {
				die("main: read");
			}
			break;
		case 0:
			c = 0;
			break;
		}
		printf("%d ", c);
		if(!iscntrl(c)) {
			printf("%c ", c);
		}
		fflush(stdout);
	}
	return 0;
}

struct foo {
	int a;
	int b;
	int c;
};


void print_foobuf(struct foo* d) {
	for(size_t i = 0; i < dynbuf_count(d); ++i) {
		struct foo* foo = dynbuf_at(d, i);
		printf("%lu: a %d b %d c %d\n", i, foo->a, foo->b, foo->c);
	}
}

void test_dynbuf() {
	struct foo some_data[] = {{41,42,43},{44,45,46},{47,48,49}};

	dynbuf_make(struct foo, store);
	dynbuf_resize(store, 10);
	for(size_t i = 0; i < dynbuf_count(store); ++i) {
		struct foo* foo = dynbuf_at(store, i);
		foo->a = i * 3;
		foo->b = i * 3 + 1;
		foo->c = i * 3 + 2;
	}
	print_foobuf(store);
	printf("\n");

	dynbuf_append(store, some_data, 3);
	print_foobuf(store);
	printf("\n");

	dynbuf_delete(store, 1, 2);
	print_foobuf(store);
	printf("\n");

	dynbuf_free(store);
}

int main(int argc, char** argv) {
	(void) argc;
	(void) argv;

	test_read_file();
}
