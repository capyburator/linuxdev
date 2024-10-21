CC = gcc
CFLAGS = -Wall -Werror -Wextra -O2 -fsanitize=undefined,address

all: esub

esub: esub.c

check: test1 test2 test3

test1: esub
	@OUT=$$(mktemp) && \
	CORRECT=$$(mktemp) && \
	./esub "ab" "BA" "eabc" > $$OUT && \
	echo "eabc" | sed -E 's/ab/BA/' > $$CORRECT && \
	cmp $$OUT $$CORRECT ; \
	rm -f $$OUT ; \
	rm -f $$CORRECT

test2: esub
	@OUT=$$(mktemp) && \
	CORRECT=$$(mktemp) && \
	./esub "a(b)c" "\1_\1" "DabcE" > $$OUT && \
	echo "DabcE" | sed -E 's/a(b)c/\1_\1/' > $$CORRECT && \
	cmp $$OUT $$CORRECT ; \
	rm -f $$OUT ; \
	rm -f $$CORRECT

test3: esub
	@OUT=$$(mktemp) && \
	CORRECT=$$(mktemp) && \
	./esub "(a)b(c)d(e)" "\3\2\1" "_abcdef_" > $$OUT && \
	echo "_abcdef_" | sed -E 's/(a)b(c)d(e)/\3\2\1/' > $$CORRECT && \
	cmp $$OUT $$CORRECT ; \
	rm -f $$OUT ; \
	rm -f $$CORRECT

clean:
	rm -f *.o esub
