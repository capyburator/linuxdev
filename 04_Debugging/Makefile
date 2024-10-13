CC = gcc
CFLAGS = -Wall -Werror -Wextra -O0 -g

all: range

range: range.o

test: test1 test2

test1: range
	@OUT=$$(mktemp) && \
	gdb -x debug/test1.gdb --batch --quiet ./range | grep @@@ > $$OUT && \
	cmp $$OUT debug/test1_out.txt ; \
	rm -f $$OUT

test2: range
	@OUT=$$(mktemp) && \
	gdb -x debug/test2.gdb --batch --quiet ./range | grep @@@ > $$OUT && \
	cmp $$OUT debug/test2_out.txt ; \
	rm -f $$OUT

clean:
	rm -f *.o range
