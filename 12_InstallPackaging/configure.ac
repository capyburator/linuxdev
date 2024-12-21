#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([GUESS-NUMBER], [0.1.0], [])
AM_INIT_AUTOMAKE([foreign])
DX_INIT_DOXYGEN([guess-number])
LT_INIT([disable-static])
AC_CONFIG_SRCDIR([guess-number.c])
AC_CONFIG_HEADERS([config.h])

# Joint pkgconfig library/include tests
PKG_CHECK_MODULES([CHECK],[check])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.


# Variable definitins
AC_SUBST(CK_VERBOSITY, verbose)
AC_ARG_VAR(CK_VERBOSITY, [Default: "verbose", can be "silent", "minimal" or "normal"])

# Checks for header files.
AC_CHECK_HEADERS([libintl.h strings.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

AC_ARG_ENABLE([gcov],
              [AS_HELP_STRING([--enable-gcov], [use Gcov to test the test suite])],
              [], [enable_gcov=no])
AM_CONDITIONAL([COND_GCOV],[test '!' "$enable_gcov" = no])

# Checks for library functions.
AC_CHECK_FUNCS([memmove memset setlocale strstr])

AC_CONFIG_FILES([Makefile Doxyfile tests/Makefile])
AC_OUTPUT
