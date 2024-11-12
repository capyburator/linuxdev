.PHONY:	clean checksha1 checkmd5
CFLAGS = -Wall -Werror -Wextra -O2

bin_PROGRAMS = rhasher

rhasher_SOURCES = rhasher.c

TESTS=checksha1 checkmd5

define make_test =
    @OUT=$$(mktemp) && \
    CORRECT=$$(mktemp) && \
    CONTENT=abracadabra && \
    echo -n $$CONTENT | $(1) | head -c $(2) > $$CORRECT && \
    echo -n $(3) \"$$CONTENT | ./rhasher | tail -n 2 | head -c $(2) > $$OUT && \
    cmp $$OUT $$CORRECT ; \
    rm -f $$OUT $$CORRECT
endef

checksha1: rhasher
    $(call make_test,sha1sum,40,SHA1)

checkmd5: rhasher
    $(call make_test,md5sum,32,MD5)

clean:
	rm -rf rhasher *.o *.cache *.m4 compile config.h *.in configure *.log *.status depcomp install-sh stamp-h1 test-driver missing *~ .deps
