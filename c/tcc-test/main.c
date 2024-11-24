#include <err.h>
#include <errno.h>
#include <libtcc.h>
#include <stdio.h>
#include <stdlib.h>

int relocated = 0;

int main() {
	TCCState* state = tcc_new();
	tcc_set_output_type(state, TCC_OUTPUT_MEMORY);

	for(;;) {
		fputs("C:\\> ", stderr);
		errno = 0;
		char*   buf = NULL;
		size_t  n   = 0;
		ssize_t len = getline(&buf, &n, stdin);
		if(len < 0) {
			free(buf);
			if(errno != 0) err(1, "Could not read line");
			else exit(0);
		}
		buf[len - 1] = 0;

		if(buf[len - 2] == ';') {
			if(tcc_compile_string(state, buf) < 0) errx(1, "Compiler error");
		} else {
			if(!relocated) {
				printf("relo: %d\n", tcc_relocate(state, TCC_RELOCATE_AUTO));
				relocated = 1;
			}
			void* sym = tcc_get_symbol(state, buf);
			printf("%p\n", sym);
		}

		free(buf);
	}
	tcc_delete(state);
}
