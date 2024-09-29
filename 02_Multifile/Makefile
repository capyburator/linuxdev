.PHONY:	clean
.SECONDARY:	fun.o const.o
.INTERMEDIATE:	liboutput_static.a(fun.o const.o)

CC = gcc
CFLAGS = -Wall -Werror -Wextra -fPIC -O2
LDFLAGS = -L.

all:	prog prog-a prog-so

prog:	prog.o fun.o const.o

prog-a:	prog.o liboutput_static.a
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -loutput_static

prog-so:	prog.o liboutput.so
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -loutput

liboutput_static.a:	liboutput_static.a(fun.o const.o)

liboutput.so:	fun.o const.o
	$(CC) $(CFLAGS) $^ -shared -o $@

fun.o prog.o:	outlib.h

test:	all testrunner
	@LD_LIBRARY_PATH=$(PWD) ./testrunner
	@LD_LIBRARY_PATH=$(PWD) ./testrunner arg
	@LD_LIBRARY_PATH=$(PWD) ./testrunner abra cad abra
	@rm -f testrunner.o testrunner

testrunner:	testrunner.o

clean:
	rm -f *.o *.a *.so prog prog-a prog-so testrunner
