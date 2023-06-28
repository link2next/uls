#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ULS], [1.8], [link2next@gmail.com])
AC_CONFIG_SRCDIR([ulscpp/UlsLex.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])
AC_CHECK_SIZEOF([void *])
AC_CHECK_SIZEOF([double])
AC_CHECK_SIZEOF([long double])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_YACC

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_mutex_init])

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([fcntl.h libintl.h limits.h locale.h malloc.h])
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h wchar.h ])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_LONG_DOUBLE

AC_CONFIG_MACRO_DIR([m4])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit dup2 getcwd memmove memset mkdir modf setlocale])
AC_CHECK_FUNCS([strchr strcasecmp strdup strerror strstr strrchr strtol strtoul])
AC_CHECK_FUNCS([readlink realpath])

AC_PATH_PROG(HELP2MAN, help2man, false)

AC_ARG_WITH([etc-dir],
  [AS_HELP_STRING([--with-etc-dir=<dirpath>],
    [use this option to specify the directory where uls.sysprops resides.])],
  [
    ax_etc_dir=$withval
  ],
  [
    ax_etc_dir=/usr/local/etc/uls
  ])

AC_SUBST([ax_etc_dir])

AC_CONFIG_FILES([Makefile
	ulsconfig/Makefile
	ulc_exam/Makefile
	src/Makefile
	ulscompat/Makefile
	ulscpp/Makefile
	tools/Makefile
	tools/ulstools/Makefile
	tools/ulc2class/Makefile
	tools/ulf_gen/Makefile
	tools/uls_stream/Makefile
	doc/Makefile
	doc/man/Makefile
	tests/Makefile
	tests/cpp_hello/Makefile
	tests/dump_toks/Makefile
	tests/id_stat/Makefile
	tests/getch/Makefile
	tests/line/Makefile
	tests/log/Makefile
	tests/many_kwrds/Makefile
	tests/nest/Makefile
	tests/strings/Makefile
	tests/tok_remap/Makefile
	tests/two_lex/Makefile
	tests/test_tree/Makefile
	tests/gcc_toks/Makefile
	examples/Makefile
	examples/test_1st/Makefile
	examples/tokseq/Makefile
	examples/yacc/Makefile
	examples/DumpToks/Makefile
	examples/Css3/Makefile
	examples/Html5/Makefile
	examples/Shell/Makefile
	examples/Mkf/Makefile
	examples/Natural/Makefile
	examples/ulc2yaml/Makefile
	post_inst/Makefile
])

AC_OUTPUT
