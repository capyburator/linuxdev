AC_PREREQ([2.71])
AC_INIT([rhasher], [0.1.0], [da_ilichev@mail.ru])
AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_CONFIG_SRCDIR([rhasher.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([rhash],[rhash_library_init],[],[AC_MSG_ERROR([librhash is needed])])
AC_CHECK_LIB([readline],[readline])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([rhash.h])
AC_CHECK_HEADERS([readline/readline.h])

AC_ARG_ENABLE([readline],
    AS_HELP_STRING([--enable-readline], [Enable reading line via readline]),
    AC_DEFINE(_READLINE,[],[Readline enabled])
)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
