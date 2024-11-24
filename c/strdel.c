#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int delete(char* str, size_t pos, size_t n) {
	size_t len = strlen(str);
	if(pos >= len) {
		return 2;
	}
	size_t src = n + pos;
	if(src > len) {
		return 1;
	}
	memmove(str + pos, str + src, len - src);
	str[len - n] = 0;
	return 0;
}

int main() {
	char foo[] = "0123456789";
	printf("%d\n", delete(foo, 5, 5));
	printf("%s\n", foo);
}
