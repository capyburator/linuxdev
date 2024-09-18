CC = gcc
CFLAGS = -Wall -Werror -Wextra -O2 -std=c99 -fsanitize=undefined,address
LDFLAGS = -lncurses
NAME = Show

all: Show.c
	$(CC) $(CFLAGS) $< -o $(NAME) $(LDFLAGS)

clean:
	rm -f *.o $(NAME)
