CC = gcc
CFLAGS = -Wall -Werror -Wextra -O2

all: move protect_remove

move: move.c

protect_remove.so: protect_remove.c
	$(CC) $(CFLAGS) $^ -shared -o $@

test: test-open test-read test-write test-protect clean

define test_strace =
	@IN=$$(mktemp) && \
	OUT=$$(cat /dev/urandom | tr -cd 'a-f0-9' | head -c 8) && \
	echo "abracadabra" > $$IN && \
	strace -e fault=$(1):error=$(2):when=$(3) ./move $$IN $$OUT > /dev/null 2>&1 ; \
	[ $$? -eq $(4) ] && [ -f $$IN ] && [ ! -f $$OUT ] && \
	rm -f $$IN $$OUT
endef

test-open: move
	$(call test_strace,openat,EACCES,3,2)
	$(call test_strace,openat,EACCES,4,3)

test-read: move
	$(call test_strace,read,EINVAL,2,4)

test-write: move
	$(call test_strace,write,EINVAL,1,5)

test-protect: move protect_remove.so
	@IN=$$(mktemp)_PROTECT && \
	OUT=$$(cat /dev/urandom | tr -cd 'a-f0-9' | head -c 8) && \
	echo "abracadabra" > $$IN && \
	LD_PRELOAD=./protect_remove.so ./move $$IN $$OUT > /dev/null 2>&1 ; \
	[ $$? -eq 0 ] && [ -f $$IN ] && [ -f $$OUT ] && \
	rm -f $$IN $$OUT

clean:
	rm -f *.o move protect_remove.so
