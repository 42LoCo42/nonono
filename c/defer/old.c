#include <stdio.h>
#include <stdlib.h>
#include "defer.h"

deferfn(int, main(int argc, char** argv), {
	if(argc < 2) return 1;

	int input = atol(argv[1]);
	defer(printf("exit\n"));

	if(input > 51) {
		defer(printf("input ist zu groß baka\n"));
		ReturnVal(255);
	}
	retval = input * 5;
	defer(printf("kalkulation erfolgreich!\n"));
	ReturnVal(retval);
});
