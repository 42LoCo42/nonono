#include <stdio.h>
#include <stdlib.h>
#include "new.h"

int main() { deferWrap {

	defer {
		puts("second");
	}

	defer {
		puts("first");
	}
}}
