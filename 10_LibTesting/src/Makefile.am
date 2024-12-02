CFLAGS = -Wall -Wextra
if COND_GCOV
CFLAGS += -O0 -g -fprofile-arcs -ftest-coverage
else
CFLAGS += -O2 -g3
endif

lib_LTLIBRARIES = libbuf.la

libbuf.c: buf.h

gcov:
	gcov -o .libs libbuf -t -k -b | egrep -v '[[:digit:]]:[ /][*]'
