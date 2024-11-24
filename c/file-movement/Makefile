##
# file-movement
#
# @file
# @version 0.1

main: main.c
	gcc -O2 -g main.c -o main

run: main
	./main

test: main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./main

# end
