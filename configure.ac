# Process this file with autoconf to produce a configure script.
AC_INIT([juken], [0.1a])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
# (AC_HEADER_STDC dropped -- obsolete, every supported compiler is C89+)
AC_CHECK_HEADERS([stdlib.h unistd.h stddef.h string.h errno.h])
AC_CHECK_HEADERS([libintl.h])
AC_CHECK_HEADERS([stdarg.h])
AC_CHECK_HEADERS([sys/select.h])
AC_CHECK_HEADERS([fcntl.h termios.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_LIB_READLINE
save_LIBS="$LIBS"
test x"${with_readline-no}" != xno && LIBS="$LIBS -lreadline"
AC_CHECK_FUNCS(rl_callback_handler_install)
AC_CHECK_FUNCS(rl_attempted_completion_function)
AC_CHECK_FUNCS(rl_attempted_completion_over)
AC_CHECK_FUNCS(add_history)
LIBS="$save_LIBS"

# adl_FUNC_GETOPT_LONG dropped -- it wired a FreeBSD-only gnugetopt
# fallback (lib/gnugetopt.h) that was never actually vendored in this
# tree, so automake refused to generate (required file not found).
# getopt_long has been in glibc for decades; just check for it, same
# as the macro would've resolved to on every platform this still
# targets. cli/*.cpp already guard the call with #ifdef HAVE_GETOPT_LONG.
AC_CHECK_FUNCS([getopt_long])

# AC_PROG_GCC_TRADITIONAL/AC_TYPE_SIGNAL/AC_DECL_SYS_SIGLIST dropped --
# obsolete pre-ANSI-C / signal-typedef workarounds, unused by any
# source file in this tree (grepped for RETSIGTYPE and sys_siglist
# consumers -- none outside the already-unbuilt common/strsignal.c).
AC_FUNC_VPRINTF
# AC_FUNC_MALLOC dropped -- its GNU-malloc(0)-returns-non-NULL
# replacement (common/malloc.c) was never vendored either, and no
# source file checks HAVE_MALLOC; glibc's malloc(0) is well-behaved
# anyway. Plain existence check instead.
AC_CHECK_FUNCS([malloc])
AC_CHECK_FUNCS([select strdup memset usleep])
AC_REPLACE_FUNCS(strerror)
AC_REPLACE_FUNCS(cfmakeraw)

CFLAGS="-DDEBUG -g"
CXXFLAGS="$CFLAGS"
AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)

AC_CONFIG_FILES([Makefile cli/Makefile lib/Makefile common/Makefile])
AC_OUTPUT

echo \
"------------------------------------------------------------------------
Configuration:

Source code location:       ${srcdir}
Compiler:                   ${CC}
Compiler flags:             ${CFLAGS}
Host System Type:           ${host}
Install path:               ${prefix}
See config.h for further configuration information.
------------------------------------------------------------------------"
