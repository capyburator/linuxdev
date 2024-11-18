CC = gcc
CFLAGS = -Wall -Werror -Wextra -O2 -fsanitize=undefined

all: maze maze_v2 maze_v3 maze_v4
	./maze
	./maze_v2 8
	./maze_v3 .o 8
	./maze_v4 42 .@ 8

maze_v2.c:
	patch -o $@ < v2.patch

maze_v3.c:
	patch -o $@ < v3.patch

maze_v4.c:
	patch -o $@ < v4.patch

clean:
	rm -f maze maze_v2 maze_v3 maze_v4 *.o maze_v2.c maze_v3.c maze_v4.c
