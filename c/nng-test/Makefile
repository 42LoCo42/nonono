all: main multiws

main: main.c utils.c
	gcc main.c utils.c -Wall -Wextra -lnng -o $@

multiws: multiws.c utils.c
	gcc multiws.c utils.c -Wall -Wextra -lnng -o $@
