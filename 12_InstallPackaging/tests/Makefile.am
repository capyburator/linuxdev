TESTS = tests
check_PROGRAMS = tests

.ts.c:
	checkmk $< > $@

AM_CFLAGS = -I$(top_builddir) @CHECK_CFLAGS@
LDADD = @CHECK_LIBS@
