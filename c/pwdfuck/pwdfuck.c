#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define check(name, cond) if(cond) {perror(name); exit(1);}
#define SOFT_NAME_MAX (NAME_MAX - 20)

char current[NAME_MAX + 1] = {};
size_t len = 0;

void add(char c);
void back();
void emit();

void add(char c) {
	current[len++] = c;
	if(len == NAME_MAX) {
		emit();
		back();
	}
}

void back() {
	add('');
	add('[');
	add('D');
}

void emit() {
	if(len == 0) {
		add(' ');
		back();
	}

	current[len] = 0;
	len = 0;
	mkdir(current, 0755); // ignore error, the folder might already exist
	check("chdir", chdir(current) < 0);
}

int main(int argc, char** argv) {
	if(argc < 3) {
		printf("Usage: %s <name file> <next program> [args]\n", argv[0]);
		return 1;
	}

	FILE* name_file = NULL;
	check("fopen", (name_file = fopen(argv[1], "r")) == NULL);

	char c;
	while((c = fgetc(name_file)) != EOF) {
		if(c == '/') {
			emit();
		} else {
			// escape sequences must not be broken, so they have a soft cap
			if(c == '' && len >= SOFT_NAME_MAX) {
				emit();
				back();
			}

			add(c);
		}
	}

	emit();
	fclose(name_file);

	check("execvpe", execvp(argv[2], &argv[2]) < 0);
}
